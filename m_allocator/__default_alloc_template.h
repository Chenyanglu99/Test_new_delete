#pragma once
template <bool threads, int inst>
class __default_alloc_template {
private:
	// Really we should use static const int x = N
	// instead of enum { x = N }, but few compilers accept the former.
	enum { __ALIGN = 8 };
	enum { __MAX_BYTES = 128 };
	enum { __NFREELISTS = __MAX_BYTES / __ALIGN };
	//ROUND_UP将bytes上调至8的倍数
	//__ALIGN = 00001000 __ALIGN-1 = 00000111
	//~(__ALIGN - 1) = 11111000
	//如果bytes是8的倍数-> bytes = xxxxx000
		// xxxxx000 + 00000111 = xxxxx111
		//~(__ALIGN - 1) = 11111000
			//xxxxx000还是原来的样子
	//如果bytes不是8的倍数-> 8n < bytes < 8(n+1)
	//bytes = 8n + p, p < 8
	//p存在低3位，8n+p必定向第四位进1，8n+p在截断后三位就是8(n+1);
	static size_t ROUND_UP(size_t bytes) {
		return (((bytes)+__ALIGN - 1) & ~(__ALIGN - 1));
	}
private:
	union obj {	//free-lists的节点构造
		union obj * free_list_link;
		char client_data[1];    /* The client sees this.        */
	};
private:
	//16个free-lists
	static obj * volatile free_list[__NFREELISTS];
	//函数根据区块的大小，决定使用第n号free-list.n从1起算
	static  size_t FREELIST_INDEX(size_t bytes) {
		return (((bytes)+__ALIGN - 1) / __ALIGN - 1);
	}
	// 返回一个大小为n的对象，并可能加入大小为n的其他区块到free list
	static void *refill(size_t n);
	//配置一大块空间，可容纳nobjs个大小为"size"的区块
	//如果配置nobjs个区块有所不便，nobjs可能会降低
	static char *chunk_alloc(size_t size, int &nobjs);
	// Chunk allocation state.
	static char *start_free;  //内存池起始地址
	static char *end_free;	//内存池结束地址
	static size_t heap_size;
public:
	/* n must be > 0      */
	struct stl {
		static void * allocate(size_t n)
		{
			obj * __volatile * my_free_list;
			obj * __restrict result;

			if (n > (size_t)__MAX_BYTES) {
				return(malloc_alloc::allocate(n));
			}
			my_free_list = free_list + FREELIST_INDEX(n);
			// Acquire the lock here with a constructor call.
			// This ensures that it is released in exit or during stack
			// unwinding.
			result = *my_free_list;
			if (result == 0) {
				void *r = refill(ROUND_UP(n));
				return r;
			}
			*my_free_list = result->free_list_link;
			return (result);
		};

		/* p may not be 0 */
		static void deallocate(void *p, size_t n)
		{
			obj *q = (obj *)p;
			obj * __volatile * my_free_list;

			if (n > (size_t)__MAX_BYTES) {
				malloc_alloc::deallocate(p, n);
				return;
			}
			my_free_list = free_list + FREELIST_INDEX(n);
			q->free_list_link = *my_free_list;
			*my_free_list = q;
			// lock is released here
		}

		static void * reallocate(void *p, size_t old_sz, size_t new_sz);
	};

	struct yc_stl
	{
		static void * allocate(size_t n)
		{
			//取出free list上可用的区块
			obj* __volatile *my_free_list;
			obj* result;

			//判断不是大于128字节了
			if (n > static_cast<size_t>(__MAX_BYTES))
			{
				//调用一级配置器
				return malloc_alloc::allocate(n);
			}
			my_free_list = free_list + FREELIST_INDEX(n);
			//取出
			result = *my_free_list;
			if (result == 0)
			{//取出失败
				void* r = refill(ROUND_UP(n));
				return r;
			}
			//调整free list
			(*my_free_list) = result->free_list_link;
			return result;
		}

		//空间释放函数
		static void deallocate(void*p, size_t n)
		{
			obj* q = static_cast<obj*>(p);
			obj* __volatile * my_free_list;
			//如果大于128就调用一级配置器
			if (n > (size_t)__MAX_BYTES)
			{
				malloc_alloc::deallocate(p, n);
				return;
			}

			//寻找对应的free list
			my_free_list = free_list + FREELIST_INDEX(n);
			//回收区块
			q->free_list_link = *my_free_list;
			*my_free_list = q;
		}
	};
};

/**
 * \brief 当allocate发现free list中没有可用区块了时，
 * 就调用refill()，准备为free list重新填充空间，新的空间取自内存池，（经由chunk_alloc）完成
 * 缺省取得20个新节点，但万一内存池空间不够，获得的节点数可能小于20；
 * \param n size_t
 * \return void*
 */
