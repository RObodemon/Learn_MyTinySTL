#ifndef MYTINYSTL_UNORDERED_SET_H_
#define MYTINYSTL_UNORDERED_SET_H_

// unordered_set and unordered_multiset

// notes:
// Exception guarantees:
// tinystl::unordered_set<Key> / tinystl::unordered_multiset<Key> 满足基本异常保证，对以下等函数做强异常安全保证：
//   * emplace
//   * emplace_hint
//   * insert

#include "hashtable.h"

namespace tinystl
{

    // unordered_set，Duplicate key values are not allowed
    // first parameter: key
    // second parameter: hash functon, default: tinystl::hash
    // third parameter: comparison method, default: tinyst::equal_to
    template <class Key, class Hash = tinystl::hash<Key>, class KeyEqual = tinystl::equal_to<Key>>
    class unordered_set
    {
    private:
        typedef hashtable<Key, Hash, KeyEqual> base_type;
        base_type ht_;

    public:
        typedef typename base_type::allocator_type       allocator_type;
        typedef typename base_type::key_type             key_type;
        typedef typename base_type::value_type           value_type;
        typedef typename base_type::hasher               hasher;
        typedef typename base_type::key_equal            key_equal;

        typedef typename base_type::size_type            size_type;
        typedef typename base_type::difference_type      difference_type;
        typedef typename base_type::pointer              pointer;
        typedef typename base_type::const_pointer        const_pointer;
        typedef typename base_type::reference            reference;
        typedef typename base_type::const_reference      const_reference;

        typedef typename base_type::const_iterator       iterator;
        typedef typename base_type::const_iterator       const_iterator;
        typedef typename base_type::const_local_iterator local_iterator;
        typedef typename base_type::const_local_iterator const_local_iterator;

        allocator_type get_allocator() const { return ht_.get_allocator(); }

    public:
        // constructor
        unordered_set(): ht_(100, Hash(), KeyEqual())
        {}

        explicit unordered_set(size_type bucket_count,const Hash& hash = Hash(),
                               const KeyEqual& equal = KeyEqual())
            :ht_(bucket_count, hash, equal)
        {}

        template <class InputIterator>
        unordered_set(InputIterator first, InputIterator last,
                      const size_type bucket_count = 100,
                      const Hash& hash = Hash(),
                      const KeyEqual& equal = KeyEqual())
            : ht_(tinystl::max(bucket_count, static_cast<size_type>(tinystl::distance(first, last))), hash, equal)
        {
            for (; first != last; ++first)
            ht_.insert_unique_noresize(*first);
        }

        unordered_set(std::initializer_list<value_type> ilist,
                      const size_type bucket_count = 100,
                      const Hash& hash = Hash(),
                      const KeyEqual& equal = KeyEqual())
            :ht_(tinystl::max(bucket_count, static_cast<size_type>(ilist.size())), hash, equal)
        {
            for (auto first = ilist.begin(), last = ilist.end(); first != last; ++first)
            ht_.insert_unique_noresize(*first);
        }

        unordered_set(const unordered_set& rhs): ht_(rhs.ht_)
        {}

        unordered_set(unordered_set&& rhs) noexcept: ht_(tinystl::move(rhs.ht_))
        {}

        unordered_set& operator=(const unordered_set& rhs)
        {
            ht_ = rhs.ht_;
            return *this;
        }
        unordered_set& operator=(unordered_set&& rhs)
        {
            ht_ = tinystl::move(rhs.ht_);
            return *this;
        }

        unordered_set& operator=(std::initializer_list<value_type> ilist)
        {
            ht_.clear();
            ht_.reserve(ilist.size());
            for (auto first = ilist.begin(), last = ilist.end(); first != last; ++first)
            ht_.insert_unique_noresize(*first);
            return *this;
        }

        ~unordered_set() = default;

        //---------------------------------------------------------------------------------
        iterator begin() noexcept
        { return ht_.begin(); }

        const_iterator begin() const noexcept
        { return ht_.begin(); }

        iterator end() noexcept
        { return ht_.end(); }

        const_iterator end() const noexcept
        { return ht_.end(); }

        const_iterator cbegin() const noexcept
        { return ht_.cbegin(); }

        const_iterator cend() const noexcept
        { return ht_.cend(); }

        //---------------------------------------------------------------------------------
        bool empty() const noexcept 
        { return ht_.empty(); }

