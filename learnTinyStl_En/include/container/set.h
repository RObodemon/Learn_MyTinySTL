#ifndef _SET_H_
#define _SET_H_

// set      : the key value is a real value, 
//            the elements in the collection will be automatically sorted, 
//            and the key value does not allow duplication
// multiset : The key value is a real value, 
//            the elements in the collection will be automatically sorted, 
//            and the key value is allowed to be repeated

// notes:
// Exception guarantees:
// tinystl::set<Key> / tinystl::multiset<Key> 
// Satisfy the basic exception guarantee, 
// and strengthen the exception safety guarantee for the following functions:
//   * emplace
//   * emplace_hint
//   * insert

#include "rb_tree.h"

namespace tinystl
{
    //======================================================================================
    // set
    // first parameter: key-value type
    // second parameter: key-value comparison, default: tinystl::less 
    template <class Key, class Compare = tinystl::less<Key>>
    class set
    {
    public:
        typedef Key        key_type;
        typedef Key        value_type;
        typedef Compare    key_compare;
        typedef Compare    value_compare;

    private:
        // tinystl::rb_tree
        typedef tinystl::rb_tree<value_type, key_compare>  base_type;
        base_type tree_;

    public:
        typedef typename base_type::node_type              node_type;

        typedef typename base_type::const_pointer          pointer;
        typedef typename base_type::const_pointer          const_pointer;

        typedef typename base_type::const_reference        reference;
        typedef typename base_type::const_reference        const_reference;

        typedef typename base_type::const_iterator         iterator;
        typedef typename base_type::const_iterator         const_iterator;

        typedef typename base_type::const_reverse_iterator reverse_iterator;
        typedef typename base_type::const_reverse_iterator const_reverse_iterator;

        typedef typename base_type::size_type              size_type;
        typedef typename base_type::difference_type        difference_type;
        typedef typename base_type::allocator_type         allocator_type;

    public:
        // constructor
        set() = default;

        template <class InputIterator>
        set(InputIterator first, InputIterator last): tree_() 
        { tree_.insert_unique(first, last); }

        set(std::initializer_list<value_type> ilist): tree_()
        { tree_.insert_unique(ilist.begin(), ilist.end()); }

        // copy constructor
        set(const set& rhs): tree_(rhs.tree_)
        {}
        
        // move constructor
        set(set&& rhs) noexcept: tree_(tinystl::move(rhs.tree_))
        {}

        // copy assignment
        set& operator=(const set& rhs)
        {
            tree_ = rhs.tree_;
            return *this;
        }
        // move assignment
        set& operator=(set&& rhs)
        { 
            tree_ = tinystl::move(rhs.tree_); 
            return *this; 
        }
        set& operator=(std::initializer_list<value_type> ilist)
        {
            tree_.clear();
            tree_.insert_unique(ilist.begin(), ilist.end());
            return *this;
        }

        // interface
        key_compare key_comp() const 
        { return tree_.key_comp(); }

        value_compare value_comp() const 
        { return tree_.key_comp(); }

        allocator_type get_allocator() const 
        { return tree_.get_allocator(); }

        // iterator related operations
        iterator begin() noexcept
        { return tree_.begin(); }

        const_iterator begin() const noexcept
        { return tree_.begin(); }

        iterator end() noexcept
        { return tree_.end(); }

        const_iterator end() const noexcept
        { return tree_.end(); }

        reverse_iterator rbegin() noexcept
        { return reverse_iterator(end()); }

        const_reverse_iterator rbegin()  const noexcept
        { return const_reverse_iterator(end()); }

        reverse_iterator rend() noexcept
        { return reverse_iterator(begin()); }

        const_reverse_iterator rend() const noexcept
        { return const_reverse_iterator(begin()); }

        const_iterator cbegin() const noexcept
        { return begin(); }

        const_iterator cend() const noexcept
        { return end(); }

        const_reverse_iterator crbegin() const noexcept
        { return rbegin(); }

        const_reverse_iterator crend() const noexcept
        { return rend(); }

        // container related operations
        bool empty() const noexcept 
        { return tree_.empty(); }

        size_type size() const noexcept 
        { return tree_.size(); }

        size_type max_size() const noexcept 
        { return tree_.max_size(); }

        template <class ...Args>
        pair<iterator, bool> emplace(Args&& ...args)
        {
            return tree_.emplace_unique(tinystl::forward<Args>(args)...);
        }

        template <class ...Args>
        iterator emplace_hint(iterator hint, Args&& ...args)
        {
            return tree_.emplace_unique_use_hint(hint, tinystl::forward<Args>(args)...);
        }

        pair<iterator, bool> insert(const value_type& value)
        {
            return tree_.insert_unique(value);
        }
        pair<iterator, bool> insert(value_type&& value)
        {
            return tree_.insert_unique(tinystl::move(value));
        }

        iterator insert(iterator hint, const value_type& value)
        {
            return tree_.insert_unique(hint, value);
        }
        iterator insert(iterator hint, value_type&& value)
        {
            return tree_.insert_unique(hint, tinystl::move(value));
        }

