#ifndef MYTINYSTL_DESTROY_H_
#define MYTINYSTL_DESTROY_H_

// 这个头文件包含函数destroy，负责对象的析构
#include <new>
#include <type_traits>

namespace mystl
{
    template <typename T>
    void destroy_one(T *, std::true_type) {}

    template <class T>
    void destroy_one(T *pointer, std::false_type)
    {
        if (pointer != nullptr)
        {
            pointer->~Ty();
        }
    }

    template <class T>
    void destroy(T *pointer)
    {
        destroy_one(pointer, std::is_trivially_destructible<T>{});
    }
    /*应该先完成迭代器类
    template <typename T>
    void destroy(T *first, T *second)
    {
    }
    */
}
#endif