        size_type size() const noexcept 
        { return ht_.size(); }

        size_type max_size() const noexcept 
        { return ht_.max_size(); }

        // modify the container------------------------------------------------------------
        // empalce / empalce_hint
        template <class ...Args>
        pair<iterator, bool> emplace(Args&& ...args)
        { return ht_.emplace_unique(tinystl::forward<Args>(args)...); }

        template <class ...Args>
        iterator emplace_hint(const_iterator hint, Args&& ...args)
        { return ht_.emplace_unique_use_hint(hint, tinystl::forward<Args>(args)...); }

        // insert
        pair<iterator, bool> insert(const value_type& value)
        { return ht_.insert_unique(value); }

        pair<iterator, bool> insert(value_type&& value)
        { return ht_.emplace_unique(tinystl::move(value)); }

        iterator insert(const_iterator hint, const value_type& value)
        { return ht_.insert_unique_use_hint(hint, value); }

        iterator insert(const_iterator hint, value_type&& value)
        { return ht_.emplace_unique_use_hint(hint, tinystl::move(value)); }

        template <class InputIterator>
        void insert(InputIterator first, InputIterator last)
        { ht_.insert_unique(first, last); }

        // erase / clear
        void erase(iterator it)
        { ht_.erase(it); }

        void erase(iterator first, iterator last)
        { ht_.erase(first, last); }

        size_type erase(const key_type& key)
        { return ht_.erase_unique(key); }

        void clear()
        { ht_.clear(); }

        void swap(unordered_set& other) noexcept
        { ht_.swap(other.ht_); }

        //----------------------------------------------------------------------------------
        size_type count(const key_type& key) const 
        { return ht_.count(key); }

        iterator find(const key_type& key) 
        { return ht_.find(key); }

        const_iterator find(const key_type& key)  const 
        { return ht_.find(key); }

        pair<iterator, iterator> equal_range(const key_type& key)
        { return ht_.equal_range_unique(key); }

        pair<const_iterator, const_iterator> equal_range(const key_type& key) const
        { return ht_.equal_range_unique(key); }

        // bucket interface
        local_iterator begin(size_type n) noexcept
        { return ht_.begin(n); }

        const_local_iterator begin(size_type n) const noexcept
        { return ht_.begin(n); }

        const_local_iterator cbegin(size_type n) const noexcept
        { return ht_.cbegin(n); }

        local_iterator end(size_type n) noexcept
        { return ht_.end(n); }

        const_local_iterator end(size_type n) const noexcept
        { return ht_.end(n); }
        
        const_local_iterator cend(size_type n) const noexcept
        { return ht_.cend(n); }

        size_type bucket_count() const noexcept
        { return ht_.bucket_count(); }

        size_type max_bucket_count() const noexcept
        { return ht_.max_bucket_count(); }

        size_type bucket_size(size_type n) const noexcept
        { return ht_.bucket_size(n); }

        size_type bucket(const key_type& key) const
        { return ht_.bucket(key); }

        // hash policy
        float load_factor() const noexcept 
        { return ht_.load_factor(); }

        float max_load_factor() const noexcept 
        { return ht_.max_load_factor(); }

        void  max_load_factor(float ml) 
        { ht_.max_load_factor(ml); }

        void rehash(size_type count)                 
        { ht_.rehash(count); }

        void reserve(size_type count)                
        { ht_.reserve(count); }

        hasher hash_fcn() const          
        { return ht_.hash_fcn(); }

        key_equal key_eq() const          
        { return ht_.key_eq(); }


    public:
        friend bool operator==(const unordered_set& lhs, const unordered_set& rhs)
        {
            return lhs.ht_.equal_range_unique(rhs.ht_);
        }

        friend bool operator!=(const unordered_set& lhs, const unordered_set& rhs)
        {
            return !lhs.ht_.equal_range_unique(rhs.ht_);
        }
    };

    template <class Key, class Hash, class KeyEqual, class Alloc>
    bool operator==(const unordered_set<Key, Hash, KeyEqual>& lhs,
                    const unordered_set<Key, Hash, KeyEqual>& rhs)
    {
        return lhs == rhs;
    }

    template <class Key, class Hash, class KeyEqual, class Alloc>
    bool operator!=(const unordered_set<Key, Hash, KeyEqual>& lhs,
                    const unordered_set<Key, Hash, KeyEqual>& rhs)
    {
        return lhs != rhs;
    }

