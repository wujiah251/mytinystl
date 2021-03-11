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
    template <>
    struct char_traits<char>
    {
        typedef char char_type;

        static size_t length(const char_type *str) noexcept
        {
            return std::strlen(str);
        }

        static int compare(const char_type *s1, const char_type *s2, size_t n) noexcept
        {
            return std::memcmp(s1, s2, n);
        }

        static char_type *copy(char_type *dst, const char_type *src, size_t n) noexcept
        {
            MYSTL_DEBUG(src + n <= dst || dst + n <= src);
            return static_cast<char_type *>(std::memcpy(dst, src, n));
        }

        static char_type *move(char_type *dst, const char_type *src, size_t n) noexcept
        {
            return static_cast<char_type *>(std::memmove(dst, src, n));
        }

        static char_type *fill(char_type *dst, char_type ch, size_t count) noexcept
        {
            return static_cast<char_type *>(std::memset(dst, ch, count));
        }
    };
    template <>
    struct char_traits<wchar_t>
    {
        typedef wchar_t char_type;

        static size_t length(const char_type *str) noexcept
        {
            return std::wcslen(str);
        }

        static int compare(const char_type *s1, const char_type *s2, size_t n) noexcept
        {
            return std::wmemcmp(s1, s2, n);
        }

        static char_type *copy(char_type *dst, const char_type *src, size_t n) noexcept
        {
            MYSTL_DEBUG(src + n <= dst || dst + n <= src);
            return static_cast<char_type *>(std::wmemcpy(dst, src, n));
        }

        static char_type *move(char_type *dst, const char_type *src, size_t n) noexcept
        {
            return static_cast<char_type *>(std::wmemmove(dst, src, n));
        }

        static char_type *fill(char_type *dst, char_type ch, size_t count) noexcept
        {
            return static_cast<char_type *>(std::wmemset(dst, ch, count));
        }
    };
    template <>
    struct char_traits<char16_t>
    {
        typedef char16_t char_type;

        static size_t length(const char_type *str) noexcept
        {
            size_t len = 0;
            for (; *str != char_type(0); ++str)
                ++len;
            return len;
        }

        static int compare(const char_type *s1, const char_type *s2, size_t n) noexcept
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

        static char_type *copy(char_type *dst, const char_type *src, size_t n) noexcept
        {
            MYSTL_DEBUG(src + n <= dst || dst + n <= src);
            char_type *r = dst;
            for (; n != 0; --n, ++dst, ++src)
                *dst = *src;
            return r;
        }

        static char_type *move(char_type *dst, const char_type *src, size_t n) noexcept
        {
            char_type *r = dst;
            if (dst < src)
            {
                for (; n != 0; --n, ++dst, ++src)
                    *dst = *src;
            }
            else if (src < dst)
            {
                dst += n;
                src += n;
                for (; n != 0; --n)
                    *--dst = *--src;
            }
            return r;
        }

        static char_type *fill(char_type *dst, char_type ch, size_t count) noexcept
        {
            char_type *r = dst;
            for (; count > 0; --count, ++dst)
                *dst = ch;
            return r;
        }
    };
    template <>
    struct char_traits<char32_t>
    {
        typedef char32_t char_type;

        static size_t length(const char_type *str) noexcept
        {
            size_t len = 0;
            for (; *str != char_type(0); ++str)
                ++len;
            return len;
        }

        static int compare(const char_type *s1, const char_type *s2, size_t n) noexcept
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

        static char_type *copy(char_type *dst, const char_type *src, size_t n) noexcept
        {
            MYSTL_DEBUG(src + n <= dst || dst + n <= src);
            char_type *r = dst;
            for (; n != 0; --n, ++dst, ++src)
                *dst = *src;
            return r;
        }

        static char_type *move(char_type *dst, const char_type *src, size_t n) noexcept
        {
            char_type *r = dst;
            if (dst < src)
            {
                for (; n != 0; --n, ++dst, ++src)
                    *dst = *src;
            }
            else if (src < dst)
            {
                dst += n;
                src += n;
                for (; n != 0; --n)
                    *--dst = *--src;
            }
            return r;
        }

        static char_type *fill(char_type *dst, char_type ch, size_t count) noexcept
        {
            char_type *r = dst;
            for (; count > 0; --count, ++dst)
                *dst = ch;
            return r;
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
        typedef typename allocator_type::const_pointer const_pointer;
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
        // 末尾位置的值，例：
        // if(str.find('a')!=string::npos){/* do something */}
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
        // 构造函数n个字符ch
        basic_string(size_type n, value_type ch)
            : buffer_(nullptr), size_(0), capacity_(0)
        {
            fill_init(n, ch);
        }
        // 构造函数
        basic_string(const basic_string &other, size_type pos)
            : buffer_(nullptr), size_(0), capacity_(0)
        {
            init_from(other.buffer_, pos, other.size_ - pos);
        }
        // 构造函数
        basic_string(const basic_string &other, size_type pos, size_type count)
            : buffer_(nullptr), size_(0), capacity_(0)
        {
            init_from(other.buffer_, pos, count);
        }
        // 构造函数，根据C风格字符串复制
        basic_string(const_pointer str)
            : buffer_(nullptr), size_(0), capacity_(0)
        {
            init_from(str, 0, char_traits::length(str));
        }
        // 复制count个字符
        basic_string(const_pointer str, size_type count)
            : buffer_(nullptr), size_(0), capacity_(0)
        {
            init_from(str, 0, count);
        }
        // 通过迭代器复制
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
        basic_string(basic_string &&rhs) noexcept
            : buffer_(rhs.buffer_), size_(rhs.size_), capacity_(rhs.capacity_)
        {
            rhs.buffer = nullptr;
            rhs.size_ = 0;
            rhs.capacity_ = 0;
        }
        // 复制函数重载
        // 通过basic_string左值复制
        basic_string &operator=(const basic_string &rhs);
        // 通过basic_string右值复制
        basic_string &operator=(basic_string &&rhs) noexcept;
        // 通过C字符串复制
        basic_string &operator=(const_pointer str);
        // 通过字符复制
        basic_string &operator=(value_type ch);

    public:
        // 迭代器相关
        iterator begin() noexcept
        {
            return buffer_;
        }
        const_iterator begin() const noexcept
        {
            return buffer_;
        }
        iterator end() noexcept
        {
            return buffer_ + size_;
        }
        const_iterator end() const noexcept
        {
            return buffer_ + size_;
        }

        reverse_iterator rbegin() noexcept
        {
            return reverse_iterator(end());
        }
        const_reverse_iterator rbegin() const noexcept
        {
            return const_reverse_iterator(end());
        }
        reverse_iterator rend() noexcept
        {
            return reverse_iterator(begin());
        }
        const_reverse_iterator rend() const noexcept
        {
            return const_reverse_iterator(begin());
        }

        const_iterator cbegin() const noexcept
        {
            return begin();
        }
        const_iterator cend() const noexcept
        {
            return end();
        }
        const_reverse_iterator crbegin() const noexcept
        {
            return rbegin();
        }
        const_reverse_iterator crend() const noexcept
        {
            return rend();
        }

        // 容量相关操作
        bool empty() const noexcept
        {
            return size_ = 0;
        }
        size_type size() const noexcept
        {
            return size_;
        }
        size_type length() const noexcept
        {
            return size_;
        }
        size_type capacity() const noexcept
        {
            return capacity_;
        }
        size_type max_size() const noexcept
        {
            return static_cast<size_type>(-1);
        }
        void reserve(size_type n);

        // 访问元素相关操作
        reference operator[](size_type n)
        {
            MYSTL_DEBUG(n <= size_);
            if (n == size_)
                *(buffer_ + n) = value_type();
            return *(buffer_ + n);
        }
        reference operator[](size_type n) const
        {
            MYSTL_DEBUG(n <= size_);
            if (n == size_)
                *(buffer_ + n) = value_type();
            return *(buffer_ + n);
        }
        reference at(size_type n)
        {
            THROW_OUT_OF_RANGE_IF(n >= size_, "basic_string<Char, Traits>::at()"
                                              "subscript out of range");
            return (*this)[n];
        }
        const_reference at(size_type n)
        {
            THROW_OUT_OF_RANGE_IF(n >= size_, "basic_string<Char, Traits>::at()"
                                              "subscript out of range");
            return (*this)[n];
        }
        reference front()
        {
            MYSTL_DEBUG(!empty());
            return *begin();
        }
        reference back()
        {
            MYSTL_DEBUG(!empty());
            return *(end() - 1);
        }
        const_reference back() const
        {
            MYSTL_DEBUG(!empty());
            return *(end() - 1);
        }

        const_pointer data() const noexcept
        {
            return to_raw_pointer();
        }
        const_pointer c_str() const noexcept
        {
            return to_raw_pointer();
        }
        // 添加删除相关操作

        // insert
        iterator insert(const_iterator pos, value_type ch);
        iterator insert(const_iterator pos, size_type count, value_type ch);

        template <class Iter>
        iterator insert(const_iterator pos, Iter first, Iter last);

        // push_back() / pop_back()
        void push_back(value_type ch)
        {
            append(1, ch);
        }
        void pop_back()
        {
            MYSTL_DEBUG(!empty());
            --size_;
        }

        //apend
        basic_string &append(size_type count, value_type ch);

        basic_string &append(const basic_string &str)
        {
            return append(str, 0, str.size_);
        }
        basic_string &append(const basic_string &str, size_type pos)
        {
            return append(str, pos, str.size_ - pos);
        }

        template <class Iter, typename std::enable_if<
                                  mystl::is_input_iterator<Iter>::value, int>::type = 0>
        basic_string &append(Iter first, Iter last)
        {
            return append_range(first, last);
        }

        // erase()和clear()
        iterator erase(cosnt_iterator pos);
        iterator erase(const_iterator first, const_iterator last);

        //resize
        void resize(size_type count)
        {
            resize(count, value_type());
        }
        void reszie(size_type count, value_type ch);

        void clear() noexcept
        {
            size_ = 0;
        }

        //basic_string 相关操作

        // compare
        int compare(const basic_string &other) const;
        int compare(size_type pos1, size_type count1, const basic_string &other) const;
        int compare(size_type pos1, size_type count1, const basic_string &other,
                    size_type pos2, size_type count2 = npos) const;
        int compare(const_pointer s) const;
        int compare(size_type pos1, size_type count1,
                    const_pointer s) const;
        int compare(size_type pos1, size_type count1,
                    const_pointer s, size_type count2) const;

        // substr
        basic_string substr(size_type index, size_type count = npos)
        {
            count = mystl::min(count, size_ - index);
            return basic_string(buffer_ + index, buffer_ + index + count);
        }

        // replace
        basic_string &replace(size_type pos, size_type count, const basic_string &str)
        {
            THROW_OUT_OF_RANGE_IF(pos > size_, "basic_string<Char, Traits>::replace's pos out of range");
            replace_cstr(buffer_ + pos, count, str.buffer_, str.size_);
        }

        // reverse
        void reverse() noexcept;

        // swap
        void swap(basic_string &rhs) noexcept;

        // 查找操作

        // find
        size_type find(value_type ch, size_type pos = 0) const noexcept;
        size_type find(const basic_string &str, size_type pos = 0) const noexcept;

        // rfind
        size_type rfind(value_type ch, size_type pos = npos) const noexcept;
        size_type rfind(const basic_string &str, size_type pos = npos) const noexcept;

        // 计数：count
        size_type count(value_type ch, size_type pos = 0) const noexcept;

    public:
        //重载 operator+=
        basic_string &operator+=(const basic_string &str)
        {
            return append(str);
        }
        basic_string &operator+=(value_type ch)
        {
            return append(1, ch);
        }

        //重载 operator>>和operator<<
        friend std::istream &operator>>(std::istream &is, basic_string &str)
        {
            value_type *buf = new value_type[4096];
            is >> buf;
            basic_string tmp(buf);
            str = std::move(tmp);
            delete[] buf;
            return is;
        }
        friend std::istream &operator>>(std::istream &os, basic_string &str)
        {
            for (size_type i = 0; i < str.size_; ++i)
                os << *(str.buffer_ + i);
            return os;
        }

    private:
        // 辅助函数
        // 初始化和销毁
        void try_init() noexcept;
        void fill_init(size_type n, value_type ch);
        template <class Iter>
        void copy_init(Iter first, Iter last, mystl::input_iterator_tag);
        template <class Iter>
        void copy_init(Iter first, Iter last, mystl::forward_iterator_tag);
        void init_from(const_pointer src, size_type pos, size_type n);
        void destroy_buffer();

        // get raw pointer
        const_pointer to_raw_pointer() const;

        // shrink_to_fit
        // 其实就是重新插入一遍，长度、容量变为size
        void reinsert(size_type size);
        // append
        template <class Iter>
        basic_string &append_range(Iter first, Iter last);

        // compare
        int compare_cstr(const_pointer s1, size_type n1,
                         const_pointer s2, size_type n2) const;
        // replace
        basic_string &replace_cstr(const_iterator first, size_type count1, const_pointer str, size_type count2);

        // reallocate
        void reallocate(size_type need);
        iterator reallocate_and_fill(iterator pos, size_type n, value_type ch);
        iterator reallocate_and_copy(iterator pos, const_iterator first, const_iterator last);
    };
    /*************************************************/
    // 赋值函数
    template <class CharType, class CharTraits>
    basic_string<CharType, CharTraits> &
    basic_string<CharType, CharTraits>::
    operator=(const basic_string &rhs)
    {
        if (this != &rhs)
        {
            basic_string tmp(rhs);
            swap(tmp);
        }
        return *this;
    }

    // 移动赋值构造函数
    template <class CharType, class CharTraits>
    basic_string<CharType, CharTraits> &
    basic_string<CharType, CharTraits>::
    operator=(basic_string &&rhs) noexcept
    {
        destroy_buffer();
        buffer_ = rhs.buffer_;
        size_ = rhs.size_;
        capacity_ = rhs.capacity_;
        rhs.buffer_ = nullptr;
        rhs.size_ = 0;
        rhs.capacity_ = 0;
        return *this;
    }

    // 用一个C风格字符串赋值
    template <class CharType, class CharTraits>
    basic_string<CharType, CharTraits> &
    basic_string<CharType, CharTraits>::
    operator=(const_pointer str)
    {
        const size_type len = char_traits::length(str);
        if (capacity_ < len)
        {
            auto new_buffer_ = data_allocator::allocate(len + 1);
            data_allocator::deallocate(buffer_);
            buffer_ = new_buffer_;
            capacity_ = len + 1;
        }
        char_traits::copy(buffer_, str, len);
        size_ = len;
        return *this;
    }

    // 用字符赋值
    template <class CharType, class CharTraits>
    basic_string<CharType, CharTraits> &
    basic_string<CharType, CharTraits>::
    operator=(value_type ch)
    {
        if (capacity_ < 1)
        {
            auto new_buffer_ = data_allocator::allocate(2);
            data_allocator::deallocate(buffer_);
            buffer_ = new_buffer;
            capacity_ = 2;
        }
        *buffer_ = ch;
        size_ = 1;
        return *this;
    }

    // 预留储存空间
    template <class CharType, class CharTraits>
    void basic_string<CharType, CharTraits>::
        reserve(size_type n)
    {
        if (capacity_ < n)
        {
            THROW_LENGTH_ERROR_IF(n > max_size(), "n can not larger than max_size()"
                                                  "in basic_string<Char,Traits>::reserve(n)");
            auto new_buffer = data_allocator::allocate(n);
            char_traits::move(new_buffer, buffer_, size_);
            buffer_ = new_buffer;
            capacity_ = n;
        }
    }

    // 在 pos 处插入一个元素
    template <class CharType, class CharTraits>
    typename basic_string<CharType, CharTraits>::iterator
    basic_string<CharType, CharTraits>::
        insert(const_iterator pos, value_type ch)
    {
        iterator r = const_cast<iterator>(pos);
        if (size_ = capacity_)
        {
            return reallocate_and_fill(r, 1, ch);
        }
        char_traits::move(r + 1, r, end() - r);
        ++size_;
        *r = ch;
        return r;
    }

    // 在pos处插入n个元素
    template <class CharType, class CharTraits>
    typename basic_string<CharType, CharTraits>::iterator
    basic_string<CharType, CharTraits>::
        insert(const_iterator pos, size_type count, value_type ch)
    {
        iterator r = const_cast<iterator>(pos);
        if (count == 0)
            return r;
        if (capacity_ - size_ < count)
        {
            return reallocate_and_fill(r, count, ch);
        }
        if (pos == end())
        {
            char_traits::fill(end(), ch, count);
            size_ += count;
            return r;
        }
        char_traits::move(r + count, r, count);
        char_traits::fill(r, ch, count);
        size_ += count;
        return r;
    }

    // reallocate_and_fill 函数
    // 在pos的位置插入n给ch字符
    template <class CharType, class CharTraits>
    typename basic_string<CharType, CharTraits>::iterator
    basic_string<CharType, CharTraits>::
        reallocate_and_fill(iterator pos, size_type n, value_type ch)
    {
        const auto r = pos - buffer_;
        const auto old_cap = capacity_;
        const auto new_cap = mystl::max(old_cap + n, old_cap + (old_cap >> 1));
        auto new_buffer = data_allocator::allocate(new_cap);
        auto e1 = char_traits::move(new_buffer, buffer_, r) + r;
        auto e2 = char_traits::fill(e1, ch, n) + n;
        char_traits::move(e2, buffer_ + r, size_ - r);
        char_allocator::deallocate(buffer_, old_cap);
        buffer_ = new_buffer;
        size_ += n;
        capacity_ = new_cap;
        return buffer_ + r;
    }

    // append，在末尾添加n个字符ch
    template <class CharType, class CharTraits>
    basic_string<CharType, CharTraits> &
    basic_string<CharType, CharTraits>::
        append(size_type count, value_type ch)
    {
        THROW_LENGTH_ERROR_IF(size_ > max_size() - count,
                              "basic_string<Char, Tratis>'s size too big");
        if (capacity_ - size_ < count)
        {
            reallocate(count);
        }
        char_traits::fill(buffer_ + size_, ch, count);
        size_ += count;
        return *this;
    }

}
#endif