template <bool threads, int inst>
void* __default_alloc_template<threads, inst>::refill(size_t n)
{
	int nobjs = 20;

	obj* __volatile *my_free_list;
	obj* result;
	obj* current_obj, *next_obj;
	int i;
	//调用chunk_alloc()尝试取得nobjs个区块作为free list的新节点
	char* chunk = chunk_alloc(n, nobjs);
	//chunk的内存空间是连续的
	//如果只获得一个区块，这个区块就直接分给调用者使用，free list无新节点
	if (1 == nobjs)
	{
		return static_cast<void*>(chunk); //这一块准备返回给客端
	}
	else
	{//否则准备调整free list,纳入新节点
		my_free_list = free_list + FREELIST_INDEX(n);
		//以下在chunk空间内建立free list
		//第一块准备返回给客端
		result = (obj*)chunk;
		//以下导引free list指向新配置的空间
		*my_free_list = next_obj = (obj*)(chunk + n);
		//以下将free list的各节点串接起来
		for (i = 1;; i++)
		{
			current_obj = next_obj;
			
			next_obj = (obj*)((char*)next_obj + n);
			if(nobjs - 1 == i)
			{
				current_obj->free_list_link = 0;
				break;
			}
			else
			{
				current_obj->free_list_link = next_obj;
			}
		}
	}
	return result;
}

template <bool threads, int inst>
char*
__default_alloc_template<threads, inst>::chunk_alloc(size_t size, int& nobjs)
{
	char * result;
	size_t total_bytes = size * nobjs;
	//内存池剩余空间bytes_left
	size_t bytes_left = end_free - start_free;

	if (bytes_left >= total_bytes) {
		//内存池剩余空间完全满足需求量
		result = start_free;
		start_free += total_bytes;
		return(result);
	} else if (bytes_left >= size) {
		//内存池剩余空间不能完全满足需求量,但足够供应一个（含）以上的区块-->bytes_left >= size
		//内存池剩余空间bytes_left / 要分配的区块大小size
		nobjs = bytes_left/size;
		//int& nobjs 是引用
		total_bytes = size * nobjs;
		result = start_free;
		start_free += total_bytes;
		return(result);
	} else {
		//1. 榨干内存池当前空间
		//2. 补充内存池
			//2.1 补充成功，更新内存池的属性，递归调用chunk_alloc()
			//2.2 补充失败，
				//第一步，试着挖掘free list中比size大的区块空间；//挖掘成功，更新内存池的属性，递归调用chunk_alloc()
				//如果不成功，第二步，尝试调用第一级
					//一级补充成功，更新内存池的属性，递归调用chunk_alloc()
		//内存池剩余空间连一块区块的大小都无法提供
		//榨干内存池，零头先配给适当的free list
		size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
		// Try to make use of the left-over piece.
		//bytes_left > 0 内存池剩余空间bytes_left
		if (bytes_left > 0) {
			obj * __VOLATILE * my_free_list =
						free_list + FREELIST_INDEX(bytes_left);
			//调整free list, 将内存池中的残余空间编入
			((obj *)start_free) -> free_list_link = *my_free_list;
			*my_free_list = (obj *)start_free;
		}
		//当bytes_left = 0时
		//配置heap空间，用来补充内存池
		start_free = (char *)malloc(bytes_to_get);

		//补充失败的情况
		if (0 == start_free) {
			//heap空间不足，malloc失败
			int i;
			obj * __VOLATILE * my_free_list, *p;
			// Try to make do with what we have.  That can't
			// hurt.  We do not try smaller requests, since that tends
			// to result in disaster on multi-process machines.
			for (i = size; i <= __MAX_BYTES; i += __ALIGN) {
				//一个个的来，将free list内尚有未用区块
				my_free_list = free_list + FREELIST_INDEX(i);
				p = *my_free_list;
				if (0 != p) {
					//调整free list以释出未用区块
					*my_free_list = p -> free_list_link;
					start_free = (char *)p;
					end_free = start_free + i;
					//递归调用自己，为了修正nobjs
					//内存池经过挖掘有空间了
					return(chunk_alloc(size, nobjs));
					// Any leftover piece will eventually make it to the
					// right free list.
				}
			}
			//到处都没有内存可用，free list中没有足够大的内存块
			end_free = 0;	// In case of exception.
			//调用第一级配置器
			start_free = (char *)malloc_alloc::allocate(bytes_to_get);
			// This should either throw an
			// exception or remedy the situation.  Thus we assume it
			// succeeded.
		}
		
		heap_size += bytes_to_get;
		end_free = start_free + bytes_to_get;
		return(chunk_alloc(size, nobjs));
	}
}