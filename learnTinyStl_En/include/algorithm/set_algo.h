#ifndef SET_ALGO_H_
#define SET_ALGO_H_

// union:                S1∪S2 
// intersection:         (S1 n S2)
// difference:           S1-S2
// symmetric_difference: (S1-S2)∪(S2-S1)

// All sequences are sorted from smallest to largest

#include "algobase.h"
#include "iterator.h"

namespace tinystl
{
    //------------------------------------------------------------------------------------------
    // set_union: S1∪S2
    // [-----------] S1                      [-----------] S1
    //        [-----------] S2        [-----------] S2
    // [------------------] S1∪S2     [------------------] S1∪S2
    // Return an iterator pointing to the end of the output result
    template <class InputIter1, class InputIter2, class OutputIter>
    OutputIter set_union(InputIter1 first1, InputIter1 last1,
        InputIter2 first2, InputIter2 last2, OutputIter result)
    {
        while (first1 != last1 && first2 != last2)
        {
            if (*first1 < *first2)
            {
                *result = *first1;
                ++first1;
            }
            else if (*first2 < *first1)
            {
                *result = *first2;
                ++first2;
            }
            else
            {
                *result = *first1;
                ++first1;
                ++first2;
            }
            ++result;
        }
        // copy the left elements which might be in the [first1,last1) or [first2,last2)
        return tinystl::copy(first2, last2, tinystl::copy(first1, last1, result));
    }

    // overload version
    template <class InputIter1, class InputIter2, class OutputIter, class Compared>
    OutputIter set_union(InputIter1 first1, InputIter1 last1,
        InputIter2 first2, InputIter2 last2, OutputIter result, Compared comp)
    {
        while (first1 != last1 && first2 != last2)
        {
            if (comp(*first1, *first2))
            {
                *result = *first1;
                ++first1;
            }
            else if (comp(*first2, *first1))
            {
                *result = *first2;
                ++first2;
            }
            else
            {
                *result = *first1;
                ++first1;
                ++first2;
            }
            ++result;
        }
        return tinystl::copy(first2, last2, tinystl::copy(first1, last1, result));
    }

    //------------------------------------------------------------------------------------------
    // set_intersection
    // [-----------] S1                      [-----------] S1
    //        [-----------] S2        [-----------] S2
    //        [----] S1∪S2                   [----] S1∪S2
    // Return an iterator pointing to the end of the output result
    template <class InputIter1, class InputIter2, class OutputIter>
    OutputIter set_intersection(InputIter1 first1, InputIter1 last1,
        InputIter2 first2, InputIter2 last2, OutputIter result)
    {
        while (first1 != last1 && first2 != last2)
        {
            if (*first1 < *first2)
            {
                ++first1;
            }
            else if (*first2 < *first1)
            {
                ++first2;
            }
            else
            {
                *result = *first1;
                ++first1;
                ++first2;
                ++result;
            }
        }
        return result;
    }

    // overload version
    template <class InputIter1, class InputIter2, class OutputIter, class Compared>
    OutputIter set_intersection(InputIter1 first1, InputIter1 last1,
        InputIter2 first2, InputIter2 last2, OutputIter result, Compared comp)
    {
        while (first1 != last1 && first2 != last2)
        {
            if (comp(*first1, *first2))
            {
                ++first1;
            }
            else if (comp(*first2, *first1))
            {
                ++first2;
            }
            else
            {
                *result = *first1;
                ++first1;
                ++first2;
                ++result;
            }
        }
        return result;
    }

    //------------------------------------------------------------------------------------------
    // set_difference
    // [-----------] S1                      [-----------] S1
    //        [-----------] S2        [-----------] S2
    // [------] S1-S2                             [------] S1-S2
    // Return an iterator pointing to the end of the output result
    template <class InputIter1, class InputIter2, class OutputIter>
    OutputIter set_difference(InputIter1 first1, InputIter1 last1,
                            InputIter2 first2, InputIter2 last2,
                            OutputIter result)
    {
        while (first1 != last1 && first2 != last2)
        {
            if (*first1 < *first2)
            {
                *result = *first1;
                ++first1;
                ++result;
            }
            else if (*first2 < *first1)
            {
                ++first2;
            }
            else
            {
                ++first1;
                ++first2;
            }
        }
        // for the right situation
        return tinystl::copy(first1, last1, result);
    }

    // overload version
    template <class InputIter1, class InputIter2, class OutputIter, class Compared>
    OutputIter set_difference(InputIter1 first1, InputIter1 last1,
        InputIter2 first2, InputIter2 last2, OutputIter result, Compared comp)
    {
        while (first1 != last1 && first2 != last2)
        {
            if (comp(*first1, *first2))
            {
                *result = *first1;
                ++first1;
                ++result;
            }
            else if (comp(*first2, *first1))
            {
                ++first2;
            }
            else
            {
                ++first1;
                ++first2;
            }
        }
        return tinystl::copy(first1, last1, result);
    }

    //------------------------------------------------------------------------------------------
    // set_symmetric_difference
    // [-----------] S1                      [-----------] S1
    //        [-----------] S2        [-----------] S2
    // [------]  + [------]S1-S2      [------] +  [------] S1-S2
    // Return an iterator pointing to the end of the output result
    template <class InputIter1, class InputIter2, class OutputIter>
    OutputIter set_symmetric_difference(InputIter1 first1, InputIter1 last1,
        InputIter2 first2, InputIter2 last2, OutputIter result)
    {
        while (first1 != last1 && first2 != last2)
        {
            if (*first1 < *first2)
            {
                *result = *first1;
                ++first1;
                ++result;
            }
            else if (*first2 < *first1)
            {
                *result = *first2;
                ++first2;
                ++result;
            }
            else
            {
                ++first1;
                ++first2;
            }
        }
        return tinystl::copy(first2, last2, tinystl::copy(first1, last1, result));
    }

    // overload version
    template <class InputIter1, class InputIter2, class OutputIter, class Compared>
    OutputIter set_symmetric_difference(InputIter1 first1, InputIter1 last1,
        InputIter2 first2, InputIter2 last2, OutputIter result, Compared comp)
    {
        while (first1 != last1 && first2 != last2)
        {
            if (comp(*first1, *first2))
            {
                *result = *first1;
                ++first1;
                ++result;
            }
            else if (comp(*first2, *first1))
            {
                *result = *first2;
                ++first2;
                ++result;
            }
            else
            {
                ++first1;
                ++first2;
            }
        }
        return tinystl::copy(first2, last2, tinystl::copy(first1, last1, result));
    }

} // namespace tinystl
#endif 