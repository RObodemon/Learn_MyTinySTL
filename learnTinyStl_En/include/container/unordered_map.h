#ifndef _UNORDERED_MAP_H_
#define _UNORDERED_MAP_H_

// unordered_map and unordered_multimap

// notes:
// Exception guarantees:
// tinystl::unordered_map<Key, T> / tinystl::unordered_multimap<Key, T> 
// Satisfy the basic exception guarantee, 
// and strengthen the exception safety guarantee for the following functions:
//   * emplace
//   * emplace_hint
//   * insert

#include "hashtable.h"

namespace tinystl
{
    //=============== unordered_map ========================================================
    // Duplicate key values are not allowed
    // first parameter: key type
    // second parameter: value type
    // third paramete: hash function, default: tinystl::hash
    // foutth parameter: comparison method, default: tinystl::equal_to
    template <class Key, class T, class Hash = tinystl::hash<Key>, class KeyEqual = tinystl::equal_to<Key>>
    class unordered_map
    {
    private:
        typedef hashtable<tinystl::pair<const Key, T>, Hash, KeyEqual> base_type;
        base_type ht_;

    public: 
        typedef typename base_type::allocator_type       allocator_type;
        typedef typename base_type::key_type             key_type;
        typedef typename base_type::mapped_type          mapped_type;
        typedef typename base_type::value_type           value_type;
        typedef typename base_type::hasher               hasher;
        typedef typename base_type::key_equal            key_equal;

        typedef typename base_type::size_type            size_type;
        typedef typename base_type::difference_type      difference_type;
        typedef typename base_type::pointer              pointer;
        typedef typename base_type::const_pointer        const_pointer;
        typedef typename base_type::reference            reference;
        typedef typename base_type::const_reference      const_reference;

        typedef typename base_type::iterator             iterator;
        typedef typename base_type::const_iterator       const_iterator;
        typedef typename base_type::local_iterator       local_iterator;
        typedef typename base_type::const_local_iterator const_local_iterator;

        allocator_type get_allocator() const { return ht_.get_allocator(); }

    public:

        unordered_map(): ht_(100, Hash(), KeyEqual())
        {}

        explicit unordered_map(size_type bucket_count, const Hash& hash = Hash(),
                               const KeyEqual& equal = KeyEqual())
            :ht_(bucket_count, hash, equal)
        {}

        template <class InputIterator>
        unordered_map(InputIterator first, InputIterator last,
                      const size_type bucket_count = 100,
                      const Hash& hash = Hash(),
                      const KeyEqual& equal = KeyEqual())
            : ht_(tinystl::max(bucket_count, static_cast<size_type>(tinystl::distance(first, last))), hash, equal)
        {
            for (; first != last; ++first)
            ht_.insert_unique_noresize(*first);
        }

        unordered_map(std::initializer_list<value_type> ilist,
                      const size_type bucket_count = 100,
                      const Hash& hash = Hash(),
                      const KeyEqual& equal = KeyEqual())
            :ht_(tinystl::max(bucket_count, static_cast<size_type>(ilist.size())), hash, equal)
        {
            for (auto first = ilist.begin(), last = ilist.end(); first != last; ++first)
            ht_.insert_unique_noresize(*first);
        }

        unordered_map(const unordered_map& rhs): ht_(rhs.ht_) 
        {}

        unordered_map(unordered_map&& rhs) noexcept : ht_(tinystl::move(rhs.ht_)) 
        {}

        unordered_map& operator=(const unordered_map& rhs) 
        { 
            ht_ = rhs.ht_;
            return *this; 
        }

        unordered_map& operator=(unordered_map&& rhs) 
        { 
            ht_ = tinystl::move(rhs.ht_);
            return *this;
        }

        unordered_map& operator=(std::initializer_list<value_type> ilist)
        {
            ht_.clear();
            ht_.reserve(ilist.size());
            for (auto first = ilist.begin(), last = ilist.end(); first != last; ++first)
            ht_.insert_unique_noresize(*first);
            return *this;
        }

