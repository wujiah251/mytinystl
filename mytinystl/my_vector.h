#ifndef MYTINYSTL_VECTOR_H_
#define MYTINYSTL_VECTOR_H_

#include <initializer_list>

#include "iterator.h"
#include "memory.h"
#include "util.h"
#include "exceptdef.h"

namespace mystl
{
#ifdef max
#pragma message("undefing marco max")
#undef max
#endif // max

#ifdef min
#pragma message("undefing marco min")
#undef min
#endif // min

    // 模板类：vector
    // 提供的公有成员主要有：
    /*********************构造函数*********************/
    // 默认构造函数：vector()
    // 参数构造函数：vector(size_type)、vector(size_type,value_type&)
    // vector(Iter first,Iter last)
    // 复制构造函数vector(const vector& rhs)，vector(vector&& rhs)
    // 初始化列表构造：vector(std::initializer_list<value_type>)
    /*********************运算符重载*******************/
    // operator=(const vector&)
    // operator=(vector&&);
    // operator=(std::initializer_list<value_type>)
    /*********************析构函数*********************/
    // 析构函数：~vector()
    /*********************迭代器**********************/
    // begin()、end()：各2种
    // rbegin()、rend()：各2种
    // cbegin()、cend()、crbegin()、crend()
    /*********************容量操作********************/
    // empty()
    // size()
    // max_size()
    // capacity()
    // 预留空间的大小：reserve(size_type)
    // 放弃多余的容量：shrink_to_fit()
    /*********************访问元素操作****************/
    // operator[](size_type)、operator[](size_type)const
    // at(size_type n)、at(size_type n)const
    // front()、front()const
    // back()、back()const
    // 获取C风格的数组：data()、data()const
    /*********************修改容器的操作**************/
    // 填充n个字符：assign(size_type n, const value_type&)
    // 填充[first,last)：assign(Iter first, Iter last)
    // assign：初始化列表进行填充
    // emplace利用了右值拷贝的思想，可以直接利用参数调用构造函数生成临时对象
    // 在pos处添加emplace(const_iterator, Args&& ...args);
    // 在末尾添加emplace(Args&& ..args)
    // push_back(const value_type&)、push_back(value_type)
    // insert(const_iterator,const value_type&);
    // insert(const_iterator,value_type&&);
    // insert(const_iterator,size_type,value_type)
    // insert(const_iterator pos, Iter first, Iter last);
    // erase(const_iterator)、erase(const_iterator first,const_iterator last);
    // clear()
    // resize(size_type)、resize(size_type,const value_type&)
    // reverse()
    // swap(vector&)
    /****************************************************************/
    /****************************************************************/
    // 私有的辅助函数有：
    /*********************初始化：init*****************/
    // try_init()
    // init_space(size_type,size_type)
    // fill_init(size_type,const value_type& value)
    // range_init(Iter first, Iter last)
    // destroy_and_recover(iterator first,iterator last,size_type)
    /*********************assign**********************/
    // fill_assign()、copy_assign(input_iterator_tag)、copy_assign(forward_iterator_tag)
    /*********************reallocate******************/
    // reallocate_emplace(iterator,Args&& ...args)
    // reallocate_insert(iterator,const value_type&)
    /*********************插入：insert****************/
    // fill_insert(iterator pos, size_type n, const value_type&)
    // copy_insert(iterator pos, IIter first, IIter last)
    // reinsert(size_type)

    template <class T>
    class vector
    {
        // 静态断言
        static_assert(!std::is_same<bool, T>::value, "vector<bool> is abandone in mystl");

    public:
        // vector的分配器类别定义
        typedef mystl::allocator<T> allocator_type;
        typedef mystl::allocator<T> data_allocator;

        // 必要接口

        typedef typename allocator_type::value value_type;
        typedef typename allocator_type::pointer pointer;
        typedef typename allocator_type::const_pointer const_pointer;
        typedef typename allocator_type::reference reference;
        typedef typename allocator_type::const_reference const_reference;
        typedef typename allocator_type::size_type;
        typedef typename allocator_type::difference_type difference_type;

        // 定义迭代器
        typedef value_type *iterator;
        typedef const value_type *const_iterator;
        typedef mystl::reverse_iterator<iterator> reverse_iterator;
        typedef mystl::reverse_iterator<const_iterator> const_reverse_iterator;

