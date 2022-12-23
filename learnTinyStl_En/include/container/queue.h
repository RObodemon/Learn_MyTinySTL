#ifndef _QUEUE_H_
#define _QUEUE_H_

// queue          
// priority_queue 

// queue:
// adapter, which gather an underlying container inside 
// and modify the container's representation by changing the interface.

// Both list and deque can be used as the underlying services of these two
// And neither of these provide iterators, so they cannot be traversed

// The queue does not implement any functions by itself, 
// all of them are the functions of the container c_ used.

#include "deque.h"
#include "vector.h"
#include "functional.h"
#include "heap_algo.h"

namespace tinystl
{
    //============== queue =================================================================
    // template class queue
    template <class T, class Container = tinystl::deque<T>>
    class queue
    {
    public:
        typedef Container                               container_type;
        typedef typename Container::value_type          value_type;
        typedef typename Container::size_type           size_type;
        typedef typename Container::reference           reference;
        typedef typename Container::const_reference     const_reference;

        static_assert(std::is_same<T, value_type>::value,
                        "the value_type of Container should be same with T");
    private:
        container_type c_;  // queue

    public:
        //----------------------------------------------------------------------------------
        // constructor
        queue() = default;

        explicit queue(size_type n) :c_(n) 
        {}

        queue(size_type n, const value_type& value) :c_(n, value)
        {}

        template <class IIter>
        queue(IIter first, IIter last) :c_(first, last) 
        {}

        queue(std::initializer_list<T> ilist) :c_(ilist.begin(), ilist.end()) 
        {}

        queue(const Container& c) :c_(c) 
        {}

        queue(Container&& c) noexcept(std::is_nothrow_move_constructible<Container>::value)
            :c_(tinystl::move(c)) 
        {}

        queue(const queue& rhs) 
            :c_(rhs.c_) 
        {}

        queue(queue&& rhs) noexcept(std::is_nothrow_move_constructible<Container>::value)
            :c_(tinystl::move(rhs.c_)) 
        {}

        queue& operator=(const queue& rhs) 
        {
            c_ = rhs.c_; 
            return *this; 
        }

        queue& operator=(queue&& rhs)
            noexcept(std::is_nothrow_move_assignable<Container>::value)
        { 
            c_ = tinystl::move(rhs.c_);
            return *this;
        }

        queue& operator=(std::initializer_list<T> ilist)
        { 
            c_ = ilist; 
            return *this; 
        }

        ~queue() = default;

        //----------------------------------------------------------------------------------
        // visit elements
        reference front()       
        { return c_.front(); }

        const_reference front() const 
        { return c_.front(); }

        reference back()        
        { return c_.back(); }

        const_reference back()  const 
        { return c_.back(); }

        bool empty() const noexcept 
        { return c_.empty(); }

        size_type size()  const noexcept 
        { return c_.size(); }

        // modify container
        template <class ...Args>
        void emplace(Args&& ...args)
        { c_.emplace_back(tinystl::forward<Args>(args)...); }

        void push(const value_type& value) 
        { c_.push_back(value); }

        void push(value_type&& value)      
        { c_.emplace_back(tinystl::move(value)); }

        void pop()                         
        { c_.pop_front(); }

        void clear()         
        { 
            while (!empty())
            pop(); 
        }

        void swap(queue& rhs) noexcept(noexcept(tinystl::swap(c_, rhs.c_)))
        { tinystl::swap(c_, rhs.c_); }

        public:
        friend bool operator==(const queue& lhs, const queue& rhs) { return lhs.c_ == rhs.c_; }
        friend bool operator< (const queue& lhs, const queue& rhs) { return lhs.c_ <  rhs.c_; }
    };

