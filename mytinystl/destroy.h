#ifndef MYTINYSTL_DESTROY_H_
#define MYTINYSTL_DESTROY_H_

// 这个头文件包含函数destroy，负责对象的析构
#include <new>
#include <type_traits>

namespace mystl
{
    template <typename T>
    void destroy(T *ptr)
    {
        // 先判断是否是普通可破坏类型
        if (is_trivially_destructible<int>::value)
            return;
        if (ptr != nullptr)
        {
            ptr->~T();
        }
    }
    /*应该先完成迭代器类
    template <typename T>
    void destroy(T *first, T *second)
    {
    }
    */
}
#endif