#ifndef MYTINYSTL_ALLOCATOR_H_
#define MYTINYSTL_ALLOCATOR_H_

#include "construct.h"
#include "destroy.h"
#include "util.h"

namespace mystl
{
    template <typename T>
    class allocator
    {
    public:
        typedef T value_type;
        typedef T *pointer;
        typedef const T *const_pointer;
        typedef T &reference;
        typedef T &&right_reference;
        typedef const T &const_reference;
        typedef size_t size_type;
        //通常用来保存两个指针相减的结果，是有符号整数
        typedef ptrdiff_t difference_type;

    public:
        static pointer allocate();
        static pointer allocate(size_type n);

        static void deallocate(pointer ptr);
        static void deallocate(pointer ptr, size_type n);

        static void construct(pointer ptr);
        static void construct(pointer ptr, const T &value);
        static void construct(pointer ptr, right_reference value);

        template <typename... Args>
        static void construct(pointer ptr, Args &&...args);

        static void destroy(pointer ptr);
        static void destroy(pointer first, pointer last);
    };
    template <typename T>
    T *allocator<T>::allocate()
    {
        return static_cast<pointer>(::operator new(sizeof(value_type)));
    }
    template <typename T>
    T *allocator<T>::allocate(size_type n)
    {
        return static_cast<pointer>(::operator new(n * sizeof(value_type)));
    }
    template <typename T>
    void allocator<T>::deallocate(T *ptr)
    {
        if (ptr == nullptr)
            return;
        ::operator delete(ptr);
    }
    template <typename T>
    void allocator<T>::deallocate(T *ptr, size_t /*size*/)
    {
        if (ptr == nullptr)
            return;
        ::operator delete(ptr);
    }
    template <typename T>
    void allocator<T>::construct(T *ptr)
    {
        mystl::construct(ptr);
    }
    template <typename T>
    void allocator<T>::construct(T *ptr, const T &value)
    {
        mystl::consruct(ptr, value);
    }
    template <typename T>
    void allocator<T>::construct(T *ptr, T &&value)
    {
        mystl::construct(ptr, mystl::move(value));
    }
    template <typename T>
    template <typename... Args>
    void allocator<T>::construct(T *ptr, Args &&...args)
    {
        mystl::construct(ptr, mystl::forward<Args>(args)...);
    }
    template <typename T>
    void allocator<T>::destroy(T *ptr)
    {
        mystl::destroy(ptr);
    }
    /*根据迭代器来析构，需要先完成迭代器类
    template <typename T>
    void allocator<T>::destroy(T *first, T *last)
    {
        mystl::destroy(first, last);
    }
    */
} //namespace mystl

#endif