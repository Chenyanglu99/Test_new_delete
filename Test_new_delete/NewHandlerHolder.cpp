#include "NewHandlerHolder.h"

NewHandlerHolder::~NewHandlerHolder()
{
	//重新安装
	std::set_new_handler(handler);
}