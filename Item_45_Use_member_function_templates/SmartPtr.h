#pragma once
#include <iostream>
//支持隐形转换smartPtr<U> --> SmartPtr<T>
//
template <typename T>
class SmartPtr
{
public:
    //智能指针通常以内置原始指针完成初始化
    explicit SmartPtr(T* realptr);
    //根据有SmartPtr<U>构造SmartPtr<T>
    template<typename U>
    SmartPtr(const SmartPtr<U>& other)
        : heldPtr(other.get()) {}

    T* get() { return heldPtr; }
private:
    T* heldPtr; //这个smartPtr所持有的内置原始指针
};

//只有U*能转为T*时，才能通过编译
