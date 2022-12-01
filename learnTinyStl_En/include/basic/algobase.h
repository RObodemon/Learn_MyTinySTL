#ifndef ALGOBASE_H_
#define ALGOBASE_H_

// basic algorithms
// std::enable_if<B,T>
// B: The value that determines the existence of the resulting type.  
// T: The type to instantiate if B is true.  
#include <cstring>

#include "iterator.h"
#include "util.h"

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

    //-------------------------------------------------------------------------------------
    // max
    template <class T>
    const T& max(const T& lhs, const T& rhs)
    {
        return lhs < rhs ? rhs : lhs;
    }
    // The overloaded version 
    // uses the function object comp instead of the comparison operation
    template <class T, class Compare>
    const T& max(const T& lhs, const T& rhs, Compare comp)
    {
        return comp(lhs, rhs) ? rhs : lhs;
    }

    //-------------------------------------------------------------------------------------
    // min 
    template <class T>
    const T& min(const T& lhs, const T& rhs)
    {
        return rhs < lhs ? rhs : lhs;
    }
    // overload version
    template <class T, class Compare>
    const T& min(const T& lhs, const T& rhs, Compare comp)
    {
        return comp(rhs, lhs) ? rhs : lhs;
    }

    //-------------------------------------------------------------------------------------
    // iter_swap
    // swap two iteratos
    template <class FIter1, class FIter2>
    void iter_swap(FIter1 lhs, FIter2 rhs)
    {  
        // in util.h, use move() to do the swap
        mystl::swap(*lhs, *rhs);
    }

    //-------------------------------------------------------------------------------------
    // copy
    // copy [first, last) to [result, result + (last - first))
    // input_iterator_tag 
    template <class InputIter, class OutputIter>
    OutputIter unchecked_copy_cat(InputIter first, InputIter last, OutputIter result, 
        mystl::input_iterator_tag)
    {
        for (; first != last; ++first, ++result)
        {
            *result = *first;
        }
        return result;
    }
    // ramdom_access_iterator_tag 
    template <class RandomIter, class OutputIter>
    OutputIter unchecked_copy_cat(RandomIter first, RandomIter last, OutputIter result,
        mystl::random_access_iterator_tag)
    {
        for (auto n = last - first; n > 0; --n, ++first, ++result)
        {
            *result = *first;
        }
        return result;
    }

    template <class InputIter, class OutputIter>
    OutputIter unchecked_copy(InputIter first, InputIter last, OutputIter result)
    {
        // extrack the type of iterator, then go do input or random tag
        return unchecked_copy_cat(first, last, result, iterator_category(first));
    }

    // trivially_copy_assignable:
    // Tests whether the type has a trivial copy assignment operator.
    // If no copy assignment operator is defined, 
    // it is most efficient to copy directly through memmove(), 
    // otherwise call unchecked_copy_cat()
    template <class Tp, class Up>
    typename std::enable_if<
        std::is_same<typename std::remove_const<Tp>::type, Up>::value 
        && std::is_trivially_copy_assignable<Up>::value, 
    Up*>::type
    unchecked_copy(Tp* first, Tp* last, Up* result)
    {
        const auto n = static_cast<size_t>(last - first);
        if (n != 0)
            std::memmove(result, first, n * sizeof(Up));
        return result + n;
    }

    template <class InputIter, class OutputIter>
    OutputIter copy(InputIter first, InputIter last, OutputIter result)
    {
        // copy(): fistly, it will call tricially_copy_assignable version,
        //         determine to use unchecked_copy() or not;
        //         secondly, call unchecked_copy_cat() to extract the type of the iterator;
        return unchecked_copy(first, last, result);
    }

    //-------------------------------------------------------------------------------------
    // copy_backward() the same as the above copy function
    // copy [first, last) to [result - (last - first), result)
    // bidirectional_iterator_tag 
    template <class BidirectionalIter1, class BidirectionalIter2>
    BidirectionalIter2 
    unchecked_copy_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
        BidirectionalIter2 result, mystl::bidirectional_iterator_tag)
    {
        while (first != last)
            *--result = *--last;
        return result;
    }

    // random_access_iterator_tag
    template <class BidirectionalIter1, class BidirectionalIter2>
    BidirectionalIter2 
    unchecked_copy_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
        BidirectionalIter2 result, mystl::random_access_iterator_tag)
    {
        for (auto n = last - first; n > 0; --n)
            *--result = *--last;
        return result;
    }

    template <class BidirectionalIter1, class BidirectionalIter2>
    BidirectionalIter2 
    unchecked_copy_backward(BidirectionalIter1 first, BidirectionalIter1 last,
        BidirectionalIter2 result)
    {
        return unchecked_copy_backward_cat(first, last, result,
            iterator_category(first));
    }

    // trivially_copy_assignable
    template <class Tp, class Up>
    typename std::enable_if<
        std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
        std::is_trivially_copy_assignable<Up>::value,
    Up*>::type
    unchecked_copy_backward(Tp* first, Tp* last, Up* result)
    {
        const auto n = static_cast<size_t>(last - first);
        if (n != 0)
        {
            result -= n;
            std::memmove(result, first, n * sizeof(Up));
        }
        return result;
    }

    template <class BidirectionalIter1, class BidirectionalIter2>
    BidirectionalIter2 
    copy_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result)
    {
        return unchecked_copy_backward(first, last, result);
    }

    //-------------------------------------------------------------------------------------
    // copy_if()
    // Copy the elements in [first, last) that 
    // satisfy the unary operation unary_pred to the position starting with result
    template <class InputIter, class OutputIter, class UnaryPredicate>
    OutputIter copy_if(InputIter first, InputIter last, OutputIter result, 
        UnaryPredicate unary_pred)
    {
        for (; first != last; ++first)
        {
            if (unary_pred(*first))
            *result++ = *first;
        }
        return result;
    }

    //-------------------------------------------------------------------------------------
    // copy_n()
    // Copy the elements in the range [first, first + n) to [result, result + n)
    // Return a pair pointing to the end of the copy respectively
    // input_iterator_tag
    template <class InputIter, class Size, class OutputIter>
    mystl::pair<InputIter, OutputIter> unchecked_copy_n(InputIter first, Size n, 
        OutputIter result, mystl::input_iterator_tag)
    {
        for (; n > 0; --n, ++first, ++result)
        {
            *result = *first;
        }

        return mystl::pair<InputIter, OutputIter>(first, result);
    }
    // random_access_iterator_tag
    template <class RandomIter, class Size, class OutputIter>
    mystl::pair<RandomIter, OutputIter> unchecked_copy_n(RandomIter first, Size n, 
        OutputIter result, mystl::random_access_iterator_tag)
    {
        auto last = first + n;
        return mystl::pair<RandomIter, OutputIter>(last, mystl::copy(first, last, result));
    }

    template <class InputIter, class Size, class OutputIter>
    mystl::pair<InputIter, OutputIter> copy_n(InputIter first, Size n, OutputIter result)
    {
        return unchecked_copy_n(first, n, result, iterator_category(first));
    }

    //-------------------------------------------------------------------------------------
    // move
    // Move the elements in the interval [first, last) to [result, result + (last - first))
    // input_iterator_tag 
    template <class InputIter, class OutputIter>
    OutputIter unchecked_move_cat(InputIter first, InputIter last,
        OutputIter result, mystl::input_iterator_tag)
    {
        for (; first != last; ++first, ++result)
        {
            // it will be changed to be rvalue
            // don't use the copy constructor for me, use the move constructor.
            *result = mystl::move(*first);
        }
        return result;
    }
    // ramdom_access_iterator_tag 
    template <class RandomIter, class OutputIter>
    OutputIter unchecked_move_cat(RandomIter first, RandomIter last, 
        OutputIter result, mystl::random_access_iterator_tag)
    {
        for (auto n = last - first; n > 0; --n, ++first, ++result)
        {
            *result = mystl::move(*first);
        }
        return result;
    }

    template <class InputIter, class OutputIter>
    OutputIter unchecked_move(InputIter first, InputIter last, OutputIter result)
    {
        return unchecked_move_cat(first, last, result, iterator_category(first));
    }

    // trivially_copy_assignable 
    template <class Tp, class Up>
    typename std::enable_if<
        std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
        std::is_trivially_move_assignable<Up>::value,
    Up*>::type unchecked_move(Tp* first, Tp* last, Up* result)
    {
        const size_t n = static_cast<size_t>(last - first);
        if (n != 0)
            std::memmove(result, first, n * sizeof(Up));
        return result + n;
    }

    template <class InputIter, class OutputIter>
    OutputIter move(InputIter first, InputIter last, OutputIter result)
    {
        return unchecked_move(first, last, result);
    }

    //-------------------------------------------------------------------------------------
    // move_backward
    // Move the elements in the range [first, last) to [result - (last - first), result)
    // bidirectional_iterator_tag
    template <class BidirectionalIter1, class BidirectionalIter2>
    BidirectionalIter2 unchecked_move_backward_cat(BidirectionalIter1 first, 
        BidirectionalIter1 last, BidirectionalIter2 result, mystl::bidirectional_iterator_tag)
    {
        while (first != last)
            *--result = mystl::move(*--last);
        return result;
    }

    // random_access_iterator_tag
    template <class RandomIter1, class RandomIter2>
    RandomIter2 unchecked_move_backward_cat(RandomIter1 first, RandomIter1 last,
        RandomIter2 result, mystl::random_access_iterator_tag)
    {
        for (auto n = last - first; n > 0; --n)
            *--result = mystl::move(*--last);
        return result;
    }

    template <class BidirectionalIter1, class BidirectionalIter2>
    BidirectionalIter2 unchecked_move_backward(BidirectionalIter1 first, BidirectionalIter1 last, 
        BidirectionalIter2 result)
    {
        return unchecked_move_backward_cat(first, last, result,
            iterator_category(first));
    }

    // trivially_copy_assignable
    template <class Tp, class Up>
    typename std::enable_if<
        std::is_same<typename std::remove_const<Tp>::type, Up>::value &&
        std::is_trivially_move_assignable<Up>::value,
    Up*>::type unchecked_move_backward(Tp* first, Tp* last, Up* result)
    {
        const size_t n = static_cast<size_t>(last - first);
        if (n != 0)
        {
            result -= n;
            std::memmove(result, first, n * sizeof(Up));
        }
        return result;
    }

    template <class BidirectionalIter1, class BidirectionalIter2>
    BidirectionalIter2 move_backward(BidirectionalIter1 first, BidirectionalIter1 last, 
        BidirectionalIter2 result)
    {
        return unchecked_move_backward(first, last, result);
    }

    //-------------------------------------------------------------------------------------
    // equal
    // Compare whether the element values of the first sequence on the interval [first, last) 
    // are equal to the second sequence, if there is one element is not the same,
    // will return false
    template <class InputIter1, class InputIter2>
    bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2)
    {
        for (; first1 != last1; ++first1, ++first2)
        {
            if (*first1 != *first2)  
            return false;
        }
        return true;
    }

    // Overload version
    template <class InputIter1, class InputIter2, class Compared>
    bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2, Compared comp)
    {
        for (; first1 != last1; ++first1, ++first2)
        {
            if (!comp(*first1, *first2))  
            return false;
        }
        return true;
    }

    //-------------------------------------------------------------------------------------
    // fill_n
    // Fill n values starting at position first
    template <class OutputIter, class Size, class T>
    OutputIter unchecked_fill_n(OutputIter first, Size n, const T& value)
    {
        for (; n > 0; --n, ++first)
        {
            *first = value;
        }
        return first;
    }
    // one-byte
    // is_integral: tests if the type Tp is integral or not.
    template <class Tp, class Size, class Up>
    typename std::enable_if<
        std::is_integral<Tp>::value && sizeof(Tp) == 1 &&
        !std::is_same<Tp, bool>::value &&
        std::is_integral<Up>::value && sizeof(Up) == 1,
    Tp*>::type unchecked_fill_n(Tp* first, Size n, Up value)
    {
        if (n > 0)
        {
            // void *memset(void *str, int c, size_t n) Copies the character c (an unsigned char) 
            // to the first n characters of the string pointed to by the parameter str.
            std::memset(first, (unsigned char)value, (size_t)(n));
        }
        return first + n;
    }

    template <class OutputIter, class Size, class T>
    OutputIter fill_n(OutputIter first, Size n, const T& value)
    {
        return unchecked_fill_n(first, n, value);
    }

    //-------------------------------------------------------------------------------------
    // fill()
    // Fill new values for all elements in the range [first, last)
    // forward_iterator_tag
    template <class ForwardIter, class T>
    void fill_cat(ForwardIter first, ForwardIter last, const T& value,
                mystl::forward_iterator_tag)
    {
    for (; first != last; ++first)
    {
        *first = value;
    }
    }
    // random_access_iterator_tag
    template <class RandomIter, class T>
    void fill_cat(RandomIter first, RandomIter last, const T& value,
                mystl::random_access_iterator_tag)
    {
        fill_n(first, last - first, value);
    }

    template <class ForwardIter, class T>
    void fill(ForwardIter first, ForwardIter last, const T& value)
    {
        fill_cat(first, last, value, iterator_category(first));
    }

    //-------------------------------------------------------------------------------------
    // lexicographical_compare
    // Compare two sequences in lexicographical order. 
    // When the first group of unequal elements is found at a certain position.

    // there are the following situations:
    // (1) If the element of the first sequence is smaller, return true, otherwise return false
    // (2) Return true if last1 has been reached but not last2
    // (3) If last2 is reached but not last1, return false
    // (4) Return false if both last1 and last2 are reached at the same time
    template <class InputIter1, class InputIter2>
    bool lexicographical_compare(InputIter1 first1, InputIter1 last1,
        InputIter2 first2, InputIter2 last2)
    {
        for (; first1 != last1 && first2 != last2; ++first1, ++first2)
        {
            if (*first1 < *first2)
                return true;
            if (*first2 < *first1)
                return false;
        }

        return first1 == last1 && first2 != last2;
    }

    // overload version
    template <class InputIter1, class InputIter2, class Compred>
    bool lexicographical_compare(InputIter1 first1, InputIter1 last1,
        InputIter2 first2, InputIter2 last2, Compred comp)
    {
        for (; first1 != last1 && first2 != last2; ++first1, ++first2)
        {
            if (comp(*first1, *first2))
                return true;
            if (comp(*first2, *first1))
                return false;
        }

        return first1 == last1 && first2 != last2;
    }

    // const unsigned char*
    bool lexicographical_compare(const unsigned char* first1,
        const unsigned char* last1,const unsigned char* first2,
        const unsigned char* last2)
    {
        const auto len1 = last1 - first1;
        const auto len2 = last2 - first2;
        // compare parts of the same length
        // void *memcpy(void *str1, const void *str2, size_t n) 
        // Copy n bytes from str2 to str1.
        // return a pointer to the target memory area str1.
        const auto result = std::memcmp(first1, first2, mystl::min(len1, len2));

        // If they are equal, the longer one is bigger
        return result != 0 ? result < 0 : len1 < len2;
    }

    //-------------------------------------------------------------------------------------
    // mismatch
    // Compare two sequences in parallel, find the first mismatched element, 
    // and return a pair of iterators, 
    // pointing to the mismatched elements in the two sequences respectively
    template <class InputIter1, class InputIter2>
    mystl::pair<InputIter1, InputIter2> 
    mismatch(InputIter1 first1, InputIter1 last1, InputIter2 first2)
    {
        while (first1 != last1 && *first1 == *first2)
        {
            ++first1;
            ++first2;
        }
        return mystl::pair<InputIter1, InputIter2>(first1, first2);
    }

    // overload version
    template <class InputIter1, class InputIter2, class Compred>
    mystl::pair<InputIter1, InputIter2> 
    mismatch(InputIter1 first1, InputIter1 last1, InputIter2 first2, Compred comp)
    {
        while (first1 != last1 && comp(*first1, *first2))
        {
            ++first1;
            ++first2;
        }
        return mystl::pair<InputIter1, InputIter2>(first1, first2);
    }

} // namespace tinystl
#endif

