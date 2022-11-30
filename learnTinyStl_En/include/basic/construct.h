#ifndef MYTINYSTL_CONSTRUCT_H_
#define MYTINYSTL_CONSTRUCT_H_

// construct 
// destroy   

#include <new>        
#include "type_traits.h"
#include "iterator.h"

// A typical use of pragma is to display informational messages at compile time.
#ifdef _MSC_VER
// It is to save the current compiler warning state.
// In this way, when compiling the code in the Some code section, 
// the warning 4100 will not appear.
#pragma warning(push)
#pragma warning(disable : 4100)  // unused parameter
#endif // _MSC_VER

// new in CPP: https://blog.csdn.net/u010318270/article/details/78608244

// new operator: in general, is new. A *ptr = new A
// Get a piece of memory space [operator new implementation], 
// call the constructor [placement new implementation], 
// and return the correct pointer

// operator new
// By default, the code that allocates memory is called first, trying to get a space on the heap. 
// Return if successful. 
// Call new_hander if failed, and then continue to repeat the previous process

// placement new
// Used to achieve positioning and construction. 
// After obtaining a piece of memory that can accommodate objects of the specified type, 
// construct an object on this piece of memory.
namespace mystl
{                
    // construct: use placement new
    template <class Ty>
    void construct(Ty* ptr)
    {
        ::new ((void*)ptr) Ty();
    }

    template <class Ty1, class Ty2>
    void construct(Ty1* ptr, const Ty2& value)
    {
        ::new ((void*)ptr) Ty1(value);
    }

    // class... Args: variadic templates, This parameter pack can contain 0 to any number of template parameters;
    // On the right side of a template definition, parameter packs can be expanded into individual parameters.
    template <class Ty, class... Args>
    void construct(Ty* ptr, Args&&... args)
    {
        ::new ((void*)ptr) Ty(mystl::forward<Args>(args)...);
    }

    // destroy: desconstruct
    template <class Ty>
    void destroy_one(Ty*, std::true_type) 
    {}
    
    // need to define own desconstruct function
    template <class Ty>
    void destroy_one(Ty* pointer, std::false_type)
    {
        if (pointer != nullptr)
        {
            pointer->~Ty();
        }
    }

    template <class Ty>
    void destroy(Ty* pointer)
    {
        // is_trivially_destructible: Tests whether the type is trivially destructible.
        destroy_one(pointer, std::is_trivially_destructible<Ty>{});
    }

    template <class ForwardIter>
    void destroy(ForwardIter first, ForwardIter last)
    {
        destroy_cat(first, last, std::is_trivially_destructible<
            typename iterator_traits<ForwardIter>::value_type>{});
    }

    template <class ForwardIter>
    void destroy_cat(ForwardIter , ForwardIter , std::true_type) 
    {}

    template <class ForwardIter>
    void destroy_cat(ForwardIter first, ForwardIter last, std::false_type)
    {
        for (; first != last; ++first)
            destroy(&*first);
    }

} // namespace mystl

#ifdef _MSC_VER
// It is to restore the original warning state.
#pragma warning(pop)
#endif // _MSC_VER

#endif // !MYTINYSTL_CONSTRUCT_H_

