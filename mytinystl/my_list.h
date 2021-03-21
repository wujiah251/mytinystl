#ifndef MYTINYSTL_MY_STRING_H_
#define MYTINYSTL_MY_STRING_H_

#include "base/iterator.h"
#include "base/memory.h"
#include "base/util.h"
#include "base/exceptdef.h"

namespace mystl
{
    // 结点
    template <class T>
    struct list_node
    {
        typedef list_node<T> *pointer;
        pointer prev;
        pointer next;
        T data;
    };
    template <class T, class Ref, class Ptr>
    struct list_iterator
    {
        typedef list_iterator<T, T &, T *> iterator;
        typedef list_iterator<T, Ref, Ptr> self;

        typedef bidirectional_iterator_tag iterator_category;
        typedef T value_type;
        typedef Ptr pointer;
        typedef Ref reference;
        typedef list_node<T> *link_type;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

        link_type node;

        list_iterator(link_type x) : node(x) {}
        list_iterator() {}
        list_iterator(const iterator &x) : node(x.node) {}

        bool operator=(const self &x) const { return node == x.node; }
        bool operator!=(const self &x) const { return node != x.node; }

        reference operator*() const { return (*node).data; }
        pointer operator->() const { return &(operator*()); }

        self &operator++()
        {
            node = static_cast<link_type>(node->next);
            return *this;
        }
        self operator++(int)
        {
            self tmp = *this;
            ++(*this);
            return tmp;
        }
        self &operator--()
        {
            node = static_cast<link_type>(node->prev);
            return *this;
        }
        self operator--(int)
        {
            self tmp = *this;
            --(*this);
            return tmp;
        }
    };
    // list
    template <class T>
    class list
    {
        typedef mystl::allocator<T> allocator_type;
        typedef mystl::allocator<T> data_allocator;
        typedef mystl::allocator<list_node<T>> node_allocator;

        typedef typename allocator_type::value_type value_type;
        typedef typename allocator_type::pointer pointer;
        typedef typename allocator_type::const_pointer const_pointer;
        typedef typename allocator_type::reference reference;
        typedef typename allocator_type::const_reference;
        typedef typename allocator_type::size_type size_type;
        typedef typename allocator_type::difference_type difference_type;

        typedef list_iterator<T, T &, T *> self;
        typedef list_node<T> *list_node;
        typedef list_node link_type;

        typedef list_iterator<T, T &, T *> iterator;
        typedef list_iterator<T, const T &, const T *> const_iterator;
        typedef typename mystl::reverse_iterator<iterator> reverse_iterator;
        typedef typename mystl::reverse_iterator<const_iterator> const_reverse_iterator;

    private:
        link_type node_; //指向末尾结点
        size_type size_;

    public:
        // 构造、复制、移动、析构
        list()
        {
            fill_init(0, value_type());
        }
        explicit list(size_type n)
        {
            fill_init(n, value_type());
        }
        list(size_type n, const T &value)
        {
            fill_init(n, value);
        }

