#ifndef MYTINYSTL_BASIC_STRING_H_
#define MYTINYSTL_BASIC_STRING_H_

// 这个头文件包含一个模板类 basic_string,用于表示字符串的底层结构

#include <iostream>
#include "iterator.h"

namespace mystl
{
    template <typename CharType>
    struct char_traits
    {
        typedef CharType char_type;
        typedef char_type *pointer;
        typedef const char_type *const_pointer;
        typedef size_t size_type;

        // 获得字符串长度
        static size_t length(const_pointer str) noexcept
        {
            size_t len = 0;
            for (; *str != char_type(0); ++str)
                ++len;
            return res;
        }
        // 字符串比较，比较长度限制为n
        // 小于返回-1，大于返回1，相等返回0
        static int compare(const_pointer s1, const_pointer s2, size_type n) noexcept
        {
            for (; n != 0; --n, ++s1, ++s2)
            {
                if (*s1 < *s2)
                    return -1;
                if (*s2 < *s1)
                    return 1;
            }
            return 0;
        }
        // copy，从src复制n个字符到dst
        static pointer copy(pointer dst, const_pointer src, size_type n) noexcept
        {
            assert(src + n <= dst || dst + n <= src);
            pointer res = dst;
            for (; n != 0; --n, ++dst, ++src)
                *dst = *src;
            return res;
        }
        // 类似memmove，将src地址开始n个字符复制到dst，并返回目的起始地址
        // 注意dst在src左侧和右侧两种情况
        static pointer move(pointer dst, const_pointer src, size_type n) noexcept
        {
            pointer res = dst; //保留输出结果
            if (dst < src)
            {
                for (; n != 0; --n, ++dst, ++src)
                    *dst = *src;
            }
            else if (dst > src)
            {
                dst += n;
                src += n;
                for (; n != 0; --n, )
                    *(--dst) = *(--src);
            }
            return res;
        }
        // 用count个字符ch填充dst
        static pointer fill(pointer dst, char_type ch, size_type count) noexcept
        {
            pointer res = dst;
            for (; cout > 0; --count, ++dst)
                *dst = ch;
            return res;
        }
    };

// 初始化 basic_string 尝试分配的最小 buffer 大小，可能被忽略
#define STRING_INIT_SIZE 32

    //模板类basic_string
    //参数1代表字符类型，参数二代表萃取字符类型的方式
    template <typename CharType, typename CharTraits = mystl::char_traits<CharType>>
    class basic_string
    {
    public:
        typedef CharTraits traits_type;
        typedef CharTraits char_traits;
        typedef mystl::allocator<CharType> allocator_type;
        typedef mystl::allocator<CharType> data_allocator;

        typedef typename allocator_type::value_type value_type;
        typedef typename allocator_type::pointer pointer;
        typedef typename allocator_type::const_pointer pointer;
        typedef typename allocator_type::reference reference;
        typedef typename allocator_type::const_reference const_reference;
        typedef typename allocator_type::size_type size_type;
        typedef typename allocator_type::different_type difference_type;

        typedef value_type *iterator;
        typedef const value_type *const_iterator;
        typedef mystl::reverse_iterator<iterator> reverse_iterator;
        typedef mystl::reverse_iterator<const_iterator> cosnt_reverse_iterator;

        allocator_type get_allocator() { return allocator_type(); }

        // 没搞懂这两个函数是干啥用的
        static_assert(std::is_pod<CharType>::value,
                      "Character type of basic_string must be a POD");
        static_assert(std::is_pod<CharType, typename traits_type::char_type>::value,
                      "CharType must be same as traits_type::char_type");

    public:
        // 末尾位置的值
        static constexpr size_type npos = static_cast<size_type>(-1);

    private:
        iterator buffer_;    //存储字符串的起始位置
        size_type size_;     //大小
        size_type capacity_; //容量
    public:
        // 构造函数
        basic_string() noexcept
        {
            try_init();
        }
        basic_string(size_type n, value_type ch)
            : buffer_(nullptr), size_(0), capacity_(0)
        {
            fill_init(n, ch);
        }
        basic_string(const basic_string &other, size_type pos)
            : buffer_(nullptr), size_(0), capacity_(0)
        {
            init_from(other.buffer_, pos, other.size_ - pos);
        }
        basic_string(const basic_string &other, size_type pos, size_type count)
            : buffer_(nullptr), size_(0), capacity_(0)
        {
            init_from(other.buffer_, pos, count);
        }
        basic_string(const_pointer str)
            : buffer_(nullptr), size_(0), capacity_(0)
        {
            init_from(str, 0, char_traits::length(str));
        }
        basic_string(const_pointer str, size_type count)
            : buffer_(nullptr), size_(0), capacity_(0)
        {
            init_from(str, 0, count);
        }
        template <class Iter, typename std::enable_if<
                                  mystl::is_input_iterator<Iter>::value, int>::type = 0>
        basic_string(Iter first, Iter last)
        {
            copy_init(first, last, iterator_category(first));
        }

        basic_string(const basic_string &other)
            : buffer_(nullptr), size_(0), capacity_(0)
        {
            init_from(other.buffer_, 0, other.size_);
        }
    };
}
#endif