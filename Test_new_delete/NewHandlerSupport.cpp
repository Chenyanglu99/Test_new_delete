#include "NewHandlerSupport.h"
#include  "NewHandlerHolder.h"
template<typename T>
std::new_handler NewHandlerSupport<T>::currenthandler = 0;

template <typename T>
std::new_handler NewHandlerSupport<T>::set_new_handler(std::new_handler p) throw()
{
	std::new_handler old_new_handler = currenthandler;
	currenthandler = p;
	return old_new_handler;
}

template <typename T>
void* NewHandlerSupport<T>::operator new(size_type size) throw(std::bad_alloc)
{
	NewHandlerHolder handler_holder(std::set_new_handler(currenthandler));
	return ::operator new(size);
}