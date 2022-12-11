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
	
	//-------------------------------------------------------------------------------------
    // find_end
    // Find the last occurrence of [first2, last2) in the interval [first1, last1), 
    // if it does not exist, return last1

    // find_end_dispatch's forward_iterator_tag version
    template <class ForwardIter1, class ForwardIter2>
    ForwardIter1 find_end_dispatch(ForwardIter1 first1, ForwardIter1 last1,
        ForwardIter2 first2, ForwardIter2 last2,
        forward_iterator_tag, forward_iterator_tag)
    {
        if (first2 == last2)
            return last1;
        else
        {
            auto result = last1;
            while (true)
            {
                // search the first occurence
                auto new_result = tinystl::search(first1, last1, first2, last2);
                if (new_result == last1)
                    return result;
                else
                {
                    result = new_result;
                    // update first1, used to find next [first2, last2)
                    first1 = new_result;
                    ++first1;
                }
            }
        }
    }

    // find_end_dispatch's bidirectional_iterator_tag version
    template <class BidirectionalIter1, class BidirectionalIter2>
    BidirectionalIter1 find_end_dispatch(BidirectionalIter1 first1, BidirectionalIter1 last1,
        BidirectionalIter2 first2, BidirectionalIter2 last2,
        bidirectional_iterator_tag, bidirectional_iterator_tag)
    {
        typedef reverse_iterator<BidirectionalIter1> revIter1;
        typedef reverse_iterator<BidirectionalIter2> revIter2;
        revIter1 rlast1(first1);
        revIter2 rlast2(first2);
        // search in reverse
        revIter1 rResult = tinystl::search(revIter1(last1), rlast1, revIter2(last2), rlast2);

        if (rResult == rlast1)
            return last1;
        else
        {
            auto result = rResult.base(); // return current
            // return the true position in [first1, last1)
            tinystl::advance(result, -tinystl::distance(first2, last2));
            return result;
        }
    }

    template <class ForwardIter1, class ForwardIter2>
    ForwardIter1 find_end(ForwardIter1 first1, ForwardIter1 last1,
        ForwardIter2 first2, ForwardIter2 last2)
    {
        // extract the iterator type
        typedef typename iterator_traits<ForwardIter1>::iterator_category Category1;
        typedef typename iterator_traits<ForwardIter2>::iterator_category Category2;

        return tinystl::find_end_dispatch(first1, last1, first2, last2,
            Category1(), Category2());
    }

    // function object comp
    // find_end_dispatch's forward_iterator_tag version
    template <class ForwardIter1, class ForwardIter2, class Compared>
    ForwardIter1 find_end_dispatch(ForwardIter1 first1, ForwardIter1 last1,
        ForwardIter2 first2, ForwardIter2 last2,
        forward_iterator_tag, forward_iterator_tag, Compared comp)
    {
        if (first2 == last2)
            return last1;
        else
        {
            auto result = last1;
            while (true)
            {
                auto new_result = tinystl::search(first1, last1, first2, last2, comp);
                if (new_result == last1)
                    return result;
                else
                {
                    result = new_result;
                    first1 = new_result;
                    ++first1;
                }
            }
        }
    }

    // find_end_dispatch's bidirectional_iterator_tag version
    template <class BidirectionalIter1, class BidirectionalIter2, class Compared>
    BidirectionalIter1 find_end_dispatch(BidirectionalIter1 first1, BidirectionalIter1 last1,
        BidirectionalIter2 first2, BidirectionalIter2 last2,
        bidirectional_iterator_tag, bidirectional_iterator_tag, Compared comp)
    {
        typedef reverse_iterator<BidirectionalIter1> reviter1;
        typedef reverse_iterator<BidirectionalIter2> reviter2;
        reviter1 rlast1(first1);
        reviter2 rlast2(first2);
        reviter1 rresult = tinystl::search(reviter1(last1), rlast1, 
            reviter2(last2), rlast2, comp);

        if (rresult == rlast1)
            return last1;
        else
        {
            auto result = rresult.base();
            tinystl::advance(result, -tinystl::distance(first2, last2));
            return result;
        }
    }

    template <class ForwardIter1, class ForwardIter2, class Compared>
    ForwardIter1 find_end(ForwardIter1 first1, ForwardIter1 last1,
        ForwardIter2 first2, ForwardIter2 last2, Compared comp)
    {
        typedef typename iterator_traits<ForwardIter1>::iterator_category Category1;
        typedef typename iterator_traits<ForwardIter2>::iterator_category Category2;
        return tinystl::find_end_dispatch(first1, last1, first2, last2, 
            Category1(), Category2(), comp);
    }

    //-------------------------------------------------------------------------------------
    // find_first_of
    // Find some elements in [first2, last2) in [first1, last1), 
    // and return an iterator pointing to the element that appears for the first time
    template <class InputIter, class ForwardIter>
    InputIter find_first_of(InputIter first1, InputIter last1,
        ForwardIter first2, ForwardIter last2)
    {
        // this is not a goog algorithm
        for (; first1 != last1; ++first1)
        {
            for (auto iter = first2; iter != last2; ++iter)
            {
                if (*first1 == *iter)
                    return first1;
            }
        }
        return last1;
    }

    // comp
    template <class InputIter, class ForwardIter, class Compared>
    InputIter find_first_of(InputIter first1, InputIter last1,
        ForwardIter first2, ForwardIter last2, Compared comp)
    {
        for (; first1 != last1; ++first1)
        {
            for (auto iter = first2; iter != last2; ++iter)
            {
                if (comp(*first1, *iter))
                    return first1;
            }
        }
        return last1;
    }

    //-------------------------------------------------------------------------------------
    // for_each: f() used to return a value
    // Use a function object f to perform an operator() operation 
    // on each element in the [first, last) range, 
    // but the element content cannot be changed
    template <class InputIter, class Function>
    Function for_each(InputIter first, InputIter last, Function f)
    {
        for (; first != last; ++first)
        {
            f(*first);
        }
        return f;
    }

    //-------------------------------------------------------------------------------------
    // adjacent_find
    // Find the first pair of adjacent elements that match, use operator==  by default, 
    // return an iterator if found, pointer to the first element of the pair
    template <class ForwardIter>
    ForwardIter adjacent_find(ForwardIter first, ForwardIter last)
    {
        if (first == last)  
            return last;

        auto next = first;
        while (++next != last)
        {
            if (*first == *next)  
                return first;

            first = next;
        }
        return last;
    }

    // comp
    template <class ForwardIter, class Compared>
    ForwardIter adjacent_find(ForwardIter first, ForwardIter last, Compared comp)
    {
        if (first == last) 
            return last;

        auto next = first;
        while (++next != last)
        {
            if (comp(*first, *next)) 
                return first;

            first = next;
        }
        return last;
    }

    //=====================================================================================

    //-------------------------------------------------------------------------------------
    // lower_bound
    // Find the first element in [first, last), which is not less than the given value
    // and return an iterator pointing to it, or return last if there is none
    // the elements in [first, last) are arranged in ascending order

    // lbound_dispatch's forward_iterator_tag version
    template <class ForwardIter, class T>
    ForwardIter lbound_dispatch(ForwardIter first, ForwardIter last,
        const T& value, forward_iterator_tag)
    {
        // binary search
        auto len = tinystl::distance(first, last);
        auto half = len;
        ForwardIter middle;

        while (len > 0)
        {
            // >> : divided by 2
            // << : multiplied by 2
            half = len >> 1; 
            middle = first;
            // middle = middle + half
            tinystl::advance(middle, half);
            if (*middle < value)
            {
                first = middle;
                ++first;
                len = len - half - 1;
            }
            else
                len = half;
        }
        return first;
    }

    // lbound_dispatch's random_access_iterator_tag version
    // no need to use advance() function
    template <class RandomIter, class T>
    RandomIter lbound_dispatch(RandomIter first, RandomIter last,
        const T& value, random_access_iterator_tag)
    {
        auto len = last - first;
        auto half = len;
        RandomIter middle;
        while (len > 0)
        {
            half = len >> 1;
            middle = first + half;
            if (*middle < value)
            {
                first = middle + 1;
                len = len - half - 1;
            }
            else
                len = half;
        }
        return first;
    }

    template <class ForwardIter, class T>
    ForwardIter lower_bound(ForwardIter first, ForwardIter last, const T& value)
    {
        return tinystl::lbound_dispatch(first, last, value, iterator_category(first));
    }

    // comp
    // lbound_dispatch's forward_iterator_tag version
    template <class ForwardIter, class T, class Compared>
    ForwardIter lbound_dispatch(ForwardIter first, ForwardIter last,
        const T& value, forward_iterator_tag, Compared comp)
    {
        auto len = tinystl::distance(first, last);
        auto half = len;
        ForwardIter middle;
        while (len > 0)
        {
            half = len >> 1;
            middle = first;
            tinystl::advance(middle, half);
            if (comp(*middle, value))
            {
                first = middle;
                ++first;
                len = len - half - 1;
            }
            else
                len = half;
        }
        return first;
    }

    // lbound_dispatch's random_access_iterator_tag version
    template <class RandomIter, class T, class Compared>
    RandomIter lbound_dispatch(RandomIter first, RandomIter last,
        const T& value, random_access_iterator_tag, Compared comp)
    {
        auto len = last - first;
        auto half = len;
        RandomIter middle;
        while (len > 0)
        {
            half = len >> 1;
            middle = first + half;
            if (comp(*middle, value))
            {
                first = middle + 1;
                len = len - half - 1;
            }
            else
                len = half;
        }
        return first;
    }

    template <class ForwardIter, class T, class Compared>
    ForwardIter lower_bound(ForwardIter first, ForwardIter last, const T& value, 
        Compared comp)
    {
        return tinystl::lbound_dispatch(first, last, value, iterator_category(first), comp);
    }

    //-------------------------------------------------------------------------------------
    // upper_bound
    // find the first element in [first, last), which is bigger than value,
    // and return the iterator pointint to it,
    // or return last if none

    // ubound_dispatch's forward_iterator_tag version
    template <class ForwardIter, class T>
    ForwardIter ubound_dispatch(ForwardIter first, ForwardIter last,
        const T& value, forward_iterator_tag)
    {
        auto len = tinystl::distance(first, last);
        auto half = len;
        ForwardIter middle;
        while (len > 0)
        {
            half = len >> 1;
            middle = first;
            tinystl::advance(middle, half);
            if (*middle>value)
                len = half; // len + 1 := a new last for previous part.
            else
            {
                first = middle;
                ++first;
                len = len - half - 1;
            }
        }
        return first;
    }

    // ubound_dispatch's random_access_iterator_tag version
    template <class RandomIter, class T>
    RandomIter ubound_dispatch(RandomIter first, RandomIter last,
        const T& value, random_access_iterator_tag)
    {
        auto len = last - first;
        auto half = len;
        RandomIter middle;
        while (len > 0)
        {
            half = len >> 1;
            middle = first + half;
            if (*middle>value)
                len = half;
            else
            {
                first = middle + 1;
                len = len - half - 1;
            }
        }
        return first;
    }

    template <class ForwardIter, class T>
    ForwardIter upper_bound(ForwardIter first, ForwardIter last, const T& value)
    {
        return tinystl::ubound_dispatch(first, last, value, iterator_category(first));
    }

    // comp
    // ubound_dispatch's forward_iterator_tag version
    template <class ForwardIter, class T, class Compared>
    ForwardIter ubound_dispatch(ForwardIter first, ForwardIter last,
        const T& value, forward_iterator_tag, Compared comp)
    {
        auto len = tinystl::distance(first, last);
        auto half = len;
        ForwardIter middle;
        while (len > 0)
        {
            half = len >> 1;
            middle = first;
            tinystl::advance(middle, half);
            if (comp(value, *middle))
                len = half;
            else
            {
                first = middle;
                ++first;
                len = len - half - 1;
            }
        }
        return first;
    }

    // ubound_dispatch's random_access_iterator_tag version
    template <class RandomIter, class T, class Compared>
    RandomIter ubound_dispatch(RandomIter first, RandomIter last,
        const T& value, random_access_iterator_tag, Compared comp)
    {
        auto len = last - first;
        auto half = len;
        RandomIter middle;
        while (len > 0)
        {
            half = len >> 1;
            middle = first + half;
            if (comp(value, *middle)) 
                len = half;
            else
            {
                first = middle + 1;
                len = len - half - 1;
            }
        }
        return first;
    }

    template <class ForwardIter, class T, class Compared>
    ForwardIter upper_bound(ForwardIter first, ForwardIter last, const T& value, 
        Compared comp)
    {
        return tinystl::ubound_dispatch(first, last, value, iterator_category(first), comp);
    }

    //-------------------------------------------------------------------------------------
    // binary_search
    // if there is an element equal to value in [first, last), 
    // return true, otherwise return false
    template <class ForwardIter, class T>
    bool binary_search(ForwardIter first, ForwardIter last, const T& value)
    {
        auto i = tinystl::lower_bound(first, last, value);
        return i != last && !(value < *i);
    }

    // comp
    template <class ForwardIter, class T, class Compared>
    bool binary_search(ForwardIter first, ForwardIter last, const T& value, Compared comp)
    {
        auto i = tinystl::lower_bound(first, last, value);
        return i != last && !comp(value, *i);
    }

    //-------------------------------------------------------------------------------------
    // equal_range
    // 查找[first,last)区间中与 value 相等的元素所形成的区间，返回一对迭代器指向区间首尾
    // 第一个迭代器指向第一个不小于 value 的元素，第二个迭代器指向第一个大于 value 的元素

    // erange_dispatch 的 forward_iterator_tag 版本
    template <class ForwardIter, class T>
    tinystl::pair<ForwardIter, ForwardIter> erange_dispatch(ForwardIter first, 
        ForwardIter last, const T& value, forward_iterator_tag)
    {
        auto len = tinystl::distance(first, last);
        auto half = len;
        ForwardIter middle, left, right;
        while (len > 0)
        {
            half = len >> 1;
            middle = first;
            tinystl::advance(middle, half);
            if (*middle < value)
            {
                // find in the right part
                first = middle;
                ++first;
                len = len - half - 1;
            }
            else if (*middle > value)
                len = half; // find in the left part
            else
            {
                // why last cannot be middle
                left = tinystl::lower_bound(first, last, value);
                tinystl::advance(first, len);
                // reduce the range for search
                right = tinystl::upper_bound(++middle, first, value);
                return tinystl::pair<ForwardIter, ForwardIter>(left, right);
            }
        }
        return tinystl::pair<ForwardIter, ForwardIter>(last, last);
    }

    // erange_dispatch's random_access_iterator_tag version
    template <class RandomIter, class T>
    tinystl::pair<RandomIter, RandomIter>erange_dispatch(RandomIter first, RandomIter last,
        const T& value, random_access_iterator_tag)
    {
        auto len = last - first;
        auto half = len;
        RandomIter middle, left, right;
        while (len > 0)
        {
            half = len >> 1;
            middle = first + half;
            if (*middle < value)
            {
                first = middle + 1;
                len = len - half - 1;
            }
            else if (value < *middle)
                len = half;
            else
            {
                left = tinystl::lower_bound(first, middle, value);
                right = tinystl::upper_bound(++middle, first + len, value);
                return tinystl::pair<RandomIter, RandomIter>(left, right);
            }
        }
        return tinystl::pair<RandomIter, RandomIter>(last, last);
    }

    template <class ForwardIter, class T>
    tinystl::pair<ForwardIter, ForwardIter> equal_range(ForwardIter first, 
        ForwardIter last, const T& value)
    {
        return tinystl::erange_dispatch(first, last, value, iterator_category(first));
    }

    // comp
    // erange_dispatch's forward iterator version
    template <class ForwardIter, class T, class Compared>
    tinystl::pair<ForwardIter, ForwardIter>erange_dispatch(ForwardIter first, 
        ForwardIter last, const T& value, forward_iterator_tag, Compared comp)
    {
        auto len = tinystl::distance(first, last);
        auto half = len;
        ForwardIter middle, left, right;
        while (len > 0)
        {
            half = len >> 1;
            middle = first;
            tinystl::advance(middle, half);
            if (comp(*middle, value))
            {
                first = middle;
                ++first;
                len = len - half - 1;
            }
            else if (comp(value, *middle))
                len = half;
            else
            {
                left = tinystl::lower_bound(first, last, value, comp);
                tinystl::advance(first, len);
                right = tinystl::upper_bound(++middle, first, value, comp);
                return tinystl::pair<ForwardIter, ForwardIter>(left, right);
            }
        }
        return tinystl::pair<ForwardIter, ForwardIter>(last, last);
    }

    // erange_dispatch's random access iterator version
    template <class RandomIter, class T, class Compared>
    tinystl::pair<RandomIter, RandomIter>
    erange_dispatch(RandomIter first, RandomIter last,
                    const T& value, random_access_iterator_tag, Compared comp)
    {
        auto len = last - first;
        auto half = len;
        RandomIter middle, left, right;
        while (len > 0)
        {
            half = len >> 1;
            middle = first + half;
            if (comp(*middle, value))
            {
                first = middle + 1;
                len = len - half - 1;
            }
            else if (comp(value, *middle))
                len = half;
            else
            {
                left = tinystl::lower_bound(first, middle, value, comp);
                right = tinystl::upper_bound(++middle, first + len, value, comp);
                return tinystl::pair<RandomIter, RandomIter>(left, right);
            }
        }
        return tinystl::pair<RandomIter, RandomIter>(last, last);
    }

    template <class ForwardIter, class T, class Compared>
    tinystl::pair<ForwardIter, ForwardIter>equal_range(ForwardIter first, 
        ForwardIter last, const T& value, Compared comp)
    {
        return tinystl::erange_dispatch(first, last, value, iterator_category(first), comp);
    }

    //=====================================================================================

    //-------------------------------------------------------------------------------------
    // generate
    // Assign the operation result of the function object gen 
    // to each element in [first, last)
    template <class ForwardIter, class Generator>
    void generate(ForwardIter first, ForwardIter last, Generator gen)
    {
        for (; first != last; ++first)
            *first = gen();
    }

    //-------------------------------------------------------------------------------------
    // generate_n
    // Assign the operatioon result of the function object gen
    // n elements consecutively
    template <class ForwardIter, class Size, class Generator>
    void generate_n(ForwardIter first, Size n, Generator gen)
    {
        for (; n > 0; --n, ++first)
            *first = gen();
    }

    //-------------------------------------------------------------------------------------
    // includes
    // 判断序列一S1 是否包含序列二S2
    template <class InputIter1, class InputIter2>
    bool includes(InputIter1 first1, InputIter1 last1,InputIter2 first2, InputIter2 last2)
    {
        // make sure every element in [first2, last2) in [first1, last1)
        while (first1 != last1 && first2 != last2)
        {
            if (*first2 < *first1)
                return false;
            else if (*first1 < *first2)
                ++first1;
            else
                ++first1, ++first2;
        }
        return first2 == last2;
    }

    // comp
    template <class InputIter1, class InputIter2, class Compared>
    bool includes(InputIter1 first1, InputIter1 last1,
                InputIter2 first2, InputIter2 last2, Compared comp)
    {
        while (first1 != last1 && first2 != last2)
        {
            if (comp(*first2, *first1))
                return false;
            else if (comp(*first1, *first2))
                ++first1;
            else
                ++first1, ++first2;
        }
        return first2 == last2;
    }

    //-------------------------------------------------------------------------------------
    // is_heap (Min-heap)
    // Check if the elements in [first, last) are a heap, if so, return true
    template <class RandomIter>
    bool is_heap(RandomIter first, RandomIter last)
    {
        auto n = tinystl::distance(first, last);
        auto parent = 0;
        for (auto child = 1; child < n; ++child)
        {
            if (first[parent] < first[child])
                return false;

            // x & 1 is equivalent to x % 2.
            // determine the child node is wheather in the last left in this line.
            if ((child & 1) == 0)
                ++parent;
        }
        return true;
    }

    // comp
    template <class RandomIter, class Compared>
    bool is_heap(RandomIter first, RandomIter last, Compared comp)
    {
	    auto n = tinystl::distance(first, last);
	    auto parent = 0;
	    for (auto child = 1; child < n; ++child)
	    {
		if (comp(first[parent], first[child]))
			return false;
		if ((child & 1) == 0)
			++parent;
	    }
	    return true;
    }
}