        template <class InputIterator>
        void insert(InputIterator first, InputIterator last)
        {
            tree_.insert_unique(first, last);
        }

        void erase(iterator position)             
        { tree_.erase(position); }

        size_type erase(const key_type& key)           
        { return tree_.erase_unique(key); }

        void erase(iterator first, iterator last) 
        { tree_.erase(first, last); }

        void clear() 
        { tree_.clear(); }

        iterator find(const key_type& key)              
        { return tree_.find(key); }

        const_iterator find(const key_type& key) const 
        { return tree_.find(key); }

        size_type count(const key_type& key) const 
        { return tree_.count_unique(key); }

        iterator lower_bound(const key_type& key)       
        { return tree_.lower_bound(key); }

        const_iterator lower_bound(const key_type& key) const 
        { return tree_.lower_bound(key); }

        iterator upper_bound(const key_type& key)       
        { return tree_.upper_bound(key); }

        const_iterator upper_bound(const key_type& key) const 
        { return tree_.upper_bound(key); }

        pair<iterator, iterator> equal_range(const key_type& key)
        { return tree_.equal_range_unique(key); }

        pair<const_iterator, const_iterator> equal_range(const key_type& key) const
        { return tree_.equal_range_unique(key); }

        void swap(set& rhs) noexcept
        { tree_.swap(rhs.tree_); }

    public:
        friend bool operator==(const set& lhs, const set& rhs) 
        { return lhs.tree_ == rhs.tree_; }

        friend bool operator< (const set& lhs, const set& rhs) 
        { return lhs.tree_ <  rhs.tree_; }
    };

    // overload comparison operations
    template <class Key, class Compare>
    bool operator==(const set<Key, Compare>& lhs, const set<Key, Compare>& rhs)
    {
        return lhs == rhs;
    }

    template <class Key, class Compare>
    bool operator<(const set<Key, Compare>& lhs, const set<Key, Compare>& rhs)
    {
        return lhs < rhs;
    }

    template <class Key, class Compare>
    bool operator!=(const set<Key, Compare>& lhs, const set<Key, Compare>& rhs)
    {
        return !(lhs == rhs);
    }

    template <class Key, class Compare>
    bool operator>(const set<Key, Compare>& lhs, const set<Key, Compare>& rhs)
    {
        return rhs < lhs;
    }

    template <class Key, class Compare>
    bool operator<=(const set<Key, Compare>& lhs, const set<Key, Compare>& rhs)
    {
        return !(rhs < lhs);
    }

    template <class Key, class Compare>
    bool operator>=(const set<Key, Compare>& lhs, const set<Key, Compare>& rhs)
    {
        return !(lhs < rhs);
    }

    // swap
    template <class Key, class Compare>
    void swap(set<Key, Compare>& lhs, set<Key, Compare>& rhs) noexcept
    {
    lhs.swap(rhs);
    }

    //======================================================================================
    // multiset
    // first parameter: key-value type
    // second parameter: key-value comparison, default: tinystl::less
    template <class Key, class Compare = tinystl::less<Key>>
    class multiset
    {
    public:
        typedef Key        key_type;
        typedef Key        value_type;
        typedef Compare    key_compare;
        typedef Compare    value_compare;

    private:
        // rb_tree
        typedef tinystl::rb_tree<value_type, key_compare>  base_type;
        base_type tree_; 

    public:
        typedef typename base_type::node_type              node_type;

        typedef typename base_type::const_pointer          pointer;
        typedef typename base_type::const_pointer          const_pointer;
        
        typedef typename base_type::const_reference        reference;
        typedef typename base_type::const_reference        const_reference;

        typedef typename base_type::const_iterator         iterator;
        typedef typename base_type::const_iterator         const_iterator;

        typedef typename base_type::const_reverse_iterator reverse_iterator;
        typedef typename base_type::const_reverse_iterator const_reverse_iterator;

        typedef typename base_type::size_type              size_type;
        typedef typename base_type::difference_type        difference_type;
        typedef typename base_type::allocator_type         allocator_type;

    public:
        // constructor
        multiset() = default;

        template <class InputIterator>
        multiset(InputIterator first, InputIterator last): tree_() 
        { tree_.insert_multi(first, last); }

        multiset(std::initializer_list<value_type> ilist): tree_() 
        { tree_.insert_multi(ilist.begin(), ilist.end()); }

        // copy constructor
        multiset(const multiset& rhs): tree_(rhs.tree_)
        {}
        // move constructor
        multiset(multiset&& rhs) noexcept: tree_(tinystl::move(rhs.tree_))
        {}
        // copy assignment
        multiset& operator=(const multiset& rhs) 
        { 
            tree_ = rhs.tree_;
            return *this; 
        }
        // move assignment
        multiset& operator=(multiset&& rhs)
        {
            tree_ = tinystl::move(rhs.tree_);
            return *this; 
        }
        // assignment
        multiset& operator=(std::initializer_list<value_type> ilist)
        {
            tree_.clear();
            tree_.insert_multi(ilist.begin(), ilist.end());
            return *this;
        }