    private:
        iterator begin_;    // 表示当前使用的空间的头部
        iterator end_;      // 表示当前使用空间的尾部
        iterator capacity_; // 表示当前储存空间的尾部

    public:
        //主要的公有成员函数：
        // 默认构造函数
        vector() noexcept
        {
            try_init();
        }
        // 参数构造函数：vector(size_type)，禁止隐式类型转换
        explicit vector(size_type n)
        {
            fill_init(n, value_type());
        }
        vector(size_type n, value_type &value)
        {
            fill_init(n, value);
        }
        template <class Iter, typename std::enable_if<
                                  mystl::is_input_iterator<Iter>::value, int>::type = 0>
        vector(Iter first, Iter last)
        {
            MYSTL_DEBUG(!(first < first));
            range_init(first, last);
        }
        // 复制构造函数
        vector(const vector &rhs)
        {
            range_init(rhs.begin_, rhs.end_);
        }
        // 右值拷贝
        vector(vector &&rhs) noexcept
            : begin_(rhs.begin_), end_(rhs.end_), capacity_(rhs.capacity_)
        {
            rhs.begin_ = nullptr;
            rhs.end_ = nullptr;
            rhs.capacoty_ = nullptr;
        }
        // 初始化列表构造
        vector(std::initializer_list<value_type> ilist)
        {
            range_list(ilist.begin(), ilist.end());
        }
        /*********************运算符重载*******************/
        vector &operator=(const vector &rhs);
        vector &operator=(vector &&rhs) noexcept;
        vector &operator=(std::initializer_list<value_type> ilist)
        {
            vector tmp(ilist.begin(), ilist.end());
            swap(tmp);
            return *this;
        }
        /*********************析构函数*********************/
        ~vector()
        {
            destroy_and_recover(begin_, end_, capacity_ - begin_);
            begin_ = end_ = capacity_ = nullptr;
        }
        /*********************迭代器**********************/
        iterator begin() noexcept
        {
            return begin_;
        }
        const_iterator begin() const noexcept
        {
            return begin_;
        }
        iterator end() noexcept
        {
            return end_;
        }
        const_iterator end() const noexcept
        {
            return end_;
        }
        // rbegin()、rend()：各2种
        reverse_iterator rbegin() noexcept
        {
            return reverse_iterator(end();)
        }
        const reverse_iterator rbegin() const noexcept
        {
            return const_reverse_iterator(end();)
        }
        reverse_iterator rend() noexcept
        {
            return reverse_iterator(begin();)
        }
        const reverse_iterator rend() const noexcept
        {
            return const_reverse_iterator(begin();)
        }
        // cbegin()、cend()、crbegin()、crend()
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
        const reverse_iterator crend() const noexcept
        {
            return begin();
        }
        /*********************容量操作********************/
        bool empty() const noexcept
        {
            return begin_ == end_;
        }
        size_type size() const noexcept
        {
            return static_cast<size_type>(end_ - begin_);
        }
        size_type max_size() const noexcept
        {
            return static_cast<size_type>(-1) / sizeof(T);
        }
        // 预留空间的大小
        void reserve(size_type n);
        void shrink_to_fit();

        /*********************访问元素操作****************/
        reference operator[](size_type n)
        {
            MYSTL_DEBUG(n < size());
            return *(begin_ + n);
        }
        reference operator[](size_type n) const
        {
            MYSTL_DEBUG(n < size());
            return *(begin_ + n);
        }
        reference at(size_type n)
        {
            THROW_OUT_OF_RANGE_IF(!(n < size()), "vector<T>::at() subscript out of range");
            return (*thist)[n];
        }
        reference at(size_type n) const
        {
            THROW_OUT_OF_RANGE_IF(!(n < size()), "vector<T>::at() subscript out of range");
            return (*this)[n];
        }

        reference first()
        {
            MYSTL_DEBUG(!empty());
            return *begin_;
        }
        const_reference first() const
        {
            MYSTL_DEBUG(!empty());
            return *begin_;
        }
        reference back()
        {
            MYSTL_DEBUG(!empty());
            return *(end_ - 1);
        }
        const_reference back() const
        {
            MYSTL_DEBUG(!empty());
            return *(end_ - 1);
        }