        ~unordered_map() = default;

        //----------------------------------------------------------------------------------
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

        //----------------------------------------------------------------------------------
        bool empty() const noexcept 
        { return ht_.empty(); }

        size_type size() const noexcept 
        { return ht_.size(); }

        size_type max_size() const noexcept 
        { return ht_.max_size(); }

        //----------------------------------------------------------------------------------
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

        void swap(unordered_map& other) noexcept
        { ht_.swap(other.ht_); }

        //----------------------------------------------------------------------------------
        mapped_type& at(const key_type& key)
        {
            iterator it = ht_.find(key);
            THROW_OUT_OF_RANGE_IF(it.node == nullptr, "unordered_map<Key, T> no such element exists");
            return it->second;
        }

        const mapped_type& at(const key_type& key) const
        {
            iterator it = ht_.find(key);
            THROW_OUT_OF_RANGE_IF(it.node == nullptr, "unordered_map<Key, T> no such element exists");
            return it->second;
        }

        mapped_type& operator[](const key_type& key)
        {
            iterator it = ht_.find(key);
            if (it.node == nullptr)
            it = ht_.emplace_unique(key, T{}).first;
            return it->second;
        }

        mapped_type& operator[](key_type&& key)
        {
            iterator it = ht_.find(key);
            if (it.node == nullptr)
            it = ht_.emplace_unique(tinystl::move(key), T{}).first;
            return it->second;
        }

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
        friend bool operator==(const unordered_map& lhs, const unordered_map& rhs)
        {
            return lhs.ht_.equal_range_unique(rhs.ht_);
        }

        friend bool operator!=(const unordered_map& lhs, const unordered_map& rhs)
        {
            return !lhs.ht_.equal_range_unique(rhs.ht_);
        }
    };

    template <class Key, class T, class Hash, class KeyEqual>
    bool operator==(const unordered_map<Key, T, Hash, KeyEqual>& lhs,
                    const unordered_map<Key, T, Hash, KeyEqual>& rhs)
    {
        return lhs == rhs;
    }

    template <class Key, class T, class Hash, class KeyEqual>
    bool operator!=(const unordered_map<Key, T, Hash, KeyEqual>& lhs,
                    const unordered_map<Key, T, Hash, KeyEqual>& rhs)
    {
        return lhs != rhs;
    }

    // swap
    template <class Key, class T, class Hash, class KeyEqual>
    void swap(unordered_map<Key, T, Hash, KeyEqual>& lhs,
              unordered_map<Key, T, Hash, KeyEqual>& rhs)
    {
        lhs.swap(rhs);
    }


    //=================== unordered_multimap ===============================================
    // Duplicate key values are not allowed
    // first parameter: key type
    // second parameter: value type
    // third parameter: hash function, default: tinystl::hash
    // fourth parameter: comparison method, default: tinystl::equal_to
    template <class Key, class T, class Hash = tinystl::hash<Key>, class KeyEqual = tinystl::equal_to<Key>>
    class unordered_multimap
    {
    private:
        typedef hashtable<pair<const Key, T>, Hash, KeyEqual> base_type;
        base_type ht_;

    public:
        typedef typename base_type::allocator_type       allocator_type;
        typedef typename base_type::key_type             key_type;
        typedef typename base_type::mapped_type          mapped_type;
        typedef typename base_type::value_type           value_type;
        typedef typename base_type::hasher               hasher;
        typedef typename base_type::key_equal            key_equal;

        typedef typename base_type::size_type            size_type;
        typedef typename base_type::difference_type      difference_type;
        typedef typename base_type::pointer              pointer;
        typedef typename base_type::const_pointer        const_pointer;
        typedef typename base_type::reference            reference;
        typedef typename base_type::const_reference      const_reference;

        typedef typename base_type::iterator             iterator;
        typedef typename base_type::const_iterator       const_iterator;
        typedef typename base_type::local_iterator       local_iterator;
        typedef typename base_type::const_local_iterator const_local_iterator;

