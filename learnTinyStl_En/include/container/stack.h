#ifndef _STACK_H_
#define _STACK_H_

// stack 
// adapter, which gather an underlying container inside 
// and modify the container's representation by changing the interface.

// Both list and deque can be used as the underlying services of these two
// And neither of these provide iterators, so they cannot be traversed

// The stack does not implement any functions by itself, 
// all of them are the functions of the container c_ used.

#include "deque.h"    

namespace tinystl
{
    // template class stack
    // first parameter: value type
    // second parameter: underlying container type
    // default: tinystl::deque as the underlying container type
    template <class T, class Container = tinystl::deque<T>>
    class stack
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
        container_type c_;  // stack

    public:
        //----------------------------------------------------------------------------------
        // constructor
        stack() = default;

        explicit stack(size_type n) :c_(n)
        {}

        stack(size_type n, const value_type& value) :c_(n, value)
        {}

        template <class IIter>
        stack(IIter first, IIter last): c_(first, last)
        {}

        stack(std::initializer_list<T> ilist) :c_(ilist.begin(), ilist.end())
        {}

        stack(const Container& c):c_(c)
        {}
        
        // is_nothrow_move_constructible: in <type_traits.h>
        // Tests whether type has a nothrow move constructor.
        // Used to determine whether an exception is thrown
        stack(Container&& c) noexcept(std::is_nothrow_move_constructible<Container>::value)
            :c_(tinystl::move(c)) 
        {}

        stack(const stack& rhs) :c_(rhs.c_) 
        {}

        stack(stack&& rhs) noexcept(std::is_nothrow_move_constructible<Container>::value)
            :c_(tinystl::move(rhs.c_))
        {
        }

        stack& operator=(const stack& rhs)
        {
            c_ = rhs.c_;
            return *this;
        }
        stack& operator=(stack&& rhs) noexcept(std::is_nothrow_move_assignable<Container>::value)
        { 
            c_ = tinystl::move(rhs.c_); 
            return *this;
        }

        stack& operator=(std::initializer_list<T> ilist) 
        {
            c_ = ilist; 
            return *this;
        }

        ~stack() = default;
        // operation on the back
        // visit elements
        reference top()       
        { return c_.back(); }

        const_reference top() const 
        { return c_.back(); }

        // container related operations
        bool empty() const noexcept 
        { return c_.empty(); }

        size_type size()  const noexcept 
        { return c_.size(); }

        // modify the container-------------------------------------------------------------
        template <class... Args>
        void emplace(Args&& ...args)
        { c_.emplace_back(tinystl::forward<Args>(args)...); }

        void push(const value_type& value)
        { c_.push_back(value); }

        void push(value_type&& value)      
        { c_.push_back(tinystl::move(value)); }

        void pop() 
        { c_.pop_back(); }

        void clear() 
        {
            while (!empty())
            pop();
        }

        void swap(stack& rhs) noexcept(noexcept(tinystl::swap(c_, rhs.c_)))
        { tinystl::swap(c_, rhs.c_); }

    public:
        friend bool operator==(const stack& lhs, const stack& rhs) 
        { return lhs.c_ == rhs.c_; }

        friend bool operator< (const stack& lhs, const stack& rhs) 
        { return lhs.c_ <  rhs.c_; }
    };

    // overload comparison operator
    template <class T, class Container>
    bool operator==(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
    {
        return lhs == rhs;
    }

    template <class T, class Container>
    bool operator<(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
    {
        return lhs < rhs;
    }

    template <class T, class Container>
    bool operator!=(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
    {
        return !(lhs == rhs);
    }

    template <class T, class Container>
    bool operator>(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
    {
        return rhs < lhs;
    }

    template <class T, class Container>
    bool operator<=(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
    {
        return !(rhs < lhs);
    }

    template <class T, class Container>
    bool operator>=(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
    {
        return !(lhs < rhs);
    }

    // overload swap
    template <class T, class Container>
    void swap(stack<T, Container>& lhs, stack<T, Container>& rhs) 
        noexcept(noexcept(lhs.swap(rhs)))
    {
        lhs.swap(rhs);
    }

} // namespace tinystl
#endif 