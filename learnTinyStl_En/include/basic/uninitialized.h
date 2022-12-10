#ifndef _UNINITIALIZED_H_
#define _UNINITIALIZED_H_

// This header file is used to construct elements for uninitialized spaces; determine the iterator type, 
// select the corresponding function, or create the space yourself

#include "algobase.h"
#include "construct.h"
#include "iterator.h"
#include "type_traits.h"
#include "util.h"

namespace tinystl
{

    //---------------------------------------------------------------------------------------
    // Copy the elements in [first, last) 
    // to the space starting from result, 
    // and return the position where the copy ends

    template <class InputIter, class ForwardIter>
    ForwardIter 
    unchecked_uninit_copy(InputIter first, InputIter last, ForwardIter result, std::true_type)
    {
        return tinystl::copy(first, last, result);
    }

    template <class InputIter, class ForwardIter>
    ForwardIter unchecked_uninit_copy(InputIter first, InputIter last, 
        ForwardIter result, std::false_type)
    {
        auto cur = result;
        try
        {
            for (; first != last; ++first, ++cur)
            {
                tinystl::construct(&*cur, *first);
            }
        }
        catch (...)
        {
            for (; result != cur; --cur)
            tinystl::destroy(&*cur);
        }
        return cur;
    }

    // is_trivially_copy_assignable
    // Tests whether the type has a trivial copy assignment operator.
    // true or false
    template <class InputIter, class ForwardIter>
    ForwardIter uninitialized_copy(InputIter first, InputIter last, ForwardIter result)
    {
        // First extract the type of the iterator, 
        // then determine whether the iterator has a normal copy operator, 
        // if true call copy() in unchecked_uninit_copy()

        // If false, create the space assignment yourself
        return tinystl::unchecked_uninit_copy(first, last, result, 
            std::is_trivially_copy_assignable<
            typename iterator_traits<ForwardIter>::value_type>{});
    }

    //---------------------------------------------------------------------------------------
    // Copy the elements in [first, first+n) 
    // to the space starting from result, 
    // and return the position where the copy ends
    template <class InputIter, class Size, class ForwardIter>
    ForwardIter 
    unchecked_uninit_copy_n(InputIter first, Size n, ForwardIter result, std::true_type)
    {
        return tinystl::copy_n(first, n, result).second;
    }

    template <class InputIter, class Size, class ForwardIter>
    ForwardIter
    unchecked_uninit_copy_n(InputIter first, Size n, ForwardIter result, std::false_type)
    {
        auto cur = result;
        try
        {
            for (; n > 0; --n, ++cur, ++first)
            {
            tinystl::construct(&*cur, *first);
            }
        }
        catch (...)
        {
            for (; result != cur; --cur)
            tinystl::destroy(&*cur);
        }
        return cur;
    }

    template <class InputIter, class Size, class ForwardIter>
    ForwardIter uninitialized_copy_n(InputIter first, Size n, ForwardIter result)
    {
        return tinystl::unchecked_uninit_copy_n(first, n, result,
            std::is_trivially_copy_assignable<
            typename iterator_traits<InputIter>::value_type>{});
    }

    //---------------------------------------------------------------------------------------
    // Fill elements in the range [first, last)
    template <class ForwardIter, class T>
    void unchecked_uninit_fill(ForwardIter first, ForwardIter last, 
        const T& value, std::true_type)
    {
        tinystl::fill(first, last, value);
    }

    template <class ForwardIter, class T>
    void unchecked_uninit_fill(ForwardIter first, ForwardIter last, 
        const T& value, std::false_type)
    {
        auto cur = first;
        try
        {
            for (; cur != last; ++cur)
            {
                tinystl::construct(&*cur, value);
            }
        }
        catch (...)
        {
            for (;first != cur; ++first)
            tinystl::destroy(&*first);
        }
    }

    template <class ForwardIter, class T>
    void  uninitialized_fill(ForwardIter first, ForwardIter last, const T& value)
    {
        tinystl::unchecked_uninit_fill(first, last, value, 
            std::is_trivially_copy_assignable<
            typename iterator_traits<ForwardIter>::value_type>{});
    }

    //---------------------------------------------------------------------------------------
    // Fill elements in the range [first, first+n)
    template <class ForwardIter, class Size, class T>
    ForwardIter unchecked_uninit_fill_n(ForwardIter first, Size n, const T& value, std::true_type)
    {
        return tinystl::fill_n(first, n, value);
    }

    template <class ForwardIter, class Size, class T>
    ForwardIter unchecked_uninit_fill_n(ForwardIter first, Size n, const T& value, std::false_type)
    {
        auto cur = first;
        try
        {
            for (; n > 0; --n, ++cur)
            {
                tinystl::construct(&*cur, value);
            }
        }
        catch (...)
        {
            for (; first != cur; ++first)
                tinystl::destroy(&*first);
        }
        return cur;
    }

    template <class ForwardIter, class Size, class T>
    ForwardIter uninitialized_fill_n(ForwardIter first, Size n, const T& value)
    {
        return tinystl::unchecked_uninit_fill_n(first, n, value, 
            std::is_trivially_copy_assignable<
            typename iterator_traits<ForwardIter>::value_type>{});
    }

    //---------------------------------------------------------------------------------------
    // Move the elemetns in [first, last) to the space starting with result, 
    // and return the position where the movement ends
    template <class InputIter, class ForwardIter>
    ForwardIter unchecked_uninit_move(InputIter first, InputIter last, 
        ForwardIter result, std::true_type)
    {
        return tinystl::move(first, last, result);
    }

    template <class InputIter, class ForwardIter>
    ForwardIter unchecked_uninit_move(InputIter first, InputIter last, 
        ForwardIter result, std::false_type)
    {
        ForwardIter cur = result;
        try
        {
            for (; first != last; ++first, ++cur)
            {
                tinystl::construct(&*cur, tinystl::move(*first));
            }
        }
        catch (...)
        {
            tinystl::destroy(result, cur);
        }
        return cur;
    }
    // is_tricially_move_assignable
    // Tests whether the type has a trivial move assignment operator.
    // the process is the same as before
    template <class InputIter, class ForwardIter>
    ForwardIter uninitialized_move(InputIter first, InputIter last, ForwardIter result)
    {
        return tinystl::unchecked_uninit_move(first, last, result,
            std::is_trivially_move_assignable<
            typename iterator_traits<InputIter>::value_type>{});
    }

    //---------------------------------------------------------------------------------------
    // Move the elements in [first, first+n) to the space starting with result,
    // and return the position where the movement ends
    template <class InputIter, class Size, class ForwardIter>
    ForwardIter unchecked_uninit_move_n(InputIter first, Size n, 
        ForwardIter result, std::true_type)
    {
        return tinystl::move(first, first + n, result);
    }

    template <class InputIter, class Size, class ForwardIter>
    ForwardIter unchecked_uninit_move_n(InputIter first, Size n, 
        ForwardIter result, std::false_type)
    {
        auto cur = result;
        try
        {
            for (; n > 0; --n, ++first, ++cur)
            {
                tinystl::construct(&*cur, tinystl::move(*first));
            }
        }
        catch (...)
        {
            for (; result != cur; ++result)
                tinystl::destroy(&*result);
            throw;
        }
        return cur;
    }

    template <class InputIter, class Size, class ForwardIter>
    ForwardIter uninitialized_move_n(InputIter first, Size n, ForwardIter result)
    {
        return tinystl::unchecked_uninit_move_n(first, n, result,
            std::is_trivially_move_assignable<
            typename iterator_traits<InputIter>::value_type>{});
    }

} // namespace tinystl
#endif