        allocator_type get_allocator() const { return ht_.get_allocator(); }

    public:
        unordered_multimap(): ht_(100, Hash(), KeyEqual())
        {}

        explicit unordered_multimap(size_type bucket_count,
                                    const Hash& hash = Hash(),
                                    const KeyEqual& equal = KeyEqual())
            :ht_(bucket_count, hash, equal) 
        {}

        template <class InputIterator>
        unordered_multimap(InputIterator first, InputIterator last,
                            const size_type bucket_count = 100,
                            const Hash& hash = Hash(),
                            const KeyEqual& equal = KeyEqual())
            :ht_(tinystl::max(bucket_count, static_cast<size_type>(tinystl::distance(first, last))), hash, equal)
        {
            for (; first != last; ++first)
            ht_.insert_multi_noresize(*first);
        }

        unordered_multimap(std::initializer_list<value_type> ilist,
                            const size_type bucket_count = 100,
                            const Hash& hash = Hash(),
                            const KeyEqual& equal = KeyEqual())
            :ht_(tinystl::max(bucket_count, static_cast<size_type>(ilist.size())), hash, equal)
        {
            for (auto first = ilist.begin(), last = ilist.end(); first != last; ++first)
            ht_.insert_multi_noresize(*first);
        }

        unordered_multimap(const unordered_multimap& rhs): ht_(rhs.ht_) 
        {}

        unordered_multimap(unordered_multimap&& rhs) noexcept : ht_(tinystl::move(rhs.ht_))
        {}

        unordered_multimap& operator=(const unordered_multimap& rhs)
        { 
            ht_ = rhs.ht_; 
            return *this;
        }

        unordered_multimap& operator=(unordered_multimap&& rhs)
        { 
            ht_ = tinystl::move(rhs.ht_); 
            return *this;
        }

        unordered_multimap& operator=(std::initializer_list<value_type> ilist)
        {
            ht_.clear();
            ht_.reserve(ilist.size());
            for (auto first = ilist.begin(), last = ilist.end(); first != last; ++first)
            ht_.insert_multi_noresize(*first);
            return *this;
        }

        ~unordered_multimap() = default;

        //----------------------------------------------------------------------------------
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

        //----------------------------------------------------------------------------------
        bool empty() const noexcept 
        { return ht_.empty(); }

        size_type size() const noexcept 
        { return ht_.size(); }

        size_type max_size() const noexcept 
        { return ht_.max_size(); }

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

        void swap(unordered_multimap& other) noexcept 
        { ht_.swap(other.ht_); }

        //----------------------------------------------------------------------------------
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

        void max_load_factor(float ml)               
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
        friend bool operator==(const unordered_multimap& lhs, const unordered_multimap& rhs)
        {
            return lhs.ht_.equal_range_multi(rhs.ht_);
        }

        friend bool operator!=(const unordered_multimap& lhs, const unordered_multimap& rhs)
        {
            return !lhs.ht_.equal_range_multi(rhs.ht_);
        }
    };

    template <class Key, class T, class Hash, class KeyEqual>
    bool operator==(const unordered_multimap<Key, T, Hash, KeyEqual>& lhs,
                    const unordered_multimap<Key, T, Hash, KeyEqual>& rhs)
    {
        return lhs == rhs;
    }

    template <class Key, class T, class Hash, class KeyEqual>
    bool operator!=(const unordered_multimap<Key, T, Hash, KeyEqual>& lhs,
                    const unordered_multimap<Key, T, Hash, KeyEqual>& rhs)
    {
        return lhs != rhs;
    }

    // swap
    template <class Key, class T, class Hash, class KeyEqual>
    void swap(unordered_multimap<Key, T, Hash, KeyEqual>& lhs,
            unordered_multimap<Key, T, Hash, KeyEqual>& rhs)
    {
    lhs.swap(rhs);
    }

} // namespace tinystl
#endif