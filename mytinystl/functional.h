#ifndef MYTINYSTL_FUNCTIONAL_H_
#define MYTINYSTL_FUNCTIONAL_H_

// 这个头文件包含了mystl的函数对象与哈希函数

#include <cstddef>

namespace mystl
{

    /*******************************************************************************/
    // 哈希函数对象
    // 对于大部分类型，hash function什么都不做
    template <class Key>
    struct hash
    {
    };
    // 针对指针的偏特化版本
    template <class T>
    struct hash<T *>
    {
        size_t operator()(T *p) cosnt noexcept
        {
            return reinterpret_cast<size_t>(p);
        }
    };

    // 对于浮点数，逐位哈希
    inline size_t bitwise_hash(const unsigned char *first, size_t count)
    {
#if (_MSC_VER && _WIN64) || ((__GNUC__ || __clang__) && __SIZEOF_POINTER__ == 8)
        const size_t fnv_offset = 14695981039346656037ull;
        const size_t fnv_prime = 1099511628211ull;
#else
        const size_t fnv_offset = 2166136261u;
        const size_t fnv_prime = 16777619u;
#endif
        size_t result = fnv_offset;
        for (size_t i = 0; i < count; ++i)
        {
            result ^= (size_t)first[i];
            result *= fnv_prime;
        }
        return result;
    }
    template <>
    struct hash<float>
    {
        size_t operator()(const float &val)
        {
            return val == 0.0f ? 0 : bitwise_hash((const unsigned char *)&val, sizeof(float));
        }
    };

    template <>
    struct hash<double>
    {
        size_t operator()(const double &val)
        {
            return val == 0.0f ? 0 : bitwise_hash((const unsigned char *)&val, sizeof(double));
        }
    };

    template <>
    struct hash<long double>
    {
        size_t operator()(const long double &val)
        {
            return val == 0.0f ? 0 : bitwise_hash((const unsigned char *)&val, sizeof(long double));
        }
    };

}

#endif