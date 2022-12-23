#ifndef _DEQUE_H_
#define _DEQUE_H_

// deque

// Exception guarantees:
// tinystl::deque<T> Satisfy the basic exception guarantee, 
// some functions have no exception guarantee, 
// and strengthen the exception safety guarantee for the following functions:
//   * emplace_front
//   * emplace_back
//   * emplace
//   * push_front
//   * push_back
//   * insert

#include <initializer_list>

#include "iterator.h"
#include "memory.h"
#include "util.h"
#include "exceptdef.h"
#include "allocator.h"
namespace tinystl
{
    #ifdef max
    #pragma message("#undefing marco max")
    #undef max
    #endif // max

    #ifdef min
    #pragma message("#undefing marco min")
    #undef min
    #endif // min

    // deque map initial size
    #ifndef DEQUE_MAP_INIT_SIZE
    #define DEQUE_MAP_INIT_SIZE 8
    #endif

    template <class T>
    struct deque_buf_size
    {
        static constexpr size_t value = sizeof(T) < 256 ? 4096 / sizeof(T) : 16;
    };

    // deque's iterator
    template <class T, class Ref, class Ptr>
    struct deque_iterator : public iterator<random_access_iterator_tag, T>
    {
        typedef deque_iterator<T, T&, T*>             iterator;
        typedef deque_iterator<T, const T&, const T*> const_iterator;
        typedef deque_iterator                        self;

        typedef T            value_type;
        typedef Ptr          pointer;
        typedef Ref          reference;
        typedef size_t       size_type;
        typedef ptrdiff_t    difference_type;
        typedef T*           value_pointer;
        typedef T**          map_pointer; // a pointer to pointer, second pointer

        static const size_type buffer_size = deque_buf_size<T>::value;

        // iterator's member
        value_pointer cur;    // Point to the current element of the buffer
        value_pointer first;  // Point to the head element of the buffer
        value_pointer last;   // Point to the end element of the buffer
        map_pointer   node;   // The node where the buffer is located

        // constructor
        deque_iterator() noexcept
            :cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {}

        deque_iterator(value_pointer v, map_pointer n)
            :cur(v), first(*n), last(*n + buffer_size), node(n) {}

        // copy constructor
        deque_iterator(const iterator& rhs)
            :cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node)
        {}

