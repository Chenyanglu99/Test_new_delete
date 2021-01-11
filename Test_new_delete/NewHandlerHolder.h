#pragma once
#include <new>
class NewHandlerHolder
{
public:
	explicit NewHandlerHolder(std::new_handler nh) :
		handler(nh) { } //保存资源

	~NewHandlerHolder();
private:
	std::new_handler handler; //记录当前handler

	NewHandlerHolder(const NewHandlerHolder&);
	NewHandlerHolder& operator=(const NewHandlerHolder&);
};
