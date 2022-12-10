#ifndef _MEMORY_H_
#define _MEMORY_H_

// This header file is responsible for more advanced dynamic memory management
// Contains some basic functions, space configurator, 
// uninitialized storage space management, and a template class auto_ptr

// Including these three files ensures that names declared 
// with external linkage in the C standard library headers are declared in the std namespace.
#include <cstddef>
#include <cstdlib>
#include <climits>

#include "algobase.h"
#include "allocator.h"
#include "construct.h"
#include "uninitialized.h"

namespace tinystl
{

    // get the address
    template <class Tp>
    constexpr Tp* address_of(Tp& value) noexcept
    {
        return &value;
    }

    // Acquire a temporary buffer
    // The ptrdiff_t type variable 
    // is usually used to save the result of the subtraction operation of two pointers
    template <class T>
    pair<T*, ptrdiff_t> get_buffer_helper(ptrdiff_t len, T*)
    {
        // INT_MAX: The maximum value of an int type variable 2147483647
        if (len > static_cast<ptrdiff_t>(INT_MAX / sizeof(T)))
            len = INT_MAX / sizeof(T);

        while (len > 0)
        {
            // The malloc() function will find a suitable block of free memory, 
            // which is anonymous.
            // In other words, malloc() allocates memory, but does not assign a name to it
            T* tmp = static_cast<T*>(malloc(static_cast<size_t>(len) * sizeof(T)));

            if (tmp)
                return pair<T*, ptrdiff_t>(tmp, len);

            len /= 2;  // Reduce the size of len when the request fails
        }
        return pair<T*, ptrdiff_t>(nullptr, 0);
    }
    // static_cast <type-id> ( expression ) converts expression to type type-id.
    template <class T>
    pair<T*, ptrdiff_t> get_temporary_buffer(ptrdiff_t len)
    {
        return get_buffer_helper(len, static_cast<T*>(0));
    }
    // overload
    template <class T>
    pair<T*, ptrdiff_t> get_temporary_buffer(ptrdiff_t len, T*)
    {
        return get_buffer_helper(len, static_cast<T*>(0));
    }

    // release a temporary buffer
    template <class T>
    void release_temporary_buffer(T* ptr)
    {
        // Dynamically allocate memory (malloc()), need to release memory, 
        // otherwise the memory exists after the program ends,
        // but no pointer can be accessed, resulting in memory leaks
        free(ptr);
    }

    // --------------------------------------------------------------------------------------
    // temporary_buffer
    // Apply and release temporary buffer
    template <class ForwardIterator, class T>
    class temporary_buffer
    {
    private:
        ptrdiff_t original_len;  // The size of the buffer for apply
        ptrdiff_t len;           // true size
        T*        buffer;        // pointer points the buffer

        public:
        temporary_buffer(ForwardIterator first, ForwardIterator last);

        ~temporary_buffer()
        {
            tinystl::destroy(buffer, buffer + len);
            free(buffer);
        }

    public:
        ptrdiff_t size() const noexcept 
        { return len; }

        ptrdiff_t requested_size() const noexcept 
        { return original_len; }

        T* begin() noexcept 
        { return buffer; }

        T* end() noexcept 
        { return buffer + len; }

    private:
        void allocate_buffer();

        void initialize_buffer(const T&, std::true_type) 
        {}
        void initialize_buffer(const T& value, std::false_type)
        { 
            tinystl::uninitialized_fill_n(buffer, len, value); 
        }

    private:
        temporary_buffer(const temporary_buffer&);
        void operator=(const temporary_buffer&);
    };

    // implement
    // constructor
    template <class ForwardIterator, class T>
    temporary_buffer<ForwardIterator, T>:: temporary_buffer(ForwardIterator first, 
        ForwardIterator last)
    {
        try
        {
            len = tinystl::distance(first, last);
            allocate_buffer();
            if (len > 0)
            {
                // is_trivially_default_constructible
                // Determine whether there is a default constructor in the type
                initialize_buffer(*first, std::is_trivially_default_constructible<T>());
            }
        }
        catch (...)
        {
            free(buffer);
            buffer = nullptr;
            len = 0;
        }
    }

    // allocate_buffer 
    template <class ForwardIterator, class T>
    void temporary_buffer<ForwardIterator, T>::allocate_buffer()
    {
        original_len = len;
        if (len > static_cast<ptrdiff_t>(INT_MAX / sizeof(T)))
            len = INT_MAX / sizeof(T);

        while (len > 0)
        {
            buffer = static_cast<T*>(malloc(len * sizeof(T)));
            if (buffer)
                break;

            len /= 2;  // Reduce the size of len when the request fails
        }
    }

    // --------------------------------------------------------------------------------------
    // auto_ptr: kind of like shared_ptr
    // A small smart pointer with strict object ownership
    template <class T>
    class auto_ptr
    {
    public:
        typedef T elem_type;

    private:
        T* m_ptr;  // actual pointer

    public:
        // constructor
        explicit auto_ptr(T* p = nullptr) :m_ptr(p) 
        {}

        // copy constructor
        // copy and release
        auto_ptr(auto_ptr& rhs) :m_ptr(rhs.release()) 
        {}

        template <class U>
        auto_ptr(auto_ptr<U>& rhs) : m_ptr(rhs.release()) 
        {}

        auto_ptr& operator=(auto_ptr& rhs)
        {
            if (this != &rhs)
            {
                delete m_ptr;
                m_ptr = rhs.release();
            }
            return *this;
        }

        template <class U>
        auto_ptr& operator=(auto_ptr<U>& rhs)
        {
            if (this->get() != rhs.get())
            {
            delete m_ptr;
            m_ptr = rhs.release();
            }
            return *this;
        }

        ~auto_ptr() { delete m_ptr; }

    public:
        // overload operator* 和 operator->
        T& operator*()  const 
        { return *m_ptr; }

        T* operator->() const 
        { return m_ptr; }

        // get the pointer
        T* get() const 
        { return m_ptr; }

        // release pointer
        T* release()
        {
            T* tmp = m_ptr;
            m_ptr = nullptr;
            return tmp;
        }

        // reset the pointer
        void reset(T* p = nullptr)
        {
            if (m_ptr != p)
            {
                delete m_ptr;
                m_ptr = p;
            }
        }
    };

} // namespace tinystl
#endif 