        // move constructor
        deque_iterator(iterator&& rhs) noexcept
            :cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node)
        {
            rhs.cur = nullptr;
            rhs.first = nullptr;
            rhs.last = nullptr;
            rhs.node = nullptr;
        }

        // const iterator version
        deque_iterator(const const_iterator& rhs)
            :cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node)
        {}

        self& operator=(const iterator& rhs)
        {
            if (this != &rhs)
            {
                cur = rhs.cur;
                first = rhs.first;
                last = rhs.last;
                node = rhs.node;
            }
            return *this;
        }

        // to a buffer
        void set_node(map_pointer new_node)
        {
            node = new_node;
            first = *new_node;
            last = first + buffer_size;
        }

        // overload operator
        reference operator*()  const 
        { 
            // Get the value pointed by the current pointer
            return *cur; 
        }
        pointer operator->() const 
        { 
            return cur; 
        }

        difference_type operator-(const self& x) const
        {
            // [--------][--------][--------]
            // 1   ^               3    !
            // 1 & 3: is the node (also the first)
            // ^ & !: is the current
            // dis: ! minus ^ equal to (3-1)*buffer_size + (! minus first(3)) - (^ minus first(1))
            return static_cast<difference_type>(buffer_size) * (node - x.node)
            + (cur - first) - (x.cur - x.first);
        }

        self& operator++() // Prefix increment operator.
        {
            ++cur;
            if (cur == last)
            { 
                // if reached the end of the current buffer
                set_node(node + 1);
                cur = first;
            }

            return *this;
        }

        self operator++(int) // Postfix increment operator.
        {
            self tmp = *this;
            ++*this;
            return tmp;
        }

        self& operator--()
        {
            if (cur == first)
            { 
                // if reached the head of the current buffer
                set_node(node - 1);
                cur = last;
            }
            --cur;
            return *this;
        }
        self operator--(int)
        {
            self tmp = *this;
            --*this;
            return tmp;
        }

        self& operator+=(difference_type n)
        {
            const auto offset = n + (cur - first);
            if (offset >= 0 && offset < static_cast<difference_type>(buffer_size))
            { 
                // sitll in the current buffer
                cur += n;
            }
            else
            { 
                // Determine whether the offset is 
                // in the buffer in front of the current buffer 
                // or the buffer behind the current buffer
                const auto node_offset = offset > 0
                    ? offset / static_cast<difference_type>(buffer_size)
                    : -static_cast<difference_type>((-offset - 1) / buffer_size) - 1;

                set_node(node + node_offset);
                cur = first + 
                    (offset - node_offset * static_cast<difference_type>(buffer_size));
            }
            return *this;
        }

        self operator+(difference_type n) const
        {
            self tmp = *this;
            return tmp += n;
        }
        self& operator-=(difference_type n)
        {
            // use += 
            return *this += -n;
        }
        self operator-(difference_type n) const
        {
            self tmp = *this;
            return tmp -= n;
        }

        reference operator[](difference_type n) const { return *(*this + n); }

        // Overloading comparison operators
        bool operator==(const self& rhs) const 
        { 
            return cur == rhs.cur; 
        }
        bool operator< (const self& rhs) const
        { 
            return node == rhs.node ? (cur < rhs.cur) : (node < rhs.node); 
        }
        bool operator!=(const self& rhs) const 
        { 
            return !(*this == rhs); 
        }
        bool operator> (const self& rhs) const 
        { 
            return rhs < *this; 
        }
        bool operator<=(const self& rhs) const 
        { 
            return !(rhs < *this); 
        }
        bool operator>=(const self& rhs) const 
        { 
            return !(*this < rhs); 
        }
    };

    // deque
    // Template parameters represent data types
    template <class T>
    class deque
    {
    public:
        // deque's type
        typedef tinystl::allocator<T>       allocator_type;
        typedef tinystl::allocator<T>       data_allocator;
        typedef tinystl::allocator<T*>      map_allocator;

        typedef typename allocator_type::value_type         value_type;
        typedef typename allocator_type::pointer            pointer;
        typedef typename allocator_type::const_pointer      const_pointer;
        typedef typename allocator_type::reference          reference;
        typedef typename allocator_type::const_reference    const_reference;
        typedef typename allocator_type::size_type          size_type;
        typedef typename allocator_type::difference_type    difference_type;

        typedef pointer*        map_pointer;
        typedef const_pointer*  const_map_pointer;

        typedef deque_iterator<T, T&, T*>                   iterator;
        typedef deque_iterator<T, const T&, const T*>       const_iterator;
        typedef tinystl::reverse_iterator<iterator>         reverse_iterator;
        typedef tinystl::reverse_iterator<const_iterator>   const_reverse_iterator;

        allocator_type get_allocator() { return allocator_type(); }

        static const size_type buffer_size = deque_buf_size<T>::value;

    private:
        // the following variables are used to represent a deque
        iterator       begin_;     // point to the first node
        iterator       end_;       // point to the last node

        // point to the map, each element in the map is a pointer to a buffer
        map_pointer    map_;       
        size_type      map_size_;  // the number of pointers in map

    public:
        // construct function-----------------------------------------------------------------
        deque()
        { 
            fill_init(0, value_type()); 
        }

        explicit deque(size_type n)
        { 
            fill_init(n, value_type()); 
        }

        deque(size_type n, const value_type& value)
        { 
            fill_init(n, value);
        }

        template <class IIter, typename std::enable_if<
            tinystl::is_input_iterator<IIter>::value, int>::type = 0>
        deque(IIter first, IIter last)
        { 
            copy_init(first, last, iterator_category(first)); 
        }

        // std::initializer_list:
        // is a standard library type used to represent an array of values of a certain type
        // The elements in the initializer_list object are always constant values, and we cannot change the value of the elements in the initializer_list object.
        // The element data type stored in initializer_list is value_type
        deque(std::initializer_list<value_type> ilist)
        {
            copy_init(ilist.begin(), ilist.end(), tinystl::forward_iterator_tag());
        }

        deque(const deque& rhs)
        {
            copy_init(rhs.begin(), rhs.end(), tinystl::forward_iterator_tag());
        }

        // move constructor
        deque(deque&& rhs) noexcept
            :begin_(tinystl::move(rhs.begin_)),end_(tinystl::move(rhs.end_)),map_(rhs.map_),
            map_size_(rhs.map_size_)
        {
            rhs.map_ = nullptr;
            rhs.map_size_ = 0;
        }

        deque& operator=(const deque& rhs);
        // move assignment operator
        deque& operator=(deque&& rhs);

        deque& operator=(std::initializer_list<value_type> ilist)
        {
            deque tmp(ilist);
            swap(tmp);
            return *this;
        }

        ~deque()
        {
            if (map_ != nullptr)
            {
                clear();
                data_allocator::deallocate(*begin_.node, buffer_size);
                *begin_.node = nullptr;
                map_allocator::deallocate(map_, map_size_);
                map_ = nullptr;
            }
        }

    public:
        //----------------------------------------------------------------------------------
        iterator  begin() noexcept
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

        reverse_iterator rbegin() noexcept
        { 
            return reverse_iterator(end()); 
        }
        const_reverse_iterator rbegin()  const noexcept
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

        //-------------------------------------------------------------------------------------
        // capacity
        bool empty() const noexcept  
        { 
            return begin() == end(); 
        }
        size_type size() const noexcept  
        { 
            return end_ - begin_; 
        }
        size_type max_size() const noexcept 
        { 
            return static_cast<size_type>(-1); 
        }
        void resize(size_type new_size) 
        { 
            resize(new_size, value_type()); 
        }
        void resize(size_type new_size, const value_type& value);

        void shrink_to_fit() noexcept;

        // access element 
        // [] overload
        reference operator[](size_type n)
        {
            TINYSTL_DEBUG(n < size());
            return begin_[n];
        }
        const_reference operator[](size_type n) const
        {
            TINYSTL_DEBUG(n < size());
            return begin_[n];
        }

        reference at(size_type n)      
        { 
            THROW_OUT_OF_RANGE_IF(!(n < size()), "deque<T>::at() subscript out of range");
            return (*this)[n];
        }
        const_reference at(size_type n) const
        {
            THROW_OUT_OF_RANGE_IF(!(n < size()), "deque<T>::at() subscript out of range");
            return (*this)[n]; 
        }

        reference front()
        {
            TINYSTL_DEBUG(!empty());
            return *begin();
        }
        const_reference front() const
        {
            TINYSTL_DEBUG(!empty());
            return *begin();
        }
        reference back()
        {
            TINYSTL_DEBUG(!empty());
            return *(end() - 1);
        }
        const_reference back() const
        {
            TINYSTL_DEBUG(!empty());
            return *(end() - 1);
        }

        //---------------------------------------------------------------------------------
        // change iterator
        // assign
        void assign(size_type n, const value_type& value)
        { 
            fill_assign(n, value); 
        }

        template <class IIter, typename std::enable_if<
            tinystl::is_input_iterator<IIter>::value, int>::type = 0>
        void assign(IIter first, IIter last)
        { 
            copy_assign(first, last, iterator_category(first)); 
        }

        void assign(std::initializer_list<value_type> ilist)
        { 
            copy_assign(ilist.begin(), ilist.end(), tinystl::forward_iterator_tag{}); 
        }

        // emplace_front / emplace_back / emplace
        template <class ...Args>
        void emplace_front(Args&& ...args);

        template <class ...Args>
        void emplace_back(Args&& ...args);

        template <class ...Args>
        iterator emplace(iterator pos, Args&& ...args);

        // push_front / push_back
        void push_front(const value_type& value);
        void push_back(const value_type& value);

        void push_front(value_type&& value) 
        { 
            emplace_front(tinystl::move(value)); 
        }
        void push_back(value_type&& value)  
        { 
            emplace_back(tinystl::move(value)); 
        }

        // pop_back / pop_front
        void pop_front();
        void pop_back();

        // insert
        iterator insert(iterator position, const value_type& value);

        iterator insert(iterator position, value_type&& value);

        void insert(iterator position, size_type n, const value_type& value);

        template <class IIter, typename std::enable_if<
            tinystl::is_input_iterator<IIter>::value, int>::type = 0>
        void insert(iterator position, IIter first, IIter last)
        { 
            insert_dispatch(position, first, last, iterator_category(first)); 
        }

        // erase /clear
        iterator erase(iterator position);
        iterator erase(iterator first, iterator last);
        
        void clear();

        // swap
        void swap(deque& rhs) noexcept;

    private:
        // helper functions
        // create node / destroy node
        map_pointer create_map(size_type size);

        void create_buffer(map_pointer nstart, map_pointer nfinish);
        void destroy_buffer(map_pointer nstart, map_pointer nfinish);

        // initialize
        void map_init(size_type nelem);
        void fill_init(size_type n, const value_type& value);

        template <class IIter>
        void copy_init(IIter, IIter, input_iterator_tag);

        template <class FIter>
        void copy_init(FIter, FIter, forward_iterator_tag);

        // assign
        void fill_assign(size_type n, const value_type& value);

        template <class IIter>
        void copy_assign(IIter first, IIter last, input_iterator_tag);

        template <class FIter>
        void copy_assign(FIter first, FIter last, forward_iterator_tag);

        // insert
        template <class... Args>
        iterator insert_aux(iterator position, Args&& ...args);

        void fill_insert(iterator position, size_type n, const value_type& x);

        template <class FIter>
        void copy_insert(iterator, FIter, FIter, size_type);
        
        template <class IIter>
        void insert_dispatch(iterator, IIter, IIter, input_iterator_tag);

        template <class FIter>
        void insert_dispatch(iterator, FIter, FIter, forward_iterator_tag);

        // reallocate
        void require_capacity(size_type n, bool front);
        void reallocate_map_at_front(size_type need);
        void reallocate_map_at_back(size_type need);
    };

    //========= implement =================================================================

    // fill_init 
    template <class T>
    void deque<T>::fill_init(size_type n, const value_type& value)
    {
        map_init(n);
        if (n != 0)
        {
            for (auto cur = begin_.node; cur < end_.node; ++cur)
            {
                tinystl::uninitialized_fill(*cur, *cur + buffer_size, value);
            }
            // fill end_
            tinystl::uninitialized_fill(end_.first, end_.cur, value);
        }
    }

    // copy_init 
    template <class T>
    template <class IIter>
    void deque<T>::copy_init(IIter first, IIter last, input_iterator_tag)
    {
        const size_type n = tinystl::distance(first, last);
        map_init(n);
        for (; first != last; ++first)
            emplace_back(*first);
    }

    template <class T>
    template <class FIter>
    void deque<T>::copy_init(FIter first, FIter last, forward_iterator_tag)
    {
        const size_type n = tinystl::distance(first, last);
        map_init(n);
        for (auto cur = begin_.node; cur < end_.node; ++cur)
        {
            auto next = first;
            tinystl::advance(next, buffer_size);
            tinystl::uninitialized_copy(first, next, *cur);
            first = next;
        }
        tinystl::uninitialized_copy(first, last, end_.first);
    }

    // shrink the capacity
    template <class T>
    void deque<T>::shrink_to_fit() noexcept
    {
        // Reserve the buffer for the header at least
        for (auto cur = map_; cur < begin_.node; ++cur)
        {
            data_allocator::deallocate(*cur, buffer_size);
            *cur = nullptr;
        }
        for (auto cur = end_.node + 1; cur < map_ + map_size_; ++cur)
        {
            data_allocator::deallocate(*cur, buffer_size);
            *cur = nullptr;
        }
    }

    // clear deque
    template <class T>
    void deque<T>::clear()
    {
        // clear: Reserve the buffer for the header.
        for (map_pointer cur = begin_.node + 1; cur < end_.node; ++cur)
        {
            data_allocator::destroy(*cur, *cur + buffer_size);
        }
        if (begin_.node != end_.node)
        { 
            // >= two buffers
            tinystl::destroy(begin_.cur, begin_.last);
            tinystl::destroy(end_.first, end_.cur);
        }
        else
        {
            tinystl::destroy(begin_.cur, end_.cur);
        }
        shrink_to_fit();
        end_ = begin_;
    }

    // copy assignment operator
    template <class T>
    deque<T>& deque<T>::operator=(const deque& rhs)
    {
        if (this != &rhs)
        {
            const auto len = size();
            if (len >= rhs.size())
            {
                erase(tinystl::copy(rhs.begin_, rhs.end_, begin_), end_);
            }
            else
            {
                iterator mid = rhs.begin() + static_cast<difference_type>(len);
                tinystl::copy(rhs.begin_, mid, begin_);
                insert(end_, mid, rhs.end_);
            }
        }
        return *this;
    }

    // move assignment operator
    template <class T>
    deque<T>& deque<T>::operator=(deque&& rhs)
    {
        clear();
        begin_ = tinystl::move(rhs.begin_);
        end_ = tinystl::move(rhs.end_);
        map_ = rhs.map_;
        map_size_ = rhs.map_size_;
        rhs.map_ = nullptr;
        rhs.map_size_ = 0;
        return *this;
    }
}
#endif
