#pragma once

#include <iostream>
template <typename T>
class stl_list
{
public:
    void insert_front(T value);
    void insert_end(T value);
    void display(std::ostream &os = std::cout) const;
    //...
private:
};

template<typename T>
class ListItem
{
public:
    T value() const { return _value; }
    ListItem* next() const { return _next; }
private:
    T _value;
    ListItem* _next;
};

template <class Item>
struct ListIter
{
    Item* ptr; //保持与容器之间的一个容器
    ListIter(Item* p = 0)
        : ptr(p) { }

    //copy ctor, operator= 缺省行为已经足够
    Item& operator*() const { return *ptr; }
    Item* operator->() const { return ptr; }

    //以下两个operator++遵循标准做法
    //pre-increment operator
    ListIter& operator++()
    {
        ptr = ptr->next(); return *this;
    }

    //post increment operator
    ListIter operator++(int)
    {
        ListIter tmp = *this; ++*this; return tmp;
    }

    bool operator==(const ListIter& i) const
    {
        return ptr == i.ptr;
    }
    bool operator!=(const ListIter& i) const
    {
        return ptr != i.ptr;
    }
};