    // swap
    template <class Key, class Hash, class KeyEqual, class Alloc>
    void swap(unordered_set<Key, Hash, KeyEqual>& lhs,
            unordered_set<Key, Hash, KeyEqual>& rhs)
    {
        lhs.swap(rhs);
    }

    //===== unordered_multiset =============================================================
    // Duplicate key values are allowed
    template <class Key, class Hash = tinystl::hash<Key>, class KeyEqual = tinystl::equal_to<Key>>
    class unordered_multiset
    {
    private:
        typedef hashtable<Key, Hash, KeyEqual> base_type;
        base_type ht_;

    public:
        typedef typename base_type::allocator_type       allocator_type;
        typedef typename base_type::key_type             key_type;
        typedef typename base_type::value_type           value_type;
        typedef typename base_type::hasher               hasher;
        typedef typename base_type::key_equal            key_equal;

        typedef typename base_type::size_type            size_type;
        typedef typename base_type::difference_type      difference_type;
        typedef typename base_type::pointer              pointer;
        typedef typename base_type::const_pointer        const_pointer;
        typedef typename base_type::reference            reference;
        typedef typename base_type::const_reference      const_reference;

        typedef typename base_type::const_iterator       iterator;
        typedef typename base_type::const_iterator       const_iterator;
        typedef typename base_type::const_local_iterator local_iterator;
        typedef typename base_type::const_local_iterator const_local_iterator;

        allocator_type get_allocator() const { return ht_.get_allocator(); }

    public:
        unordered_multiset(): ht_(100, Hash(), KeyEqual())
        {}

        explicit unordered_multiset(size_type bucket_count,
                                    const Hash& hash = Hash(),
                                    const KeyEqual& equal = KeyEqual())
            :ht_(bucket_count, hash, equal)
        {}

        template <class InputIterator>
        unordered_multiset(InputIterator first, InputIterator last,
                           const size_type bucket_count = 100,
                           const Hash& hash = Hash(),
                           const KeyEqual& equal = KeyEqual())
            : ht_(tinystl::max(bucket_count, static_cast<size_type>(tinystl::distance(first, last))), hash, equal)
        {
            for (; first != last; ++first)
            ht_.insert_multi_noresize(*first);
        }

        unordered_multiset(std::initializer_list<value_type> ilist,
                           const size_type bucket_count = 100,
                           const Hash& hash = Hash(),
                           const KeyEqual& equal = KeyEqual())
            :ht_(tinystl::max(bucket_count, static_cast<size_type>(ilist.size())), hash, equal)
        {
            for (auto first = ilist.begin(), last = ilist.end(); first != last; ++first)
            ht_.insert_multi_noresize(*first);
        }

        unordered_multiset(const unordered_multiset& rhs): ht_(rhs.ht_)
        {}

        unordered_multiset(unordered_multiset&& rhs) noexcept : ht_(tinystl::move(rhs.ht_))
        {}

        unordered_multiset& operator=(const unordered_multiset& rhs)
        {
            ht_ = rhs.ht_;
            return *this;
        }

        unordered_multiset& operator=(unordered_multiset&& rhs)
        {
            ht_ = tinystl::move(rhs.ht_);
            return *this;
        }

        unordered_multiset& operator=(std::initializer_list<value_type> ilist)
        {
            ht_.clear();
            ht_.reserve(ilist.size());
            for (auto first = ilist.begin(), last = ilist.end(); first != last; ++first)
                ht_.insert_multi_noresize(*first);
            return *this;
        }

        ~unordered_multiset() = default;
        
        //---------------------------------------------------------------------------------
        iterator begin() noexcept
        { return ht_.begin(); }

        const_iterator begin() const noexcept
        { return ht_.begin(); }

        iterator end() noexcept
        { return ht_.end(); }

        const_iterator end() const noexcept
        { return ht_.end(); }

        const_iterator cbegin() const noexcept
        { return ht_.cbegin(); }

        const_iterator cend() const noexcept
        { return ht_.cend(); }

        bool empty() const noexcept 
        { return ht_.empty(); }

        size_type size() const noexcept 
        { return ht_.size(); }

        size_type max_size() const noexcept 
        { return ht_.max_size(); }

        //----------------------------------------------------------------------------------
        // emplace / emplace_hint
        template <class ...Args>
        iterator emplace(Args&& ...args)
        { return ht_.emplace_multi(tinystl::forward<Args>(args)...); }

