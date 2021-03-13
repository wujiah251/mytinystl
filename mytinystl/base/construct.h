#ifndef MYTINYSTL_CONSTRUCT_H_
#define MYTINYSTL_CONSTRUCT_H_

//这个头文件包含函数construct，负责对象的构造

#include <new>

#include "type_traits.h"
#include "iterator.h"

namespace mystl
{
    template <typename T>
    void construct(T *ptr)
    {
        // 在已经分配了内存的地方构建对象
        ::new ((void *)ptr) T();
    }
    template <typename T>
    void construct(T *ptr, const T &value)
    {
        ::new ((void *)ptr) T(value);
    }
    template <typename T, typename T_>
    void construct(T *ptr, T_ &&value)
    {
        // 没搞懂为啥要用T_
        ::new ((void *)ptr) T_(value);
    }
    template <typename T, typename... Args>
    void construct(T *ptr, Args &&...args)
    {
        ::new ((void *)ptr) T(mystl::forward<Args>(args)...);
    }
}

#endif
