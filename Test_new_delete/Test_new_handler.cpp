#include "Test_new_handler.h"
#include "NewHandlerHolder.h"

#if Test_class_Widget

std::new_handler Widget::current_handler = 0;

/**
 * \brief 指定Widget类专属的new_handler, 就像std::set_new_handler一样
 * 指定global的new_handler
 * \param p 设置
 * \return 返回旧的new_handler
 */
std::new_handler Widget::set_new_handler(std::new_handler p) throw()
{
	std::new_handler old_handler = current_handler;
	current_handler = p;
	return old_handler;
}

/**
 * \brief 确保在分配Widget对象内存的过程中以class专属之new_handler替换 global new_handler
 * \param size
 * \return
 */
void * Widget::operator new(size_type size) throw(std::bad_alloc)
{
	//利用资源管理类管理
	//确保原本的new_handler总是被重新安装

	//1. 替换 global new_handler
	//2. 为确保当发生异常时，
	//new_handler会被还原成global_handler,用资源管理对象来管理global new_handler
	//3. 调用真正的内存分配函数
	NewHandlerHolder(std::set_new_handler(current_handler));
	return (::operator new(size));

	//当分配内存完成时，将global new_handler函数重新安装，因为
	//NewHandlerHolder会调用析构函数
	//确保谁改变，谁复原
}

#endif

#if Test_class_Widget_From_NewHandlerSupport

#endif