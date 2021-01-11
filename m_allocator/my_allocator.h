#pragma once

#include <cstddef>
#include <new>
#include <iostream>
namespace test_my_namespace
{
	template <class T>
	inline T* _allocate(ptrdiff_t size, T*)
	{
		std::set_new_handler(0);

		T* tmp = (T*)(::operator new((size_t)(size * sizeof(T))));

		if (!tmp)
		{
			std::cerr << "out of memory " << std::endl;
			exit(1);
		}

		return tmp;
	}

	template <class T>
	inline void _deallocate(T* buffer)
	{
		::operator delete(buffer);
	}
	template<class T1, class T2>
	inline void _construct(T1* p, const T2& value)
	{
		new(p) T1(value);
	}

	template<class T>
	inline void _destroy(T* ptr)
	{
		ptr->~T();	//调用T的析构函数
	}
	
	template<typename T>
	class my_allocator
	{
	public:
		typedef T value_type;
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef T& reference;
		typedef const T& const_reference;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;

		//rebind allocator of type U
		//??????????????????????????????
		template<class U>
		struct rebind
		{
			typedef my_allocator<U> other;
		};
		pointer allocate(size_type n, const void* hint = 0)
		{
			return _allocate((difference_type)n, (pointer)0);
		}

		void deallocate(pointer p, size_type n)
		{
			_deallocate(p);
		}

		//construct ??

		void construct(pointer p, const T& value)
		{
			_construct(p, value);
		}


		void destroy(pointer p)
		{
			_destroy(p);
		}

		pointer adress(reference x)
		{
			return static_cast<pointer>(&x);
		}

		const_pointer const_address(const_reference x)
		{
			return static_cast<const_pointer>(&x);
		}

		size_type max_size() const
		{
			return static_cast<size_type>(UINT64_MAX / sizeof(T));
		}
	};
}

/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 *
 * Copyright (c) 1996,1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

 /* NOTE: This is an internal header file, included by other STL headers.
  *   You should not attempt to use it directly.
  */