        pointer data() noexcept { return begin_; }
        const_pointer data() const noexcept { return begin_; }

        /*********************修改容器的操作**************/
        void assign(size_type n, const value_type &value)
        {
            fill_assign(n, value);
        }
        template <class Iter, typename std::enable_if<
                                  mystl::is_input_iterator<Iter>::value, int>::type = 0>
        void assign(Iter first, Iter last)
        {
            MYSTL_DEBUG(!(last < first));
            copy_assign(first, last, iterator_category(first));
        }
        void assign(std::initializer_list<value_type> il)
        {
            copy_assign(il.begin(), il.end(), mystl::forward_iterator_tag{});
        }

        // emplace利用了右值拷贝的思想，可以直接利用参数调用构造函数生成临时对象
        template <class... Args>
        iterator emplace(const_iterator pos, Args &&...args);

        template <class... Args>
        void emplace_back(Args &&...args);

        void push_back(const value_type &value);
        void push_back(value_type &&value)
        {
            emplace_back(mystl::move(value));
        }
        void pop_back()
        {
            MYSTL_DEBUG(!empty());
            data_allocator::destroy(end_ - 1);
            --end_;
        }

        iterator insert(const_iterator pos, const value_type &value);
        iterator insert(const_iterator pos, value_type &&value)
        {
            return emplace(pos, mystl::move(value));)
        }
        iterator insert(const_iterator pos, size_type n, value_type &val)
        {
            MYSTL_DEBUG(pos >= begin() && pos <= end());
            return fill_insert(const_cast<iterator>(pos), n, value);
        }
        template <class Iter, typename std::enable_if<
                                  mystl::is_input_iterator<Iter>::value, int>::type = 0>
        void insert(const_iterator pos, Iter first, Iter last)
        {
            MYSTL_DEBUG(pos >= begin && pos <= end() && !(lst < first));
            copy_insert(const_cast<iterator>(pos), first, last);
        }

        // erase/clear
        iterator erase(const_iterator pos);
        iterator erase(const_iterator first, const_iterator last);
        void clear() { erase(begin(), end()); }

        // resize() / reverse
        void resize(size_type new_size) { return resize(new_size, value_type()); }
        void resize(size_type new_size, const value_type &value);
        void reverse() { mystl::reverse(begin(), end()); }

        // swap(vector&)
        void swap(vector &rhs) noexcept;
        /****************************************************************/
    private:
        // 私有的辅助函数有：
        // init / destroy
        void try_init() noexcept;
        void init_space(size_type size, size_type capacity_);
        void fill_init(size_type n, const value_type &value);
        template <class Iter>
        void range_init(Iter first, Iter last);
        void destroy_and_recover(iterator first, iterator last, size_type n);

        size_type get_new_capacity(size_type add_size);
        // assign
        void fill_assign(size_type n, const value_type &value);
        template <class IIter>
        void copy_assign(IIter first, IIter last, input_iterator_tag);
        template <class FIter>
        void copy_assign(FIter first, FIter last, forward_iterator_tag);
        /*********************reallocate******************/
        template <class... Args>
        void reallocate_emplace(iterator pos, Args &&...args);
        void reallocate_insert(iterator pos, const value_type &value);

        // insert

        iterator fill_insert(iterator pos, size_type n, const value_type &value);
        template <class IIter>
        void copy_insert(iterator pos, IIter first, IIter last);

        void reinsert(size_type size);
    };
    /****************************************************************************************/

    // 复制赋值操作符
    template <class T>
    vector<T> &vector<T>::operator=(const vector &rhs)
    {
        if (this != &rhs)
        {
            const auto len = rhs.size();
            if (len > capacity())
            {
                vector tmp(rhs.begin(), rhs.end());
                swap(tmp);
            }
            else if (size() >= len)
            {
                auto i = mystl::copy(rhs.begin(), rhs.end(), begin());
                data_allocator::destroy(i, end_);
                end_ = begin_ + len;
            }
            else
            {
                mystl::copy(rhs.begin(), rhs.end(), begin_);
                mystl::uninitialized_copy(rhs.begin() + size(), rhs.end(), end_);
            }
        }
    }
}

#endif