    // overload comparison operator
    template <class T, class Container>
    bool operator==(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
    {
        return lhs == rhs;
    }

    template <class T, class Container>
    bool operator!=(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
    {
        return !(lhs == rhs);
    }

    template <class T, class Container>
    bool operator<(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
    {
        return lhs < rhs;
    }

    template <class T, class Container>
    bool operator>(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
    {
        return rhs < lhs;
    }

    template <class T, class Container>
    bool operator<=(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
    {
        return !(rhs < lhs);
    }

    template <class T, class Container>
    bool operator>=(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
    {
        return !(lhs < rhs);
    }

    // overload swap
    template <class T, class Container>
    void swap(queue<T, Container>& lhs, queue<T, Container>& rhs) 
        noexcept(noexcept(lhs.swap(rhs)))
    {
        lhs.swap(rhs);
    }

    //============== priority_queue ========================================================
    // The difference between priority queue and queue: 
    // the normal queue follows the principle of first in first out; 
    // the dequeue order of the priority queue has nothing to do with the queue order, 
    // but is related to the priority.

    // -------------------------------------------------------------------------------------
    // Implement a priority queue with a max heap
    // -------------------------------------------------------------------------------------

    // template class: priority_queue
    // By default tinystl::vector is used as the underlying container
    // third parameter: represents the way to compare weights, 
    // and tinystl::less is used as the comparison method by default
    template <class T, class Container = tinystl::vector<T>,
        class Compare = tinystl::less<typename Container::value_type>>
    class priority_queue
    {
    public:
        typedef Container                               container_type;
        typedef Compare                                 value_compare;
        typedef typename Container::value_type          value_type;
        typedef typename Container::size_type           size_type;
        typedef typename Container::reference           reference;
        typedef typename Container::const_reference     const_reference;

        static_assert(std::is_same<T, value_type>::value,
                        "the value_type of Container should be same with T");

    private:
        container_type c_;     // vector
        value_compare  comp_;  // Standards for weight comparison

    public:
        // constructor
        priority_queue() = default;

        priority_queue(const Compare& c) :c_(), comp_(c) 
        {}

        explicit priority_queue(size_type n) :c_(n)
        {
            tinystl::make_heap(c_.begin(), c_.end(), comp_);
        }
        priority_queue(size_type n, const value_type& value) :c_(n, value)
        {
            tinystl::make_heap(c_.begin(), c_.end(), comp_);
        }

        template <class IIter>
        priority_queue(IIter first, IIter last) :c_(first, last)
        {
            tinystl::make_heap(c_.begin(), c_.end(), comp_);
        }

        priority_queue(std::initializer_list<T> ilist) :c_(ilist)
        {
            tinystl::make_heap(c_.begin(), c_.end(), comp_);
        }

        priority_queue(const Container& s) :c_(s)
        {
            tinystl::make_heap(c_.begin(), c_.end(), comp_);
        }

        priority_queue(Container&& s) :c_(tinystl::move(s))
        {
            tinystl::make_heap(c_.begin(), c_.end(), comp_);
        }

        priority_queue(const priority_queue& rhs) :c_(rhs.c_), comp_(rhs.comp_)
        {
            tinystl::make_heap(c_.begin(), c_.end(), comp_);
        }

        priority_queue(priority_queue&& rhs) :c_(tinystl::move(rhs.c_)), comp_(rhs.comp_)
        {
            tinystl::make_heap(c_.begin(), c_.end(), comp_);
        }

        priority_queue& operator=(const priority_queue& rhs)
        {
            c_ = rhs.c_;
            comp_ = rhs.comp_;
            tinystl::make_heap(c_.begin(), c_.end(), comp_);
            return *this;
        }

        priority_queue& operator=(priority_queue&& rhs)
        {
            c_ = tinystl::move(rhs.c_);
            comp_ = rhs.comp_;
            tinystl::make_heap(c_.begin(), c_.end(), comp_);
            return *this;
        }

        priority_queue& operator=(std::initializer_list<T> ilist)
        {
            c_ = ilist;
            comp_ = value_compare();
            tinystl::make_heap(c_.begin(), c_.end(), comp_);
            return *this;
        }

        ~priority_queue() = default;

    public:

        // visit elements
        const_reference top() const 
        { return c_.front(); }

        bool empty() const noexcept 
        { return c_.empty(); }

        size_type size()  const noexcept 
        { return c_.size(); }

        // modify container
        template <class... Args>
        void emplace(Args&& ...args)
        {
            c_.emplace_back(tinystl::forward<Args>(args)...);
            tinystl::push_heap(c_.begin(), c_.end(), comp_);
        }

        void push(const value_type& value)
        {
            c_.push_back(value);
            tinystl::push_heap(c_.begin(), c_.end(), comp_);
        }
        void push(value_type&& value)
        {
            c_.push_back(tinystl::move(value));
            tinystl::push_heap(c_.begin(), c_.end(), comp_);
        }

        void pop()
        {
            tinystl::pop_heap(c_.begin(), c_.end(), comp_);
            c_.pop_back();
        }

        void clear()
        {
            while (!empty())
            pop();
        }

        void swap(priority_queue& rhs) noexcept(noexcept(tinystl::swap(c_, rhs.c_)) &&
                                                noexcept(tinystl::swap(comp_, rhs.comp_)))
        {
            tinystl::swap(c_, rhs.c_);
            tinystl::swap(comp_, rhs.comp_);
        }

        public:
        friend bool operator==(const priority_queue& lhs, const priority_queue& rhs)
        {
            return lhs.c_ == rhs.c_;
        }
        friend bool operator!=(const priority_queue& lhs, const priority_queue& rhs)
        {
            return lhs.c_ != rhs.c_;
        }
    };

    // overload 
    template <class T, class Container, class Compare>
    bool operator==(priority_queue<T, Container, Compare>& lhs,
                    priority_queue<T, Container, Compare>& rhs)
    {
        return lhs == rhs;
    }

    template <class T, class Container, class Compare>
    bool operator!=(priority_queue<T, Container, Compare>& lhs,
                    priority_queue<T, Container, Compare>& rhs)
    {
        return lhs != rhs;
    }

    // swap
    template <class T, class Container, class Compare>
    void swap(priority_queue<T, Container, Compare>& lhs, 
        priority_queue<T, Container, Compare>& rhs) noexcept(noexcept(lhs.swap(rhs)))
    {
        lhs.swap(rhs);
    }

} // namespace tinystl
#endif 