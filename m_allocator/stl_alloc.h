#pragma once
namespace stl_alloc_test
{
#if 0
#   include <new>
#   define __THROW_BAD_ALLOC throw bad_alloc
#elif !defined(__THROW_BAD_ALLOC)
#   include <iostream>
#   define __THROW_BAD_ALLOC cerr << "out of memory" << endl; exit(1)
#endif
/*
//第一级配置器 __malloc_alloc_template剖析
*/
template <int inst>
class __malloc_alloc_template {

private:
/*
//以下都是函数指针，所指向的函数将用来处理内存不足的情况
*/

//oom, out of memory
static void *oom_malloc(size_t);

static void *oom_realloc(void *, size_t);

//函数指针
#ifndef __STL_STATIC_TEMPLATE_MEMBER_BUG
	static void (* __malloc_alloc_oom_handler)();
#endif

public:

static void * allocate(size_t n)
{
	void *result = malloc(n);												//第一级配置器直接使用malloc()
	//以下无法满足需求时，改用oom_malloc;
	if (0 == result) result = oom_malloc(n);
	return result;
}

static void deallocate(void *p, size_t /* n */)
{
	free(p);																//第一级配置器直接使用free()
}

static void * reallocate(void *p, size_t /* old_sz */, size_t new_sz)
{
	void * result = realloc(p, new_sz);										//第一级配置器直接使用realloc()
	if (0 == result) result = oom_realloc(p, new_sz);						//以下无法满足需求时，改用oom_realloc();
	return result;
}
/*
//以下仿真C++的set_new_handler() 换句话说你可以通过它
//指定你直接的out_of_memory handler
*/
static void (* set_malloc_handler(void (*f)()))()							
{
	void (* old)() = __malloc_alloc_oom_handler;
	__malloc_alloc_oom_handler = f;
	return(old);
}

};



//*****************************************//
// malloc_alloc out-of-memory handling
//初值为0，有待客端设定

#ifndef __STL_STATIC_TEMPLATE_MEMBER_BUG
template <int inst>
void (* __malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;
#endif


template <int inst>
void * __malloc_alloc_template<inst>::oom_malloc(size_t n)
{
	void (* my_malloc_handler)();
	void *result;

	for (;;) {													//不断尝试释放，配置，再释放，再配置
		my_malloc_handler = __malloc_alloc_oom_handler;
		if (0 == my_malloc_handler) { __THROW_BAD_ALLOC; }			//cerr << "out of memory" << endl; exit(1)
		
		(*my_malloc_handler)();										//调用处理例程，企图释放内存
		//【1】令更多内存可用
		//【5】终止程序
		//【4】抛出异常
		result = malloc(n);											//再次尝试配置内存
		if (result) return(result);
	}
}

template <int inst>
void * __malloc_alloc_template<inst>::oom_realloc(void *p, size_t n)
{
	void (* my_malloc_handler)();
	void *result;

	for (;;) {													//不断尝试释放，配置，再释放，再配置
		my_malloc_handler = __malloc_alloc_oom_handler; 			//
		if (0 == my_malloc_handler) { __THROW_BAD_ALLOC; }
		(*my_malloc_handler)();										//调用处理例程
		result = realloc(p, n);										//再次尝试配置内存
		if (result) return(result);
	}
}






}





