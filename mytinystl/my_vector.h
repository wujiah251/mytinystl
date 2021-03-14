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
        typedef typename allocator_type::size_type size_type;
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

        //计算需要成长的大小
        size_type get_new_cap(size_type add_size);

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
    // 移动赋值操作符
    template <class T>
    vector<T> &vector<T>::operator=(vector &&rhs) noexcept
    {
        destroy_and_recover(begin_, end_, capacity_ - begin_);
        begin_ = rhs.begin_;
        end_ = rhs.end_;
        capacity_ = rhs.capacity_;
        rhs.begin_ = nullptr;
        rhs.end_ = nullptr;
        rhs.capacity_ = nullptr;
        return *this;
    }
    // 在pos位置就地构造元素，避免额外的复制或移动开销
    template <class T>
    template <class... Args>
    typename vector<T>::iterator
    vector<T>::emplace(const_iterator pos, Args &&...args)
    {
        MYSTL_DEBUG(pos >= begin() && pos <= end());
        iterator xpos = const_cast<iterator>(pos);
        const size_type n = xpos - begin_;
        if (end_ != capacity_ && xpos == end_)
        {
            data_allocator::construct(mystl::address_of(*end_), mystl::forward<Args>(args)...);
            ++end_;
        }
        else if (end_ != capacity_)
        {
            auto new_end = end_;
            data_allocator::construct(mystl::address_of(*end_), (*end_ - 1));
            ++new_end;
            mystl::copy_backward(xpos, end_ - 1, end_); //后退
            *xpos = value_type(mystl::forward<Args>(args)...);
            end_ = new_end;
        }
        else
        {
            reallocate_emplace(xpos, mystl::forward<Args>(args)...);
        }
        return begin_ + n;
    }
    template <class T>
    template <class... Args>
    void vector<T>::emplace_back(Args &&...args)
    {
        if (end_ < capacity_)
        {
            data_allocator::construct(mystl::address_of(*end_), mystl::forward<Args>(args)...);
            ++end_;
        }
        else if (end_ != capacity_)
        {
            reallocate_emplace(end_, mystl::forward<Args>(args)...);
        }
    }
    template <class T>
    void vector<T>::push_back(const value_type &value)
    {
        if (end_ != capacity_)
        {
            data_allocator::construct(mystl::address_of(*end_), value);
        }
        else
        {
            reallocate_insert(end_, value);
        }
    }
    template <class T>
    void pop_back()
    {
        MYSTL_DEBUG(!empty());
        data_allocator::destroy(end_ - 1);
        --end_;
    }
    // 在pos处插入元素
    template <class T>
    typename vector<T>::iterator
    vector<T>::insert(const_iterator pos, const value_type &value)
    {
        MYSTL_DEBUG(pos >= begin() && pos <= end());
        iterator xpos = const_cast<iterator>(pos);
        const size_type n = pos - begin_;
        if (end_ != cap_ && xpos == end_)
        {
            data_allocator::construct(mystl::address_of(*end_), value);
            ++end_;
        }
        else if (end_ != cap_)
        {
            auto new_end = end_;
            data_allocator::construct(mystl::address_of(*end_), *(end_ - 1));
            ++new_end;
            auto value_copy = value; // 避免元素因以下复制操作而被改变
            mystl::copy_backward(xpos, end_ - 1, end_);
            *xpos = mystl::move(value_copy);
            end_ = new_end;
        }
        else
        {
            reallocate_insert(xpos, value);
        }
        return begin_ + n;
    }
    // 删除pos位置上的元素
    template <class T>
    typename vector<T>::iterator
    vector<T>::erase(const_iterator pos)
    {
        MYSTL_DEBUG(pos >= begin() && pos < end());
        iterator xpos = begin_ + (pos - begin());
        mystl::move(xpos + 1, end_, xpos);
        data_allocator::destroy(end_ - 1);
        --end_;
        return xpos;
    }
    // 删除[first,last)上的元素
    template <class T>
    typename vector<T>::iterator
    vector<T>::erase(const_iterator first, const_iterator last)
    {
        MYSTL_DEBUG(first >= begin() && last <= end() && !(last < first));
        const auto n = first - begin();
        iterator r = begin_ + n;
        data_allocator::destroy(mystl::move(last, end_, r), end_);
        return begin_ + n;
    }
    // 重置容器大小
    template <class T>
    void vector<T>::resize(size_type new_size, const value_type &value)
    {
        if (new_size < size())
        {
            erase(begin() + new_size, end());
        }
        else
        {
            insert(end(), new_size - size(), value);
        }
    }
    // 与另一个vector交换
    template <class T>
    void vector<T>::swap(vector<T> &rhs) noexcept
    {
        if (this != &rhs)
        {
            mystl::swap(begin_, rhs.begin_);
            mystl::swap(end_, rhs.end_);
            mystl::swap(capacity_, rhs.capacity_);
        }
    }
    /********************************私有的互助函数***********************************/
    template <class T>
    void vector<T>::try_init() noexcept
    {
        try
        {
            begin_ = data_allocator::allocate(16);
            end_ = begin_;
            capacity_ = begin_ + 16;
        }
        catch (...)
        {
            begin_ = nullptr;
            end_ = nullptr;
            capacity_ = nullptr;
        }
    }
    template <class T>
    void vector<T>::init_space(size_type size, size_type capacity)
    {
        try
        {
            begin_ = data_allocator::allocate(capacity);
            end_ = begin_ + size;
            capacity_ = begin_ + capacity_;
        }
        catch (...)
        {
            begin_ = nullptr;
            end_ = nullptr;
            capacity_ = nullptr;
        }
    }
    template <class T>
    void vector<T>::fill_init(size_type size, const value_type &value)
    {
        const size_type init_size = mystl::max(static_cast<size_type>(16), n);
        init_space(n, init_size);
        mystl::uninitialized_fill_n(begin_, n, value);
    }
    template <class T>
    template <class Iter>
    void vector<T>::range_init(Iter first, Iter last)
    {
        const size_type init_size = mystl::max(static_cast<size_type>(last - first),
                                               static_cast<size_type>(16));
        init_space(static_cast<size_type>(last - first), init_size);
        mystl::uninitialized_copy(first, last, begin_);
    }
    template <class T>
    void vector<T>::destroy_and_recover(iterator first, iterator last, size_type n)
    {
        data_allocator::destroy(first, last);
        data_allocator::deallocate(first, n);
    }
    template <class T>
    typename vector<T>::size_type
    vector<T>::get_new_cap(size_type add_size)
    {
        const auto old_size = capacity();
        THROW_LENGTH_ERROR_IF(old_size > max_size() - add_size,
                              "vector<T>'s size too big");
        // 1.5倍扩容还是2倍扩容各有各的好处
        if (old_size > max_size() - old_size / 2)
        {
            return old_size + add_size > max_size() - 16
                       ? old_size + add_size
                       : old_size + add_size + 16;
        }
        const size_type new_size = (old_size == 0)
                                       ? mystl::max(add_size, static_cast<size_type>(16))
                                       : mystl::max(old_size + old_size / 2, old_size + add_size);
        return new_size;
    }
    /*********************reallocate******************/
    template <class T>
    template <class... Args>
    void vector<T>::reallocate_emplace(iterator pos, Args &&...args)
    {
        const auto new_size = get_new_cap(1);
        auto new_begin = data_allocator::allocate(new_size);
        auto new_end = new_begin;
        try
        {
            new_end = mystl::uninitialized_move(begin_, pos, new_begin);
            data_allocator::construct(mystl::address_of(*new_end), mystl::forward<Args>(args)...);
            ++new_end;
            new_end = mystl::uninitialized_move(pos, end_, new_end);
        }
        catch (...)
        {
            data_allocator::deallocate(new_begin, new_size);
            throw;
        }
        destroy_and_recover(begin_, end_, cap_ - begin_);
        begin_ = new_begin;
        end_ = new_end;
        capacity_ = new_begin + new_size;
    }
    template <class T>
    void vector<T>::reallocate_insert(iterator pos, const value_type &value)
    {
        const auto new_size = get_new_cap(1);
        auto new_begin = data_allocator::allocate(new_size);
        auto new_end = new_begin;
        const value_type &value_copy = value;
        try
        {
            new_end = mystl::uninitialized_move(begin_, pos, new_begin);
            data_allocator::construct(mystl::address_of(*new_end), value_copy);
            ++new_end;
            new_end = mystl::uninitialized_move(pos, end_, new_end);
        }
        catch (...)
        {
            data_allocator::deallocate(new_begin, new_size);
            throw;
        }
        destroy_and_recover(begin_, end_, cap_ - begin_);
        begin_ = new_begin;
        end_ = new_end;
        capacity_ = new_begin + new_size;
    }
    /*********************插入：insert****************/
    template <class T>
    typename vector<T>::iterator
    vector<T>::
        fill_insert(iterator pos, size_type n, const value_type &value)
    {
        if (n == 0)
            return pos;
        const size_type xpos = pos - begin_;
        const value_type value_copy = value; // 避免被覆盖
        if (static_cast<size_type>(cap_ - end_) >= n)
        { // 如果备用空间大于等于增加的空间
            const size_type after_elems = end_ - pos;
            auto old_end = end_;
            if (after_elems > n)
            {
                mystl::uninitialized_copy(end_ - n, end_, end_);
                end_ += n;
                mystl::move_backward(pos, old_end - n, old_end);
                mystl::uninitialized_fill_n(pos, n, value_copy);
            }
            else
            {
                end_ = mystl::uninitialized_fill_n(end_, n - after_elems, value_copy);
                end_ = mystl::uninitialized_move(pos, old_end, end_);
                mystl::uninitialized_fill_n(pos, after_elems, value_copy);
            }
        }
        else
        { // 如果备用空间不足
            const auto new_size = get_new_cap(n);
            auto new_begin = data_allocator::allocate(new_size);
            auto new_end = new_begin;
            try
            {
                new_end = mystl::uninitialized_move(begin_, pos, new_begin);
                new_end = mystl::uninitialized_fill_n(new_end, n, value);
                new_end = mystl::uninitialized_move(pos, end_, new_end);
            }
            catch (...)
            {
                destroy_and_recover(new_begin, new_end, new_size);
                throw;
            }
            data_allocator::deallocate(begin_, cap_ - begin_);
            begin_ = new_begin;
            end_ = new_end;
            capacity_ = begin_ + new_size;
        }
        return begin_ + xpos;
    }

    // copy_insert 函数
    template <class T>
    template <class IIter>
    void vector<T>::
        copy_insert(iterator pos, IIter first, IIter last)
    {
        if (first == last)
            return;
        const auto n = mystl::distance(first, last);
        if ((cap_ - end_) >= n)
        { // 如果备用空间大小足够
            const auto after_elems = end_ - pos;
            auto old_end = end_;
            if (after_elems > n)
            {
                end_ = mystl::uninitialized_copy(end_ - n, end_, end_);
                mystl::move_backward(pos, old_end - n, old_end);
                mystl::uninitialized_copy(first, last, pos);
            }
            else
            {
                auto mid = first;
                mystl::advance(mid, after_elems);
                end_ = mystl::uninitialized_copy(mid, last, end_);
                end_ = mystl::uninitialized_move(pos, old_end, end_);
                mystl::uninitialized_copy(first, mid, pos);
            }
        }
        else
        { // 备用空间不足
            const auto new_size = get_new_cap(n);
            auto new_begin = data_allocator::allocate(new_size);
            auto new_end = new_begin;
            try
            {
                new_end = mystl::uninitialized_move(begin_, pos, new_begin);
                new_end = mystl::uninitialized_copy(first, last, new_end);
                new_end = mystl::uninitialized_move(pos, end_, new_end);
            }
            catch (...)
            {
                destroy_and_recover(new_begin, new_end, new_size);
                throw;
            }
            data_allocator::deallocate(begin_, cap_ - begin_);
            begin_ = new_begin;
            end_ = new_end;
            capacity_ = begin_ + new_size;
        }
    }

    // reinsert 函数
    template <class T>
    void vector<T>::reinsert(size_type size)
    {
        auto new_begin = data_allocator::allocate(size);
        try
        {
            mystl::uninitialized_move(begin_, end_, new_begin);
        }
        catch (...)
        {
            data_allocator::deallocate(new_begin, size);
            throw;
        }
        data_allocator::deallocate(begin_, cap_ - begin_);
        begin_ = new_begin;
        end_ = begin_ + size;
        capacity_ = begin_ + size;
    }
    /*****************************运算符重载*******************************/
    template <class T>
    bool operator==(const vector<T> &lhs, const vector<T> &rhs)
    {
        return lhs.size() == rhs.size() &&
               mystl::equal(lhs.begin(), lhs.end(), rhs.beign());
    }
    template <class T>
    bool operator<(const vector<T> &lhs, const vector<T> &rhs)
    {
        return mystl::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), lhs.end());
    }
    template <class T>
    bool operator!=(const vector<T> &lhs, const vector<T> &rhs)
    {
        return !(lhs == rhs);
    }
    template <class T>
    bool operator>(const vector<T> &lhs, const vector<T> &rhs)
    {
        return rhs < lhs;
    }
    template <class T>
    bool operator<=(const vector<T> &lhs, const vector<T> &rhs)
    {
        return !(lhs < rhs);
    }
    template <class T>
    bool operator>=(const vector<T> &lhs, const vector<T> &rhs)
    {
        return !(lhs < rhs);
    }
    template <class T>
    void swap(vector<T> &lhs, vector<T> &rhs)
    {
        lhs.swap(rhs);
    }
}

#endif