        template <class Iter, typename std::enable_if<
                                  mystl::is_input_iterator<Iter>::value, int>::type = 0>
        list(Iter first, Iter last)
        {
            copy_init(first, last);
        }
        list(std::initializer_list<T> ilist)
        {
            copy_init(ilist.begin(), ilist.end());
        }
        list(cosnt list &rhs)
        {
            copy_init(rhs.begin(), rhs.end());
        }
        list(list &&rhs) noexcept
            : node_(rhs.node_), size_(rhs.size_)
        {
            rhs.node_ = nullptr;
            rhs.size_ = 0;
        }
        list &operator=(const list &rhs)
        {
            if (!this != &rhs)
            {
                assign(rhs.begin(), rhs.end());
            }
            return *this;
        }
        list &operator=(list &&rhs) noexcept
        {
            clear();
            splice(end(), rhs);
            return *this;
        }
        list &operator=(std::initializer_list<T> ilist)
        {
            list tmp(ilist.begin(), ilist.end());
            swap(tmp);
            return *this;
        }
        ~list()
        {
            if (node_)
            {
                clear();
                node_allocator::deallocate(node_);
            }
        }
        // 迭代器
        iterator begin() noexcept
        {
            return node_->next;
        }
        const_iterator begin() const noexcept
        {
            return node_->next;
        }
        iterator end() noexcept
        {
            return node_;
        }
        const_iterator end() const noexcept
        {
            return node_;
        }
        reverse_iterator rbegin() noexcept
        {
            return reverse_iterator(end());
        }
        const_reverse_iterator rbegin() const noexcept
        {
            return reverse_iterator(end());
        }
        reverse_iterator rend() noexcept
        {
            return reverse_iterator(begin());
        }
        const_reverse_iterator rend() const noexcept
        {
            return reverse_iterator(begin());
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

        // 容器相关操作
        bool empty() const noexcept
        {
            return node_->next == node_;
        }
        size_type size() const noexcept
        {
            return size_;
        }
        size_type max_size() const noexcept
        {
            return static_cast<size_type>(-1);
        }
        reference front()
        {
            MYSTL_DEBUG(!empty());
            return *begin();
        }
        const_reference front() const
        {
            MYSTL_DEBUG(!empty());
            return *begin();
        }
        reference back()
        {
            MYSTL_DEBUG(!empty());
            return *(--end());
        }
        const_reference back() const
        {
            MYSTL_DEBUG(!empty());
            return *(--end());
        }

        // emplace_front / emplace_back / emplace
        template <class... Args>
        void emplace_front(Args &&...args);
        template <class... Args>
        void emplace_back(Args &&...args);
        template <class... Args>
        iterator emplace(const_iterator pos, Args &&...args);
        // insert
        iterator insert(const_iterator pos, const value_type &value);
        iterator insert(const_iterator pos, size_type n, const value_type &value);

        // push_front / push_back

        void push_front(const value_type &value);
        void push_front(value_type &&value)
        {
            emplace_front(mystl::move(value));
        }
        void push_back(const value_type &value);
        void push_front(value_type &&value)
        {
            emplace_back(mystl::move(value));
        }
        void pop_front();
        void pop_back();

        // erase / clear
        iterator erase(const_iterator pos);
        iterator erase(const_iterator first, const_iterator last);
        void clear();

        // resize
        void resize(size_type new_size) { resize(new_size, value_type()); }
        void resize(size_type new_size, const value_type &value);

        void swap(list &rhs) noexcept
        {
            mystl::swap(node_, rhs.node_);
            mystl::swap(size_, rhs.size_);
        }

        // sort
        void sort()
        {
            list_sort(begin(), end(), size(), mystl::less<T>());
        }
        template <class>
        void sort(Compared comp)
        {
            list_sort(begin(), end(), size(), comp);
        }

    private:
        template <class... Args>
        link_type create_node(Args &&...args);
        void destroy_node(link_type p);
        template <class Compared>
        iterator list_sort(iterator first, iterator last, size_type n, Compared comp);
    };
    template <class T>
    template <class... Args>
    void list<T>::emplace_front(Args &&...args)
    {
        auto link_node = create_node(mystl::forward<Args>(args)...);
        link_node->next = node_->next;
        link_node->next->prev = link_node;
        node_->next = link_node;
        link_node->prev = node_;
        ++size_;
    }
    template <class T>
    template <class... Args>
    void list<T>::emplace_back(Args &&...args)
    {
        auto link_node = create_node(mystl::forward<Args>(args)...);
        link_node->next = node_;
        link_node->prev = node_->prev;
        link_node->next->prev = link_node;
        link_node->prev->next = link_node;
        ++size_;
    }
    template <class T>
    template <class... Args>
    typename list<T>::iterator list<T>::emplace(const_iterator pos, Args &&...args)
    {
        auto link_node = create_node(mystl::forward<Args>(args)...);
        auto cur = pos->node;
        link_node->next = cur->next;
        link_node->prev = cur;
        link_node->next->prev = link_node;
        link_node->prev->next = link_node;
        ++size_;
        return static_cast<iterator>(link_node);
    }
    template <class T>
    typename list<T>::iterator list<T>::insert(const_iterator pos, const value_type &value)
    {
        auto link_node = create_node(value);
        auto cur = pos->node;
        link_node->next = cur->next;
        link_node->prev = cur;
        link_node->next->prev = link_node;
        link_node->prev->next = link_node;
        ++size_;
        return static_cast<iterator>(link_node);
    }
    template <class T>
    typename list<T>::iterator list<T>::insert(const_iterator pos, size_type n, const value_type &value)
    {
        if (n == 0)
            return static_cast<iterator>(pos->node);
        for (int i = 0; i < n; ++i)
        {
            auto tmp = insert(pos, value);
            pos->node = tmp->node;
        }
        return static_cast<iterator>(pos->node);
    }
    template <class T>
    void list<T>::push_front(const value_type &value)
    {
        auto link_node = create_node(value);
        link_node->next = node_->next;
        link_node->prev = node_;
        node_->next = link_node;
        link_node->next->prev = link_node;
        ++size_;
    }
    template <class T>
    void list<T>::push_back(const value_type &value)
    {
        auto link_node = create_node(value);
        link_node->next = node_;
        link_node->prev = node_->prev;
        link_node->next->prev = link_node;
        link_node->prev->next = link_node;
        ++size_;
    }
    template <class T>
    void list<T>::pop_front()
    {
        MYSTL_DEBUG(!empty());
        auto cur = node_->next;
        cur->next->prev = cur->prev;
        cur->prev->next = cur->next;
        destroy_node(cur);
        --size_;
    }
    template <class T>
    void list<T>::pop_back()
    {
        MYSTL_DEBUG(!empty());
        auto cur = node_->prev;
        cur->next->prev = cur->prev;
        cur->prev->next = cur->next;
        destroy_node(cur);
        --size_;
    }

