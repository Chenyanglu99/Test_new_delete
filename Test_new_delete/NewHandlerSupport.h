#pragma once
#include <new>
#include <iostream>
template<typename T>
class NewHandlerSupport
{
public:
	typedef size_t size_type;
	static std::new_handler set_new_handler(std::new_handler p) throw();
	static void* operator new(size_type) throw(std::bad_alloc);
private:
	static std::new_handler currenthandler;
};
