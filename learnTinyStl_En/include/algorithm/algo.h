#ifndef ALGO_H_
#define ALGO_H_

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)
#endif

// algorithms

#include <cstddef>
#include <ctime>

#include "algobase.h"
#include "memory.h"
#include "heap_algo.h"
#include "functional.h"

namespace tinystl
{

    //-------------------------------------------------------------------------------------
    // all_of
    // Check whether all elements in [first, last) satisfy the condition 
    // that the unary operation unary_pred is true, 
    // and return true if satisfied
    template <class InputIter, class UnaryPredicate>
    bool all_of(InputIter first, InputIter last, UnaryPredicate unary_pred)
    {
        for (; first != last; ++first)
        {
            if (!unary_pred(*first))
                return false;
        }
        return true;
    }

    //-------------------------------------------------------------------------------------
    // any_of
    // Check whether there is an element in [first, last) that satisfies the condition 
    // that the unary operation unary_pred is true, 
    // and returns true if it is satisfied
    template <class InputIter, class UnaryPredicate>
    bool any_of(InputIter first, InputIter last, UnaryPredicate unary_pred)
    {
        for (; first != last; ++first)
        {
            if (unary_pred(*first))
                return true;
        }
        return false;
    }

    //-------------------------------------------------------------------------------------
    // none_of
    // Check whether all elements in [first, last) do not satisfy the condition 
    // that the unary operation unary_pred is true, 
    // and return true if satisfied
    template <class InputIter, class UnaryPredicate>
    bool none_of(InputIter first, InputIter last, UnaryPredicate unary_pred)
    {
        for (; first != last; ++first)
        {
            if (unary_pred(*first))
                return false;
        }
        return true;
    }

    //===================================================================================
    
    //-------------------------------------------------------------------------------------
    // count
    // Compare the elements in [first, last) with the given value, 
    // use operator== by default, 
    // and return the number of equal elements
    template <class InputIter, class T>
    size_t count(InputIter first, InputIter last, const T& value)
    {
        size_t n = 0;
        for (; first != last; ++first)
        {
            if (*first == value)
                ++n;
        }

        return n;
    }

    //-------------------------------------------------------------------------------------
    // count_if
    // Perform a unary unary_pred operation on each element in [first, last), 
    // and return the number of true results
    template <class InputIter, class UnaryPredicate>
    size_t count_if(InputIter first, InputIter last, UnaryPredicate unary_pred)
    {
        size_t n = 0;
        for (; first != last; ++first)
        {
            if (unary_pred(*first))
                ++n;
        }
        return n;
    }

    //-------------------------------------------------------------------------------------
    // find
    // Find the first element equal to value in [first, last), 
    // and return an iterator pointing to the element
    template <class InputIter, class T>
    InputIter find(InputIter first, InputIter last, const T& value)
    {
        while (first != last && *first != value)
            ++first;
        return first;
    }

    //-------------------------------------------------------------------------------------
    // find_if (unary functor version)
    template <class InputIter, class UnaryPredicate>
    InputIter
    find_if(InputIter first, InputIter last, UnaryPredicate unary_pred)
    {
        while (first != last && !unary_pred(*first))
            ++first;

        return first;
    }

    //-------------------------------------------------------------------------------------
    // find_if_not (unary functor version)
    template <class InputIter, class UnaryPredicate>
    InputIter find_if_not(InputIter first, InputIter last, UnaryPredicate unary_pred)
    {
        while (first != last && unary_pred(*first))
            ++first;

        return first;
    }

    //=====================================================================================

    //-------------------------------------------------------------------------------------
    // search
    // Find the first occurrence of [first2, last2) in [first1, last1)
    template <class ForwardIter1, class ForwardIter2>
    ForwardIter1 search(ForwardIter1 first1, ForwardIter1 last1,
        ForwardIter2 first2, ForwardIter2 last2)
    {
        auto d1 = tinystl::distance(first1, last1);
        auto d2 = tinystl::distance(first2, last2);

        // [first1, last1) cannot contain [first2, last2)
        if (d1 < d2)
            return last1;

        auto current1 = first1;
        auto current2 = first2;
        while (current2 != last2)
        {
            if (*current1 == *current2)
            {
                ++current1;
                ++current2;
            }
            else
            {
                if (d1 == d2)
                    return last1;
                else
                {
                    current1 = ++first1;
                    current2 = first2;
                    --d1;
                }
            }
        }
        return first1;
    }

    // function object comp
    template <class ForwardIter1, class ForwardIter2, class Compared>
    ForwardIter1 search(ForwardIter1 first1, ForwardIter1 last1,
        ForwardIter2 first2, ForwardIter2 last2, Compared comp)
    {
        auto d1 = tinystl::distance(first1, last1);
        auto d2 = tinystl::distance(first2, last2);
        if (d1 < d2)
            return last1;

        auto current1 = first1;
        auto current2 = first2;
        while (current2 != last2)
        {
            if (comp(*current1, *current2))
            {
                ++current1;
                ++current2;
            }
            else
            {
                if (d1 == d2)
                    return last1;
                else
                {
                    current1 = ++first1;
                    current2 = first2;
                    --d1;
                }
            }
        }
        return first1;
    }

    //-------------------------------------------------------------------------------------
    // search_n
    // Find the subsequence formed by n consecutive values in [first, last), 
    // and return an iterator pointing to the beginning of the subsequence
    template <class ForwardIter, class Size, class T>
    ForwardIter
    search_n(ForwardIter first, ForwardIter last, Size n, const T& value)
    {
        if (n <= 0)
            return first;
        else
        {
            // find the first element equal to value
            first = tinystl::find(first, last, value);
            while (first != last)
            {
                auto m = n - 1; // we must keep n as a constant, because we might need to determine new sequence    
				auto i = first;
				++i;
				while (i != last && m != 0 && *i == value)
				{
					++i;
					--m;
				}
				if (m == 0)
					return first;
				else
					first = mystl::find(i, last, value);
            }
            return last;
        }
    }

    // function object comp
    template <class ForwardIter, class Size, class T, class Compared>
    ForwardIter search_n(ForwardIter first, ForwardIter last,
        Size n, const T& value, Compared comp)
    {
        if (n <= 0)
            return first;
        else
        {
            // same as find()
            while (first != last)
            {
                if (comp(*first, value))
                    break;
                ++first;
            }
            while (first != last)
            {
                auto m = n - 1;
                auto i = first;
                ++i;
                while (i != last && m != 0 && comp(*i, value))
                {
                    ++i;
                    --m;
                }

                if (m == 0)
                    return first;
                else
                {
                    // find()
                    while (i != last)
                    {
                        if (comp(*i, value))
                            break;
                        ++i;
                    }
                    first = i;
                }
            }
            return last;
        }
    }
}
