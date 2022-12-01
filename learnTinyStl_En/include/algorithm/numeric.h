#ifndef NUMERIC_H_
#define NUMERIC_H_

// numeric calculation
// accumulate:          accumulation algorithm (overloaded, function object)
// adjacent_difference: Calculate the adjacent difference (overloaded, function object)
// inner_product:       calculates the product of two intervals (overloaded, two function objects)
// iota:                increment
// partial_sum:         partial cumulative sum, and save (overload, function object)

#include "iterator.h"

namespace tinystl
{
    //------------------------------------------------------------------------------------------
    // accumulate
    template <class InputIter, class T>
    T accumulate(InputIter first, InputIter last, T init)
    {
        for (; first != last; ++first)
        {
            init += *first;
        }
        return init;
    }
    // overload version
    template <class InputIter, class T, class BinaryOp>
    T accumulate(InputIter first, InputIter last, T init, BinaryOp binary_op)
    {
        for (; first != last; ++first)
        {
            init = binary_op(init, *first);
        }
        return init;
    }

    //------------------------------------------------------------------------------------------
    // adjacent_difference
    // Calculate the difference between adjacent elements, 
    // and save the result to the interval starting with result
    template <class InputIter, class OutputIter>
    OutputIter adjacent_difference(InputIter first, InputIter last, OutputIter result)
    {
    if (first == last)  return result;
    *result = *first;  // 记录第一个元素
    auto value = *first;
    while (++first != last)
    {
        auto tmp = *first;
        *++result = tmp - value;
        value = tmp;
    }
    return ++result;
    }

    // overload version
    template <class InputIter, class OutputIter, class BinaryOp>
    OutputIter adjacent_difference(InputIter first, InputIter last, OutputIter result,
        BinaryOp binary_op)
    {
        if (first == last)  return result;
        *result = *first;  // 记录第一个元素
        auto value = *first;
        while (++first != last)
        {
            auto tmp = *first;
            *++result = binary_op(tmp, value);
            value = tmp;
        }
        return ++result;
    }

    //------------------------------------------------------------------------------------------
    // inner_product
    template <class InputIter1, class InputIter2, class T>
    T inner_product(InputIter1 first1, InputIter1 last1, InputIter2 first2, T init)
    {
        for (; first1 != last1; ++first1, ++first2)
        {
            init = init + (*first1) * (*first2);
        }
        return init;
    }

    template <class InputIter1, class InputIter2, class T, class BinaryOp1, class BinaryOp2>
    T inner_product(InputIter1 first1, InputIter1 last1, InputIter2 first2, T init,
        BinaryOp1 binary_op1, BinaryOp2 binary_op2)
    {
        for (; first1 != last1; ++first1, ++first2)
        {
            init = binary_op1(init, binary_op2(*first1, *first2));
        }
        return init;
    }

    //------------------------------------------------------------------------------------------
    // iota()
    // Fill in [first, last), start incrementing with value as the initial value
    template <class ForwardIter, class T>
    void iota(ForwardIter first, ForwardIter last, T value)
    {
        while (first != last)
        {
            *first++ = value;
            ++value;
        }
    }

    //------------------------------------------------------------------------------------------
    // partial_sum
    // sum the elements from first to last, saved in result
    template <class InputIter, class OutputIter>
    OutputIter partial_sum(InputIter first, InputIter last, OutputIter result)
    {
        if (first == last)  
            return result;

        *result = *first;  // record the first element
        auto value = *first;
        while (++first != last)
        {
            value = value + *first;
            *++result = value;
        }
        return ++result;
    }

    // overload version
    template <class InputIter, class OutputIter, class BinaryOp>
    OutputIter partial_sum(InputIter first, InputIter last, OutputIter result,
        BinaryOp binary_op)
    {
        if (first == last)  
            return result;

        *result = *first; 
        auto value = *first;
        while (++first != last)
        {
            value = binary_op(value, *first);
            *++result = value;
        }
        return ++result;
    }

} // namespace tinystl
#endif // 
