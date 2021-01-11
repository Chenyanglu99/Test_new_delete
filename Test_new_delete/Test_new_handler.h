#pragma once
#include <iostream>
#include <new>

//为客户指定class专属new_handler
//举例
//定制专属的set_new_handler
//定制专属的operator new()
#define Test_class_Widget 0
#define Test_class_Widget_From_NewHandlerSupport 1
#if Test_class_Widget

class Widget
{
public:
	typedef std::size_t size_type;
	static std::new_handler set_new_handler(std::new_handler p) throw();
	static void* operator new(size_type size) throw(std::bad_alloc);
private:
	static std::new_handler current_handler;
};

#endif

#if Test_class_Widget_From_NewHandlerSupport
#include "NewHandlerSupport.h"
class Widget : public NewHandlerSupport<Widget>
{
	//每个都拥有其static成员 currenthandler
public:
};

#endif
