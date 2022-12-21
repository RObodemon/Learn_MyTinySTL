#ifndef _VECTOR_H_
#define _VECTOR_H_

// vector

// notes:
// Exception guarantees:
// tinystl::vecotr<T> Satisfy the basic exception guarantee, some functions have no exception guarantee, 
// and strengthen the exception safety guarantee for the following functions:
//   * emplace
//   * emplace_back
//   * push_back

// when std::is_nothrow_move_assignable<T>::value == true，
// The following functions also satisfy the strong exception guarantee:
//   * reserve
//   * resize
//   * insert

// Defines the container class template initializer_list and several supporting templates.
#include <initializer_list>

#include "iterator.h"
#include "memory.h"
#include "util.h"
#include "exceptdef.h"
#include "algo.h"
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

    // vector template
    template <class T>
    class vector
    {
        // static_assert:
        // If the specified constant expression is false, 
        // the compiler displays the specified message, if one is provided, 
        // and the compilation fails with error C2338; 
        // otherwise, the declaration has no effect.
        static_assert(!std::is_same<bool, T>::value, "vector<bool> is abandoned in tinystl");
    public:
        // vector's nested type definitions
        typedef tinystl::allocator<T>   allocator_type;
        typedef tinystl::allocator<T>   data_allocator;

        typedef typename allocator_type::value_type         value_type;
        typedef typename allocator_type::pointer            pointer;
        typedef typename allocator_type::const_pointer      const_pointer;
        typedef typename allocator_type::reference          reference;
        typedef typename allocator_type::const_reference    const_reference;
        typedef typename allocator_type::size_type          size_type;
        typedef typename allocator_type::difference_type    difference_type;

        typedef value_type*                                 iterator;
        typedef const value_type*                           const_iterator;
        typedef tinystl::reverse_iterator<iterator>         reverse_iterator;
        typedef tinystl::reverse_iterator<const_iterator>   const_reverse_iterator;

        allocator_type get_allocator() { return data_allocator(); }

    private:
        iterator begin_;  // Indicates the head of the currently used space
        iterator end_;    // Indicates the end of the currently used space
        iterator cap_;    // Indicates the end of the current storage space

    public:
        // constructor----------------------------------------------------------------------
        vector() noexcept
        { try_init(); }

        explicit vector(size_type n)
        { fill_init(n, value_type()); }

        vector(size_type n, const value_type& value)
        { fill_init(n, value); }

        template <class Iter, typename std::enable_if<
            tinystl::is_input_iterator<Iter>::value, int>::type = 0>
        vector(Iter first, Iter last)
        {
            TINYSTL_DEBUG(!(last < first));
            range_init(first, last);
        }
        // copy construct
        vector(const vector& rhs)
        {
            range_init(rhs.begin_, rhs.end_);
        }
        // Move Constructor
        vector(vector&& rhs) noexcept
            :begin_(rhs.begin_), end_(rhs.end_), cap_(rhs.cap_)
        {
            rhs.begin_ = nullptr;
            rhs.end_   = nullptr;
            rhs.cap_   = nullptr;
        }

        // Specify the type of each member in the ilist array
        vector(std::initializer_list<value_type> ilist)
        {
            range_init(ilist.begin(), ilist.end());
        }

        vector& operator=(const vector& rhs);
        // Move Assignment Operators
        vector& operator=(vector&& rhs) noexcept;

        vector& operator=(std::initializer_list<value_type> ilist)
        {
            vector tmp(ilist.begin(), ilist.end());
            swap(tmp);
            return *this;
        }
        // deconstructor
        ~vector()
        { 
            destroy_and_recover(begin_, end_, cap_ - begin_);
            begin_ = end_ = cap_ = nullptr;
        }

    public:
        // operation on iterator------------------------------------------------------------
        iterator begin() noexcept 
        { return begin_; }

        const_iterator begin() const noexcept
        { return begin_; }

        iterator end() noexcept
        { return end_; }

        const_iterator end() const noexcept 
        { return end_; }

        // reverse
        reverse_iterator rbegin() noexcept 
        { return reverse_iterator(end()); }
        
        const_reverse_iterator rbegin()  const noexcept
        { return const_reverse_iterator(end()); }

        reverse_iterator rend() noexcept
        { return reverse_iterator(begin()); }

        const_reverse_iterator rend() const noexcept 
        { return const_reverse_iterator(begin()); }

        // const
        const_iterator cbegin() const noexcept 
        { return begin(); }

        const_iterator cend() const noexcept
        { return end(); }

        const_reverse_iterator crbegin() const noexcept
        { return rbegin(); }

        const_reverse_iterator crend() const noexcept
        { return rend(); }

        // parameters-----------------------------------------------------------------------
        bool empty() const noexcept
        { return begin_ == end_; }

        size_type size() const noexcept
        { return static_cast<size_type>(end_ - begin_); }

        size_type max_size() const noexcept
        { return static_cast<size_type>(-1) / sizeof(T); }

        size_type capacity() const noexcept
        { return static_cast<size_type>(cap_ - begin_); }

        void reserve(size_type n);
        void shrink_to_fit();

        // Access element operations
        reference operator[](size_type n)
        {
            TINYSTL_DEBUG(n < size());
            return *(begin_ + n);
        }
        
        const_reference operator[](size_type n) const
        {
            TINYSTL_DEBUG(n < size());
            return *(begin_ + n);
        }

        reference at(size_type n)
        {
            THROW_OUT_OF_RANGE_IF(!(n < size()), "vector<T>::at() subscript out of range");
            return (*this)[n];
        }

        const_reference at(size_type n) const
        {
            THROW_OUT_OF_RANGE_IF(!(n < size()), "vector<T>::at() subscript out of range");
            return (*this)[n];
        }

        reference front()
        {
            TINYSTL_DEBUG(!empty());
            return *begin_;
        }

        const_reference front() const
        {
            TINYSTL_DEBUG(!empty());
            return *begin_;
        }

        reference back()
        {
            TINYSTL_DEBUG(!empty());
            return *(end_ - 1);
        }

        const_reference back() const
        {
            TINYSTL_DEBUG(!empty());
            return *(end_ - 1);
        }

        pointer data() noexcept 
        { return begin_; }

        const_pointer data() const noexcept 
        { return begin_; }

        // Modify container-----------------------------------------------------------------
        // assign
        void assign(size_type n, const value_type& value)
        { fill_assign(n, value); }

        template <class Iter, typename std::enable_if<
            tinystl::is_input_iterator<Iter>::value, int>::type = 0>
        void assign(Iter first, Iter last)
        {
            TINYSTL_DEBUG(!(last < first));
            copy_assign(first, last, iterator_category(first));
        }

        void assign(std::initializer_list<value_type> il)
        { copy_assign(il.begin(), il.end(), tinystl::forward_iterator_tag{}); }

        // emplace / emplace_back
        template <class... Args>
        iterator emplace(const_iterator pos, Args&& ...args);

        template <class... Args>
        void emplace_back(Args&& ...args);

        // push_back / pop_back
        void push_back(const value_type& value);

        void push_back(value_type&& value)
        { emplace_back(tinystl::move(value)); }

        void pop_back();

        // insert
        iterator insert(const_iterator pos, const value_type& value);

        iterator insert(const_iterator pos, value_type&& value)
        { return emplace(pos, tinystl::move(value)); }

        iterator insert(const_iterator pos, size_type n, const value_type& value)
        {
            TINYSTL_DEBUG(pos >= begin() && pos <= end());
            return fill_insert(const_cast<iterator>(pos), n, value);
        }

        template <class Iter, typename std::enable_if<
            tinystl::is_input_iterator<Iter>::value, int>::type = 0>
        void insert(const_iterator pos, Iter first, Iter last)
        {
            TINYSTL_DEBUG(pos >= begin() && pos <= end() && !(last < first));
            copy_insert(const_cast<iterator>(pos), first, last);
        }

        // erase / clear
        iterator erase(const_iterator pos);

        iterator erase(const_iterator first, const_iterator last);

        void clear() { erase(begin(), end()); }

        // resize / reverse
        void resize(size_type new_size) 
        { return resize(new_size, value_type()); }

        void resize(size_type new_size, const value_type& value);

        void reverse() 
        { tinystl::reverse(begin(), end()); }

        // swap
        void swap(vector& rhs) noexcept;

    private:
        // helper functions
        // construct / copy / move / deconstruct / ----------------------------------------
        void try_init() noexcept;

        void init_space(size_type size, size_type cap);

        void fill_init(size_type n, const value_type& value);

        template <class Iter>
        void range_init(Iter first, Iter last);

        void destroy_and_recover(iterator first, iterator last, size_type n);

        // helper functions Modify container------------------------------------------------
        // calculate the growth size
        size_type get_new_cap(size_type add_size);

        // assign
        void fill_assign(size_type n, const value_type& value);

        template <class IIter>
        void copy_assign(IIter first, IIter last, input_iterator_tag);

        template <class FIter>
        void copy_assign(FIter first, FIter last, forward_iterator_tag);

        // reallocate
        template <class... Args>
        void reallocate_emplace(iterator pos, Args&& ...args);

        void reallocate_insert(iterator pos, const value_type& value);

        // insert
        iterator fill_insert(iterator pos, size_type n, const value_type& value);

        template <class IIter>
        void copy_insert(iterator pos, IIter first, IIter last);

        // shrink_to_fit
        void reinsert(size_type size);
    };

    //==================implement=========================================================//
    
    // construct / copy / move / deconstruct / --------------------------------------------- 
    // try_init
    // Ignored if allocation fails, no exception is thrown.
    template <class T>
    void vector<T>::try_init() noexcept
    {
        try
        {
            begin_ = data_allocator::allocate(16);
            // at beginning, so end and begin are pointing the same place
            end_   = begin_;
            cap_   = begin_ + 16;
        }
        catch (...)
        {
            begin_ = nullptr;
            end_   = nullptr;
            cap_   = nullptr;
        }
    }

    // init_space
    template <class T>
    void vector<T>::init_space(size_type size, size_type cap)
    {
        try
        {
            begin_ = data_allocator::allocate(cap);
            end_   = begin_ + size;
            cap_   = begin_ + cap;
        }
        catch (...)
        {
            begin_ = nullptr;
            end_   = nullptr;
            cap_   = nullptr;
            throw;
        }
    }

    // fill_init
    template <class T>
    void vector<T>::
    fill_init(size_type n, const value_type& value)
    {
        const size_type init_size = tinystl::max(static_cast<size_type>(16), n);
        init_space(n, init_size);
        tinystl::uninitialized_fill_n(begin_, n, value);
    }

    // range_init
    template <class T>
    template <class Iter>
    void vector<T>::range_init(Iter first, Iter last)
    {
        const size_type init_size = tinystl::max(static_cast<size_type>(last - first),
                                                 static_cast<size_type>(16));

        init_space(static_cast<size_type>(last - first), init_size);
        tinystl::uninitialized_copy(first, last, begin_);
    }

    // destroy_and_recover 
    template <class T>
    void vector<T>::destroy_and_recover(iterator first, iterator last, size_type n)
    {
        data_allocator::destroy(first, last);
        data_allocator::deallocate(first, n);
    }

    // exchange with another vector
    template <class T>
    void vector<T>::swap(vector<T>& rhs) noexcept
    {
        if (this != &rhs)
        {
            tinystl::swap(begin_, rhs.begin_);
            tinystl::swap(end_, rhs.end_);
            tinystl::swap(cap_, rhs.cap_);
        }
    }

    // copy assignment operator
    template <class T>
    vector<T>& vector<T>::operator=(const vector& rhs)
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
                auto i = tinystl::copy(rhs.begin(), rhs.end(), begin());
                data_allocator::destroy(i, end_);
                end_ = begin_ + len;
            }
            else
            { 
                tinystl::copy(rhs.begin(), rhs.begin() + size(), begin_);
                tinystl::uninitialized_copy(rhs.begin() + size(), rhs.end(), end_);
                cap_ = end_ = begin_ + len;
            }
        }
        return *this;
    }

    // move assignment operator
    template <class T>
    vector<T>& vector<T>::operator=(vector&& rhs) noexcept
    {
        destroy_and_recover(begin_, end_, cap_ - begin_);
        begin_ = rhs.begin_;
        end_ = rhs.end_;
        cap_ = rhs.cap_;
        rhs.begin_ = nullptr;
        rhs.end_ = nullptr;
        rhs.cap_ = nullptr;
        return *this;
    }

    // when the original capacity is less than the required size, it will be reallocated
    template <class T>
    void vector<T>::reserve(size_type n)
    {
        if (capacity() < n)
        {
            THROW_LENGTH_ERROR_IF(n > max_size(),
                "n can not larger than max_size() in vector<T>::reserve(n)");

            const auto old_size = size();
            auto tmp = data_allocator::allocate(n);
            tinystl::uninitialized_move(begin_, end_, tmp);
            data_allocator::deallocate(begin_, cap_ - begin_);
            begin_ = tmp;
            end_ = tmp + old_size;
            cap_ = begin_ + n;
        }
    }

    // give up excess capacity
    template <class T>
    void vector<T>::shrink_to_fit()
    {
        if (end_ < cap_)
            reinsert(size());
    }

    // modify container---------------------------------------------------------------------
    // fill_assign 
    template <class T>
    void vector<T>::fill_assign(size_type n, const value_type& value)
    {
        if (n > capacity())
        {
            vector tmp(n, value);
            swap(tmp);
        }
        else if (n > size())
        {
            tinystl::fill(begin(), end(), value);
            end_ = tinystl::uninitialized_fill_n(end_, n - size(), value);
        }
        else
            erase(tinystl::fill_n(begin_, n, value), end_);// delete left part
    }

    // copy_assign 
    template <class T>
    template <class IIter>
    void vector<T>::copy_assign(IIter first, IIter last, input_iterator_tag)
    {
        auto cur = begin_;
        for (; first != last && cur != end_; ++first, ++cur)
        {
            *cur = *first;
        }

        if (first == last)
            erase(cur, end_);
        else
            insert(end_, first, last);
    }

    // assigenment by [first, last) 
    template <class T>
    template <class FIter>
    void vector<T>::copy_assign(FIter first, FIter last, forward_iterator_tag)
    {
        const size_type len = tinystl::distance(first, last);
        if (len > capacity())
        {
            vector tmp(first, last);
            swap(tmp);
        }
        else if (size() >= len)
        {
            auto new_end = tinystl::copy(first, last, begin_);
            data_allocator::destroy(new_end, end_);
            end_ = new_end;
        }
        else
        {
            auto mid = first;
            tinystl::advance(mid, size());
            tinystl::copy(first, mid, begin_);
            auto new_end = tinystl::uninitialized_copy(mid, last, end_);
            end_ = new_end;
        }
    }

    // get_new_cap 
    template <class T>
    typename vector<T>::size_type 
    vector<T>::get_new_cap(size_type add_size)
    {
        const auto old_size = capacity();
        THROW_LENGTH_ERROR_IF(old_size > max_size() - add_size,
                                "vector<T>'s size too big");
        if (old_size > max_size() - old_size / 2)
        {
            return old_size + add_size > max_size() - 16
            ? old_size + add_size 
            : old_size + add_size + 16;
        }

        const size_type new_size = old_size == 0
            ? tinystl::max(add_size, static_cast<size_type>(16))
            : tinystl::max(old_size + old_size / 2, old_size + add_size);
        return new_size;
    }

    // Reallocates space and constructs element in-place at pos
    template <class T>
    template <class ...Args>
    void vector<T>::reallocate_emplace(iterator pos, Args&& ...args)
    {
        const auto new_size = get_new_cap(1);
        auto new_begin = data_allocator::allocate(new_size);
        auto new_end = new_begin;
        try
        {
            new_end = tinystl::uninitialized_move(begin_, pos, new_begin);
            data_allocator::construct(tinystl::address_of(*new_end), tinystl::forward<Args>(args)...);
            ++new_end;
            new_end = tinystl::uninitialized_move(pos, end_, new_end);
        }
        catch (...)
        {
            data_allocator::deallocate(new_begin, new_size);
            throw;
        }
        destroy_and_recover(begin_, end_, cap_ - begin_);
        begin_ = new_begin;
        end_ = new_end;
        cap_ = new_begin + new_size;
    }

    // Constructs the element in-place at position pos, 
    // avoiding additional copy or move overhead
    template <class T>
    template <class ...Args>
    typename vector<T>::iterator
    vector<T>::emplace(const_iterator pos, Args&& ...args)
    {
        TINYSTL_DEBUG(pos >= begin() && pos <= end());
        iterator xpos = const_cast<iterator>(pos);
        const size_type n = xpos - begin_;
        if (end_ != cap_ && xpos == end_)
        {
            data_allocator::construct(tinystl::address_of(*end_), tinystl::forward<Args>(args)...);
            ++end_;
        }
        else if (end_ != cap_)
        {
            auto new_end = end_;
            data_allocator::construct(tinystl::address_of(*end_), *(end_ - 1));
            ++new_end;
            tinystl::copy_backward(xpos, end_ - 1, end_);
            *xpos = value_type(tinystl::forward<Args>(args)...);
            end_ = new_end;
        }
        else
            reallocate_emplace(xpos, tinystl::forward<Args>(args)...);

        return begin() + n;
    }

    // Constructs elements in-place at the end, 
    // avoiding additional copy or move overhead
    template <class T>
    template <class ...Args>
    void vector<T>::emplace_back(Args&& ...args)
    {
        if (end_ < cap_)
        {
            data_allocator::construct(tinystl::address_of(*end_), tinystl::forward<Args>(args)...);
            ++end_;
        }
        else
            reallocate_emplace(end_, tinystl::forward<Args>(args)...);
    }

    // insert element at the end
    template <class T>
    void vector<T>::push_back(const value_type& value)
    {
        if (end_ != cap_)
        {
            data_allocator::construct(tinystl::address_of(*end_), value);
            ++end_;
        }
        else
            reallocate_insert(end_, value);
    }

    // pop end element
    template <class T>
    void vector<T>::pop_back()
    {
        TINYSTL_DEBUG(!empty());
        data_allocator::destroy(end_ - 1);
        --end_;
    }

    // delete element at pos
    template <class T>
    typename vector<T>::iterator
    vector<T>::erase(const_iterator pos)
    {
        TINYSTL_DEBUG(pos >= begin() && pos < end());
        iterator xpos = begin_ + (pos - begin());
        tinystl::move(xpos + 1, end_, xpos);
        data_allocator::destroy(end_ - 1);
        --end_;
        return xpos;
    }

    // delete elements in [first, last)
    template <class T>
    typename vector<T>::iterator
    vector<T>::erase(const_iterator first, const_iterator last)
    {
        TINYSTL_DEBUG(first >= begin() && last <= end() && !(last < first));
        const auto n = first - begin();
        iterator r = begin_ + (first - begin());
        data_allocator::destroy(tinystl::move(r + (last - first), end_, r), end_);
        end_ = end_ - (last - first);
        return begin_ + n;
    }

    // resize
    template <class T>
    void vector<T>::resize(size_type new_size, const value_type& value)
    {
        if (new_size < size())
            erase(begin() + new_size, end());
        else
            insert(end(), new_size - size(), value);
    }


    // insert element at pos
    template <class T>
    typename vector<T>::iterator
    vector<T>::insert(const_iterator pos, const value_type& value)
    {
        TINYSTL_DEBUG(pos >= begin() && pos <= end());
        iterator xpos = const_cast<iterator>(pos);
        const size_type n = pos - begin_;
        if (end_ != cap_ && xpos == end_)
        {
            data_allocator::construct(tinystl::address_of(*end_), value);
            ++end_;
        }
        else if (end_ != cap_)
        {
            auto new_end = end_;
            data_allocator::construct(tinystl::address_of(*end_), *(end_ - 1));
            ++new_end;
            // Prevent elements from being changed by the following copy operations
            auto value_copy = value;  
            tinystl::copy_backward(xpos, end_ - 1, end_);
            *xpos = tinystl::move(value_copy);
            end_ = new_end;
        }
        else
            reallocate_insert(xpos, value);
        return begin_ + n;
    }

    // reallocate space and insert element at pos
    template <class T>
    void vector<T>::reallocate_insert(iterator pos, const value_type& value)
    {
        const auto new_size = get_new_cap(1);
        auto new_begin = data_allocator::allocate(new_size);
        auto new_end = new_begin;
        const value_type& value_copy = value;
        try
        {
            new_end = tinystl::uninitialized_move(begin_, pos, new_begin);
            data_allocator::construct(tinystl::address_of(*new_end), value_copy);
            ++new_end;
            new_end = tinystl::uninitialized_move(pos, end_, new_end);
        }
        catch (...)
        {
            data_allocator::deallocate(new_begin, new_size);
            throw;
        }
        destroy_and_recover(begin_, end_, cap_ - begin_);
        begin_ = new_begin;
        end_ = new_end;
        cap_ = new_begin + new_size;
    }

    // fill_insert 
    template <class T>
    typename vector<T>::iterator 
    vector<T>::fill_insert(iterator pos, size_type n, const value_type& value)
    {
        if (n == 0)
            return pos;

        const size_type xpos = pos - begin_;
        const value_type value_copy = value;  // avoid being covered
        if (static_cast<size_type>(cap_ - end_) >= n)
        { 
            // If the spare space is greater than or equal to the added space
            const size_type after_elems = end_ - pos;
            auto old_end = end_;
            if (after_elems > n)
            {
                tinystl::uninitialized_copy(end_ - n, end_, end_);
                end_ += n;
                tinystl::move_backward(pos, old_end - n, old_end);
                tinystl::uninitialized_fill_n(pos, n, value_copy);
            }
            else
            {
                end_ = tinystl::uninitialized_fill_n(end_, n - after_elems, value_copy);
                end_ = tinystl::uninitialized_move(pos, old_end, end_);
                tinystl::uninitialized_fill_n(pos, after_elems, value_copy);
            }
        }
        else
        { 
            // Insufficient spare space
            const auto new_size = get_new_cap(n);
            auto new_begin = data_allocator::allocate(new_size);
            auto new_end = new_begin;
            try
            {
                new_end = tinystl::uninitialized_move(begin_, pos, new_begin);
                new_end = tinystl::uninitialized_fill_n(new_end, n, value);
                new_end = tinystl::uninitialized_move(pos, end_, new_end);
            }
            catch (...)
            {
                destroy_and_recover(new_begin, new_end, new_size);
                throw;
            }
            data_allocator::deallocate(begin_, cap_ - begin_);
            begin_ = new_begin;
            end_ = new_end;
            cap_ = begin_ + new_size;
        }
        return begin_ + xpos;
    }

    // copy_insert 
    template <class T>
    template <class IIter>
    void vector<T>::
    copy_insert(iterator pos, IIter first, IIter last)
    {
        if (first == last)
            return;
        const auto n = tinystl::distance(first, last);
        if ((cap_ - end_) >= n)
        {
            // If the spare space size is sufficient
            const auto after_elems = end_ - pos;
            auto old_end = end_;
            if (after_elems > n)
            {
                end_ = tinystl::uninitialized_copy(end_ - n, end_, end_);
                tinystl::move_backward(pos, old_end - n, old_end);
                tinystl::uninitialized_copy(first, last, pos);
            }
            else
            {
                auto mid = first;
                tinystl::advance(mid, after_elems);
                end_ = tinystl::uninitialized_copy(mid, last, end_);
                end_ = tinystl::uninitialized_move(pos, old_end, end_);
                tinystl::uninitialized_copy(first, mid, pos);
            }
        }
        else
        { 
            // Insufficient spare space
            const auto new_size = get_new_cap(n);
            auto new_begin = data_allocator::allocate(new_size);
            auto new_end = new_begin;
            try
            {
                new_end = tinystl::uninitialized_move(begin_, pos, new_begin);
                new_end = tinystl::uninitialized_copy(first, last, new_end);
                new_end = tinystl::uninitialized_move(pos, end_, new_end);
            }
            catch (...)
            {
                destroy_and_recover(new_begin, new_end, new_size);
                throw;
            }
            data_allocator::deallocate(begin_, cap_ - begin_);
            begin_ = new_begin;
            end_ = new_end;
            cap_ = begin_ + new_size;
        }
    }

    // reinsert 
    template <class T>
    void vector<T>::reinsert(size_type size)
    {
        auto new_begin = data_allocator::allocate(size);
        try
        {
            tinystl::uninitialized_move(begin_, end_, new_begin);
        }
        catch (...)
        {
            data_allocator::deallocate(new_begin, size);
            throw;
        }
        data_allocator::deallocate(begin_, cap_ - begin_);
        begin_ = new_begin;
        end_ = begin_ + size;
        cap_ = begin_ + size;
    }

    // overload swap
    template <class T>
    void swap(vector<T>& lhs, vector<T>& rhs)
    {
        lhs.swap(rhs);
    }

    // overload comparison operator
    template <class T>
    bool operator==(const vector<T>& lhs, const vector<T>& rhs)
    {
        return lhs.size() == rhs.size() &&
            tinystl::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template <class T>
    bool operator<(const vector<T>& lhs, const vector<T>& rhs)
    {
        return tinystl::lexicographical_compare(lhs.begin(), lhs.end(), 
            rhs.begin(), lhs.end());
    }

    template <class T>
    bool operator!=(const vector<T>& lhs, const vector<T>& rhs)
    {
        return !(lhs == rhs);
    }

    template <class T>
    bool operator>(const vector<T>& lhs, const vector<T>& rhs)
    {
        return rhs < lhs;
    }

    template <class T>
    bool operator<=(const vector<T>& lhs, const vector<T>& rhs)
    {
        return !(rhs < lhs);
    }

    template <class T>
    bool operator>=(const vector<T>& lhs, const vector<T>& rhs)
    {
        return !(lhs < rhs);
    }
} // namespace tinystl
#endif