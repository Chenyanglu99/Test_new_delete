#pragma once
template<class T>
class simple_auto_ptr
{
public:
    explicit simple_auto_ptr(T* p = 0) : pointee(p) { }
    //泛化拷贝构造函数
    template<class U>
    simple_auto_ptr(simple_auto_ptr<U>& rhs) : pointee(rhs.release()) { }

    template<class U>
    simple_auto_ptr<T>& operator=(simple_auto_ptr<U>& rhs)
    {
        if (this != &rhs) reset(rhs.release());
        return *this;
    }

    T& operator*() const { return *pointee; }
    T* operator->() const { return pointee; }
    T* get() const { return pointee; }
private:
    T* pointee;
};