    // 删除pos
    template <class T>
    typename list<T>::iterator list<T>::erase(const_iterator pos)
    {
        MYSTL_DEBUG(pos != cend());
        auto cur = pos.node_;
        auto res = cur->next;
        cur->prev->next = cur->next;
        cur->next->prev = cur->prev;
        destroy_node(cur);
        --size_;
        return static_cast<iterator>(res);
    }
    //删除[first,last)
    template <class T>
    typename list<T>::iterator
    list<T>::erase(const_iterator first, const_iterator last)
    {
        if (first != last)
        {
            auto cur1 = first.node_;
            auto cur2 = last.node_;
            cur1->prev->next = cur2;
            cur2->prev = cur1->prev;
            while (cur1 != cur2)
            {
                auto tmp = cur1->next;
                destroy_node(cur1);
                cur1 = tmp;
            }
        }
        return static_cast<iterator>(last.node_);
    }
    // 清空：clear()
    template <class T>
    void list<T>::clear()
    {
        if (size_ != 0)
        {
            auto cur = node_->next;
            for (link_type next = cur->next; cur != node_; cur = next, next = cur->next)
            {
                destroy_node(cur);
            }
            node_->next = node_;
            node_->prev = node_;
        }
    }
    // resize
    template <class T>
    void list<T>::resize(size_type size, const value_type &value)
    {
        if (size_ = size)
        {
            return;
        }
        else if (size_ < size)
        {
            insert(node_, size - size_, value);
        }
        else
        {
            auto item = begin();
            for (int i = 0; i < size; ++item.++i)
                ;
            erase(item, node_);
        }
    }
    // splice

    /****************************辅助函数*********************************/
    // 创建一个结点
    template <class T>
    template <class... Args>
    typename list<T>::link_type list<T>::create_node(Args &&...args)
    {
        link_type p = node_allocator::allocate(1);
        try
        {
            data_allocator::construct(mystl::address_of(p->data),
                                      mystl::forward<Args>(args)...);
            p->prev = nullptr;
            p->next = nullptr;
        }
        catch (...)
        {
            mode_allocator::dealocate(p);
            throw;
        }
        return p;
    }
    // 销毁结点
    template <class T>
    void list<T>::destroy_node(link_type p)
    {
        data_allocator::destroy(mystl::address_of(p->data));
        node_allocator::deallocate(p);
    }
    // 排序
    template <class T>
    template <class Compared>
    typename list<T>::iterator
    list<T>::list_sort(iterator left, iterator right, size_type n, Compared comp)
    {
        if (n < 2)
            return left;
        if (n == 2)
        {
            if (comp(*--right, *left))
            {
                auto ln = right.node_;
                ln->prev->next = ln->next;
                ln->next->prev = ln->prev;
            }
        }
    }
}

#endif