        template <class ...Args>
        iterator emplace_hint(const_iterator hint, Args&& ...args)
        { return ht_.emplace_multi_use_hint(hint, tinystl::forward<Args>(args)...); }

        // insert
        iterator insert(const value_type& value)
        { return ht_.insert_multi(value); }

        iterator insert(value_type&& value)
        { return ht_.emplace_multi(tinystl::move(value)); }

        iterator insert(const_iterator hint, const value_type& value)
        { return ht_.insert_multi_use_hint(hint, value); }

        iterator insert(const_iterator hint, value_type&& value)
        { return ht_.emplace_multi_use_hint(hint, tinystl::move(value)); }

        template <class InputIterator>
        void     insert(InputIterator first, InputIterator last)
        { ht_.insert_multi(first, last); }

        // erase / clear
        void erase(iterator it)
        { ht_.erase(it); }

        void erase(iterator first, iterator last)
        { ht_.erase(first, last); }

        size_type erase(const key_type& key)
        { return ht_.erase_multi(key); }

        void clear()
        { ht_.clear(); }

        void swap(unordered_multiset& other) noexcept 
        { ht_.swap(other.ht_); }

        //---------------------------------------------------------------------------------
        size_type count(const key_type& key) const 
        { return ht_.count(key); }

        iterator find(const key_type& key) 
        { return ht_.find(key); }

        const_iterator find(const key_type& key) const 
        { return ht_.find(key); }

        pair<iterator, iterator> equal_range(const key_type& key)
        { return ht_.equal_range_multi(key); }

        pair<const_iterator, const_iterator> equal_range(const key_type& key) const
        { return ht_.equal_range_multi(key); }

        // bucket interface
        local_iterator begin(size_type n) noexcept
        { return ht_.begin(n); }

        const_local_iterator begin(size_type n) const noexcept
        { return ht_.begin(n); }

        const_local_iterator cbegin(size_type n) const noexcept
        { return ht_.cbegin(n); }

        local_iterator end(size_type n) noexcept
        { return ht_.end(n); }

        const_local_iterator end(size_type n) const noexcept
        { return ht_.end(n); }

        const_local_iterator cend(size_type n) const noexcept
        { return ht_.cend(n); }

        size_type bucket_count() const noexcept
        { return ht_.bucket_count(); }

        size_type max_bucket_count() const noexcept
        { return ht_.max_bucket_count(); }

        size_type bucket_size(size_type n) const noexcept
        { return ht_.bucket_size(n); }

        size_type bucket(const key_type& key) const
        { return ht_.bucket(key); }

        // hash policy

        float load_factor() const noexcept 
        { return ht_.load_factor(); }

        float max_load_factor() const noexcept 
        { return ht_.max_load_factor(); }

        void  max_load_factor(float ml) 
        { ht_.max_load_factor(ml); }

        void rehash(size_type count)                 
        { ht_.rehash(count); }

        void reserve(size_type count)                
        { ht_.reserve(count); }

        hasher hash_fcn() const          
        { return ht_.hash_fcn(); }

        key_equal key_eq() const          
        { return ht_.key_eq(); }

    public:
        friend bool operator==(const unordered_multiset& lhs, const unordered_multiset& rhs)
        {
            return lhs.ht_.equal_range_multi(rhs.ht_);
        }

        friend bool operator!=(const unordered_multiset& lhs, const unordered_multiset& rhs)
        {
            return !lhs.ht_.equal_range_multi(rhs.ht_);
        }
    };

    template <class Key, class Hash, class KeyEqual, class Alloc>
    bool operator==(const unordered_multiset<Key, Hash, KeyEqual>& lhs,
                    const unordered_multiset<Key, Hash, KeyEqual>& rhs)
    {
        return lhs == rhs;
    }

    template <class Key, class Hash, class KeyEqual, class Alloc>
    bool operator!=(const unordered_multiset<Key, Hash, KeyEqual>& lhs,
                    const unordered_multiset<Key, Hash, KeyEqual>& rhs)
    {
        return lhs != rhs;
    }

    // swap
    template <class Key, class Hash, class KeyEqual, class Alloc>
    void swap(unordered_multiset<Key, Hash, KeyEqual>& lhs,
            unordered_multiset<Key, Hash, KeyEqual>& rhs)
    {
        lhs.swap(rhs);
    }

} // namespace tinystl
#endif
