#ifndef MYTINYSTL_BASIC_STRING_H_
#define MYTINYSTL_BASIC_STRING_H_

// 这个头文件包含一个模板类 basic_string,用于表示字符串的底层结构

#include <iostream>

#include "base/iterator.h"
#include "base/memory.h"
#include "base/exceptdef.h"
#include "base/functional.h"

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
            return len;
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
                for (; n != 0; --n)
                    *(--dst) = *(--src);
            }
            return res;
        }
        // 用count个字符ch填充dst
        static pointer fill(pointer dst, char_type ch, size_type count) noexcept
        {
            pointer res = dst;
            for (; count > 0; --count, ++dst)
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
        typedef typename allocator_type::const_pointer const_pointer;
        typedef typename allocator_type::reference reference;
        typedef typename allocator_type::const_reference const_reference;
        typedef typename allocator_type::size_type size_type;
        typedef typename allocator_type::difference_type difference_type;

        typedef value_type *iterator;
        typedef const value_type *const_iterator;
        typedef mystl::reverse_iterator<iterator> reverse_iterator;
        typedef mystl::reverse_iterator<const_iterator> const_reverse_iterator;

        allocator_type get_allocator() { return allocator_type(); }

        // 没搞懂这两个函数是干啥用的
        static_assert(std::is_pod<CharType>::value,
                      "Character type of basic_string must be a POD");
        static_assert(std::is_same<CharType, typename traits_type::char_type>::value,
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

        // 拷贝构造函数
        basic_string(const basic_string &other)
            : buffer_(nullptr), size_(0), capacity_(0)
        {
            init_from(other.buffer_, 0, other.size_);
        }
        // 右值拷贝
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
        const_iterator crbegin() const noexcept
        {
            return rbegin();
        }
        const_iterator crend() const noexcept
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
        const_pointer c_str() const noexcept
        {
            return to_raw_pointer();
        }

        // insert
        iterator insert(const_iterator pos, value_type ch);
        iterator insert(const_iterator pos, size_type count, value_type ch);

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

        //append
        basic_string &append(size_type count, value_type ch);
        basic_string &append(value_type ch)
        {
            append(1, ch);
        }
        basic_string &append(const basic_string &str)
        {
            return append(str, 0, str.size_);
        }
        basic_string &append(const basic_string &str, size_type pos, size_type count);
        basic_string &append(const_pointer s)
        {
            return append(s, char_traits::length(s));
        }
        basic_string &append(const_pointer s, size_type count);

        // erase()、resize()、clear()
        iterator erase(const_iterator pos);
        iterator erase(const_iterator first, const_iterator last);

        void resize(size_type count)
        {
            resize(count, value_type());
        }
        void resize(size_type count, value_type ch);

        void clear() noexcept
        {
            size_ = 0;
        }

        //basic_string 相关操作

        // compare
        int compare(const basic_string &other) const;

        // substr
        basic_string substr(size_type index, size_type count = npos)
        {
            count = mystl::min(count, size_ - index);
            return basic_string(buffer_ + index, buffer_ + index + count);
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
        friend std::ostream &operator<<(std::ostream &os, basic_string &str)
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

        // compare
        int compare_cstr(const_pointer s1, size_type n1,
                         const_pointer s2, size_type n2) const;
        // reallocate
        void reallocate(size_type need);
        iterator reallocate_and_fill(iterator pos, size_type n, value_type ch);
        iterator reallocate_and_copy(iterator pos, const_iterator first, const_iterator last);
    };
    /*************************************************/

    /*赋值函数重载*/
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
    // 移动赋值函数
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
            auto new_buffer = data_allocator::allocate(2);
            data_allocator::deallocate(buffer_);
            buffer_ = new_buffer;
            capacity_ = 2;
        }
        *buffer_ = ch;
        size_ = 1;
        return *this;
    }

    /***************************************************************/
    /*插入函数*/
    // 在 pos 处插入一个元素
    template <class CharType, class CharTraits>
    typename basic_string<CharType, CharTraits>::iterator
    basic_string<CharType, CharTraits>::
        insert(const_iterator pos, value_type ch)
    {
        // 调用插入count个字符的情况
        insert(pos, 1, ch);
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
    // 在末尾添加[str[pos:pos+count]一段
    template <class CharType, class CharTraits>
    basic_string<CharType, CharTraits> &
    basic_string<CharType, CharTraits>::
        append(const basic_string &str, size_type pos, size_type count)
    {
        THROW_LENGTH_ERROR_IF(size_ > max_size() - count,
                              "basic_string<Char, Traits>'s size to big");
        if (count == 0)
            return *this;
        if (capacity_ - size_ < count)
        {
            reallocate(count);
        }
        char_traits::copy(buffer_ + size_, str.buffer_ + pos, count);
        size_ += count;
        return *this;
    }
    // 在末尾添加c_str的前count个字符
    template <class CharType, class CharTraits>
    basic_string<CharType, CharTraits> &
    basic_string<CharType, CharTraits>::
        append(const_pointer s, size_type count)
    {
        THROW_LENGHT_ERROR_IF(size_ > max_size() - count,
                              "basic_string<Char, Tratis>'s size too big");
        if (capacity_ - size_ < count)
        {
            reallocate(count);
        }
        char_traits::copy(buffer_ + size_, s, count);
        size_ += count;
        return *this;
    }

    /***************************************************************/
    /*erase*/
    //删除一个迭代器pos指向的元素
    template <class CharType, class CharTraits>
    typename basic_string<CharType, CharTraits>::iterator
    basic_string<CharType, CharTraits>::
        erase(const_iterator pos)
    {
        MYSTL_DEBUG(pos != end());
        iterator r = const_cast<iterator>(pos);
        char_traits::move(r, pos + 1, end() - pos - 1);
        --size_;
        return r;
    }
    // 删除[first,last)的元素
    template <class CharType, class CharTraits>
    typename basic_string<CharType, CharTraits>::iterator
    basic_string<CharType, CharTraits>::
        erase(const_iterator first, const_iterator last)
    {
        if (first == begin() && last == end())
        {
            clear();
            return end();
        }
        const size_type n = end() - last;
        iterator r = const_cast<iterator>(first);
        char_traits::move(r, last, n);
        size_ -= (last - first);
        return r;
    }
    /***************************************************************/
    // 重置容器大小：resize
    // 重置为count个ch
    template <class CharType, class CharTraits>
    void basic_string<CharType, CharTraits>::
        resize(size_type count, value_type ch)
    {
        if (count < size_)
        {
            erase(buffer_ + count, buffer_ + size_);
        }
        else
        {
            append(count - size_, ch);
        }
    }
    /***************************************************************/
    // compare：比较两个basic_string，小于返回-1，大于返回1，等于返回0
    template <class CharType, class CharTraits>
    int basic_string<CharType, CharTraits>::
        compare(const basic_string &other) const
    {
        return compare_cstr(buffer_, size_, other.buffer_, other.size_);
    }
    /***************************************************************/
    // reverse：反转 basic_string
    template <class CharType, class CharTraits>
    void basic_string<CharType, CharTraits>::
        reverse() noexcept
    {
        for (auto i = begin(), j = end(); i < j;)
        {
            mystl::iter_swap(i++, --j);
        }
    }
    /***************************************************************/
    // swap：和另一个rhs交换
    template <class CharType, class CharTraits>
    void basic_string<CharType, CharTraits>::
        swap(basic_string &rhs) noexcept
    {
        if (this != rhs)
        {
            mystl::swap(buffer_, rhs.buffer_);
            mystl::swap(size_, rhs.size_);
            mystl::swap(capacity_, rhs.capacity_);
        }
    }
    /***************************************************************/
    // find：从pos开始查找ch
    template <class CharType, class CharTraits>
    typename basic_string<CharType, CharTraits>::size_type
    basic_string<CharType, CharTraits>::
        find(value_type ch, size_type pos) const noexcept
    {
        for (auto i = pos; i < size_; ++i)
        {
            if (*(buffer_ + i) == ch)
                return i;
        }
        return npos;
    }
    // find：从pos开始查找str
    template <class CharType, class CharTraits>
    typename basic_string<CharType, CharTraits>::size_type
    basic_string<CharType, CharTraits>::
        find(const basic_string &str, size_type pos) const noexcept
    {
        const size_type count = str.size_;
        if (count == 0)
            return pos;
        if (size_ - pos < count)
            return npos;
        const auto left = size_ - count;
        for (auto i = pos; i <= left; ++i)
        {
            if (*(buffer_ + i) == str.front())
            {
                size_type j = 1;
                for (; j < count; ++j)
                {
                    if (*(buffer_ + i + j) != str[j])
                        break;
                }
                if (j == count)
                    return i;
            }
        }
        return npos;
    }
    // rfind：从后往前查找ch
    template <class CharType, class CharTraits>
    typename basic_string<CharType, CharTraits>::size_type
    basic_string<CharType, CharTraits>::
        rfind(value_type ch, size_type pos) const noexcept
    {
        if (pos >= size_)
            pos = size_ - 1;
        for (auto i = pos; i >= 0; --i)
        {
            if (*(buffer_ + i) == ch)
                return i;
        }
        return npos;
    }
    // rfind：从后往前查找str
    template <class CharType, class CharTraits>
    typename basic_string<CharType, CharTraits>::size_type
    basic_string<CharType, CharTraits>::
        rfind(const basic_string &str, size_type pos) const noexcept
    {
        const size_type count = str.size_;
        if (pos >= size_)
            pos = size_ - 1;
        if (count == 0)
            return pos;
        if (pos < count - 1)
            return npos;
        for (auto i = pos; i >= count - 1; --i)
        {
            if (*(buffer_ + i) == str[count - 1])
            {
                size_type j = 1;
                for (; j < count; ++j)
                {
                    if (*(buffer_ + i - j) != str[count - j - 1])
                        break;
                }
                if (j == count)
                    return i - count + 1;
            }
        }
        return npos;
    }
    /*************************************************************************/
    /*以下为辅助函数*/
    /*************************************************************************/
    // 尝试初始化一段buffer，若分配失败则忽略，不会抛出异常
    template <class CharType, class CharTraits>
    void basic_string<CharType, CharTraits>::
        try_init() noexcept
    {
        try
        {
            buffer_ = data_allocator::allocate(static_cast<size_type>(STRING_INIT_SIZE));
            size_ = 0;
            capacity_ = 0;
        }
        catch (...)
        {
            buffer_ = nullptr;
            size_ = 0;
            capacity_ = 0;
        }
    }
    // fill_init：用字符ch来初始化
    template <class CharType, class CharTraits>
    void basic_string<CharType, CharTraits>::
        fill_init(size_type n, value_type ch)
    {

        const auto init_size = mystl::max(static_cast<size_type>(STRING_INIT_SIZE), n + 1);
        buffer_ = data_allocator::allocate(init_size);
        char_traits::fill(buffer_, ch, n);
        size_ = n;
        capacity_ = init_size;
    }
    // copy_inti：用迭代器来初始化
    template <class CharType, class CharTraits>
    template <class Iter>
    void basic_string<CharType, CharTraits>::
        copy_init(Iter first, Iter last, mystl::input_iterator_tag)
    {
        size_type n = mystl::distance(first, last);
        const auto init_size = mystl::max(static_cast<size_type>(STRING_INIT_SIZE), n + 1);
        try
        {
            buffer_ = data_allocator::allocate(init_size);
            size_ = n;
            capacity_ = init_size;
        }
        catch (...)
        {
            buffer_ = nullptr;
            size_ = 0;
            capacity_ = 0;
            throw;
        }
        for (; n > 0; --n, ++first)
            append(*first);
    }
    // init_from：从C风格字符串初始化，从src的pos处开始的count个字符
    template <class CharType, class CharTraits>
    void basic_string<CharType, CharTraits>::
        init_from(const_pointer src, size_type pos, size_type count)
    {
        const auto init_size = mystl::max(static_cast<size_type>(STRING_INIT_SIZE), count + 1);
        buffer_ = data_allocator::allocate(init_size);
        char_traits::copy(buffer_, src + pos, count);
        size_ = count;
        capacity_ = init_size;
    }
    // destroy_buffer函数
    template <class CharType, class CharTraits>
    void basic_string<CharType, CharTraits>::
        destroy_buffer()
    {
        if (buffer_ != nullptr)
        {
            data_allocator::deallocate(buffer_, capacity_);
            buffer_ = nullptr;
            size_ = 0;
            capacity_ = 0;
        }
    }
    // to_raw_pointer函数：返回一个c风格字符串
    template <class CharType, class CharTraits>
    typename basic_string<CharType, CharTraits>::const_pointer
    basic_string<CharType, CharTraits>::
        to_raw_pointer() const
    {
        *(buffer_ + size_) = value_type();
        return buffer_;
    }
    // 比较C风格字符串
    template <class CharType, class CharTraits>
    int basic_string<CharType, CharTraits>::
        compare_cstr(const_pointer s1, size_type n1, const_pointer s2, size_type n2) const
    {
        auto len = mystl::min(n1, n2);
        auto res = char_traits::compare(s1, s2, len);
        if (res != 0)
            return res;
        if (n1 < n2)
            return -1;
        if (n1 > n2)
            return 1;
        return 0;
    }
    /*******************************************************************************/
    // reallocate函数：重新添加内存，长度为need
    template <class CharType, class CharTraits>
    void basic_string<CharType, CharTraits>::
        reallocate(size_type need)
    {
        const auto new_capacity_ = mystl::max(capacity_ + need, capacity_ + (capacity_ >> 1));
        auto new_buffer = data_allocator::allocate(new_capacity_);
        char_traits::move(new_buffer, buffer_, size_);
        data_allocator::deallocate(buffer_);
        buffer_ = new_buffer;
        capacity_ = new_capacity_;
    }
    // reallocate_and_fill函数：在pos位置插入n个ch字符
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
        data_allocator::deallocate(buffer_, old_cap);
        buffer_ = new_buffer;
        size_ += n;
        capacity_ = new_cap;
        return buffer_ + r;
    }

    // reallocate_and_copy函数：把[first,last)复制添加到pos处
    template <class CharType, class CharTraits>
    typename basic_string<CharType, CharTraits>::iterator
    basic_string<CharType, CharTraits>::
        reallocate_and_copy(iterator pos, const_iterator first, const_iterator last)
    {
        const auto r = pos - buffer_;
        const auto old_cap = capacity_;
        const size_type n = mystl::distance(first, last);
        const auto new_cap = mystl::max(old_cap + n, old_cap + (old_cap >> 1));
        auto new_buffer = data_allocator::allocate(new_cap);
        auto e1 = char_traits::move(new_buffer, buffer_, r) + r;
        auto e2 = mystl::uninitialized_copy_n(first, n, e1) + n;
        char_traits::move(e2, buffer_ + r, size_ - r);
        data_allocator::deallocate(buffer_, old_cap);
        buffer_ = new_buffer;
        size_ += n;
        capacity_ = new_cap;
        return buffer_ + r;
    }

    /**********************************************************************************/
    /*重载全局操作符*/
    // 重载operator+
    // string+string
    template <class CharType, class CharTraits>
    basic_string<CharType, CharTraits>
    operator+(const basic_string<CharType, CharTraits> &lhs,
              const basic_string<CharType, CharTraits> &rhs)
    {
        basic_string<CharType, CharTraits> tmp(lhs);
        tmp.append(rhs);
        return tmp;
    }
    // c_string+string
    template <class CharType, class CharTraits>
    basic_string<CharType, CharTraits>
    operator+(const CharType *lhs, const basic_string<CharType, CharTraits> &rhs)
    {
        basic_string<CharType, CharTraits> tmp(lhs);
        tmp.append(rhs);
        return tmp;
    }
    // char+string
    template <class CharType, class CharTraits>
    basic_string<CharType, CharTraits>
    operator+(CharType ch, const basic_string<CharType, CharTraits> &rhs)
    {
        basic_string<CharType, CharTraits> tmp(1, ch);
        tmp.append(rhs);
        return tmp;
    }
    // string+c_string;
    template <class CharType, class CharTraits>
    basic_string<CharType, CharTraits>
    operator+(const basic_string<CharType, CharTraits> &lhs, const CharType *rhs)
    {
        basic_string<CharType, CharTraits> tmp(lhs);
        tmp.append(rhs);
        return tmp;
    }
    // string+char
    template <class CharType, class CharTraits>
    basic_string<CharType, CharTraits>
    operator+(const basic_string<CharType, CharTraits> &lhs, CharType ch)
    {
        basic_string<CharType, CharTraits> tmp(lhs);
        tmp.append(1, ch);
        return tmp;
    }
    // string（右值）+string
    template <class CharType, class CharTraits>
    basic_string<CharType, CharTraits>
    operator+(basic_string<CharType, CharTraits> &&lhs, const basic_string<CharType, CharTraits> &rhs)
    {
        basic_string<CharType, CharTraits> tmp(mystl::move(lhs));
        tmp.append(rhs);
        return tmp;
    }
    // string + string(右值)
    template <class CharType, class CharTraits>
    basic_string<CharType, CharTraits>
    operator+(const basic_string<CharType, CharTraits> &lhs, basic_string<CharType, CharTraits> &&rhs)
    {
        basic_string<CharType, CharTraits> tmp(mystl::move(rhs));
        tmp.insert(tmp.begin(), lhs.begin(), lhs.end());
        return tmp;
    }
    // string(右值)+string(右值)
    template <class CharType, class CharTraits>
    basic_string<CharType, CharTraits>
    operator+(basic_string<CharType, CharTraits> &&lhs,
              basic_string<CharType, CharTraits> &&rhs)
    {
        basic_string<CharType, CharTraits> tmp(mystl::move(lhs));
        tmp.append(rhs);
        return tmp;
    }
    // c_string+string(右值)
    template <class CharType, class CharTraits>
    basic_string<CharType, CharTraits>
    operator+(const CharType *lhs, basic_string<CharType, CharTraits> &&rhs)
    {
        basic_string<CharType, CharTraits> tmp(lhs);
        tmp.append(rhs);
        return tmp;
    }
    // char+string（右值）
    template <class CharType, class CharTraits>
    basic_string<CharType, CharTraits>
    operator+(CharType ch, basic_string<CharType, CharTraits> &&rhs)
    {
        basic_string<CharType, CharTraits> tmp(ch);
        tmp.append(rhs);
        return tmp;
    }
    // string（右值）+ c_string
    template <class CharType, class CharTraits>
    basic_string<CharType, CharTraits>
    operator+(basic_string<CharType, CharTraits> &&lhs, const CharType *rhs)
    {
        basic_string<CharType, CharTraits> tmp(mystl::move(lhs));
        tmp.append(rhs);
        return tmp;
    }
    // string（右值）+ char
    template <class CharType, class CharTraits>
    basic_string<CharType, CharTraits>
    operator+(basic_string<CharType, CharTraits> &&lhs, CharType ch)
    {
        basic_string<CharType, CharTraits> tmp(mystl::move(lhs));
        tmp.append(1, ch);
        return tmp;
    }
    /*****************************************************************************/
    // 重载operator==
    // string==string
    template <class CharType, class CharTraits>
    bool operator==(const basic_string<CharType, CharTraits> &lhs,
                    const basic_string<CharType, CharTraits> &rhs)
    {
        return lhs.size() == rhs.size() && lhs.compare(rhs) == 0;
    }
    // string!=string
    template <class CharType, class CharTraits>
    bool operator!=(const basic_string<CharType, CharTraits> &lhs,
                    const basic_string<CharType, CharTraits> &rhs)
    {
        return lhs.size() != rhs.size() || lhs.compare(rhs) != 0;
    }
    /*****************************************************************************/
    // 重载operator<、>、<=、>=
    // string < string
    template <class CharType, class CharTraits>
    bool operator<(const basic_string<CharType, CharTraits> &lhs,
                   const basic_string<CharType, CharTraits> &rhs)
    {
        return lhs.compare(rhs) < 0;
    }
    // string <= string
    template <class CharType, class CharTraits>
    bool operator<=(const basic_string<CharType, CharTraits> &lhs,
                    const basic_string<CharType, CharTraits> &rhs)
    {
        return lhs.compare(rhs) <= 0;
    }
    // string > string
    template <class CharType, class CharTraits>
    bool operator>(const basic_string<CharType, CharTraits> &lhs,
                   const basic_string<CharType, CharTraits> &rhs)
    {
        return lhs.compare(rhs) > 0;
    }
    // string >= string
    template <class CharType, class CharTraits>
    bool operator>=(const basic_string<CharType, CharTraits> &lhs,
                    const basic_string<CharType, CharTraits> &rhs)
    {
        return lhs.compare(rhs) >= 0;
    }
    /****************************************************************************/
    // 重载mystl的swap
    template <class CharType, class CharTraits>
    void swap(basic_string<CharType, CharTraits> &lhs,
              basic_string<CharType, CharTraits> &rhs) noexcept
    {
        lhs.swap(rhs);
    }
    // 特化mystl的hash
    template <class CharType, class CharTraits>
    struct hash<basic_string<CharType, CharTraits>>
    {
        size_t operator()(const basic_string<CharType, CharTraits> &str)
        {
            return bitwise_hash((const unsigned char *)str.c_str(),
                                str.size() * sizeof(CharType));
        }
    };
}
#endif