        // interface
        key_compare key_comp() const 
        { return tree_.key_comp(); }

        value_compare value_comp() const 
        { return tree_.key_comp(); }

        allocator_type get_allocator() const 
        { return tree_.get_allocator(); }

        iterator begin() noexcept
        { return tree_.begin(); }

        const_iterator begin() const noexcept
        { return tree_.begin(); }

        iterator end() noexcept
        { return tree_.end(); }

        const_iterator end() const noexcept
        { return tree_.end(); }

        reverse_iterator rbegin() noexcept
        { return reverse_iterator(end()); }

        const_reverse_iterator rbegin() const noexcept
        { return const_reverse_iterator(end()); }

        reverse_iterator rend() noexcept
        { return reverse_iterator(begin()); }

        const_reverse_iterator rend() const noexcept
        { return const_reverse_iterator(begin()); }

        const_iterator cbegin() const noexcept
        { return begin(); }

        const_iterator cend() const noexcept
        { return end(); }

        const_reverse_iterator crbegin() const noexcept
        { return rbegin(); }

        const_reverse_iterator crend() const noexcept
        { return rend(); }

        // container related operations
        bool empty() const noexcept 
        { return tree_.empty(); }

        size_type size() const noexcept 
        { return tree_.size(); }

        size_type max_size() const noexcept 
        { return tree_.max_size(); }

        template <class ...Args>
        iterator emplace(Args&& ...args)
        {
            return tree_.emplace_multi(tinystl::forward<Args>(args)...);
        }

        template <class ...Args>
        iterator emplace_hint(iterator hint, Args&& ...args)
        {
            return tree_.emplace_multi_use_hint(hint, tinystl::forward<Args>(args)...);
        }

        iterator insert(const value_type& value)
        {
            return tree_.insert_multi(value);
        }

        iterator insert(value_type&& value)
        {
            return tree_.insert_multi(tinystl::move(value));
        }

        iterator insert(iterator hint, const value_type& value)
        {
            return tree_.insert_multi(hint, value);
        }
        iterator insert(iterator hint, value_type&& value)
        {
            return tree_.insert_multi(hint, tinystl::move(value));
        }

        template <class InputIterator>
        void insert(InputIterator first, InputIterator last)
        {
            tree_.insert_multi(first, last);
        }

        void erase(iterator position)             
        { tree_.erase(position); }

        size_type erase(const key_type& key) 
        { return tree_.erase_multi(key); }

        void erase(iterator first, iterator last) 
        { tree_.erase(first, last); }

        void clear() 
        { tree_.clear(); }

        iterator find(const key_type& key)              
        { return tree_.find(key); }

        const_iterator find(const key_type& key) const 
        { return tree_.find(key); }

        size_type count(const key_type& key) const 
        { return tree_.count_multi(key); }

        iterator lower_bound(const key_type& key)       
        { return tree_.lower_bound(key); }

        const_iterator lower_bound(const key_type& key) const 
        { return tree_.lower_bound(key); }

        iterator upper_bound(const key_type& key)       
        { return tree_.upper_bound(key); }

        const_iterator upper_bound(const key_type& key) const 
        { return tree_.upper_bound(key); }

        pair<iterator, iterator> equal_range(const key_type& key)
        { return tree_.equal_range_multi(key); }

        pair<const_iterator, const_iterator> equal_range(const key_type& key) const
        { return tree_.equal_range_multi(key); }

        void swap(multiset& rhs) noexcept
        { tree_.swap(rhs.tree_); }

    public:
        friend bool operator==(const multiset& lhs, const multiset& rhs) 
        { return lhs.tree_ == rhs.tree_; }

        friend bool operator< (const multiset& lhs, const multiset& rhs) 
        { return lhs.tree_ <  rhs.tree_; }
    };

    // overload comparison operations
    template <class Key, class Compare>
    bool operator==(const multiset<Key, Compare>& lhs, const multiset<Key, Compare>& rhs)
    {
        return lhs == rhs;
    }

    template <class Key, class Compare>
    bool operator<(const multiset<Key, Compare>& lhs, const multiset<Key, Compare>& rhs)
    {
        return lhs < rhs;
    }

    template <class Key, class Compare>
    bool operator!=(const multiset<Key, Compare>& lhs, const multiset<Key, Compare>& rhs)
    {
        return !(lhs == rhs);
    }

    template <class Key, class Compare>
    bool operator>(const multiset<Key, Compare>& lhs, const multiset<Key, Compare>& rhs)
    {
        return rhs < lhs;
    }

    template <class Key, class Compare>
    bool operator<=(const multiset<Key, Compare>& lhs, const multiset<Key, Compare>& rhs)
    {
        return !(rhs < lhs);
    }

    template <class Key, class Compare>
    bool operator>=(const multiset<Key, Compare>& lhs, const multiset<Key, Compare>& rhs)
    {
        return !(lhs < rhs);
    }

    // swap
    template <class Key, class Compare>
    void swap(multiset<Key, Compare>& lhs, multiset<Key, Compare>& rhs) noexcept
    {
        lhs.swap(rhs);
    }

} // namespace tinystl
#endif