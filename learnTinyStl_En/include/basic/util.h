#ifndef UTIL_H_
#define UTIL_H_

#include<cstddef>

#include "type_traits.h"

namespace tinystl
{
    // move: return right value
    // change the left value to the right value, and 
    template <class T>
    typename std::remove_reference<T>::type&& move(T&& arg) noexcept
    {
        // std::remove_reference: remove the referenced relationship
        // static_cast <type-id> ( expression ):
        // Converts an expression to the type of type-id, 
        // based only on the types that are present in the expression.
        return static_cast<typename std::remove_reference<T>::type&&>(arg);
    }

    // forward: change the lvalue reference to the rvalue reference
    template <class T>
    T&& forward(typename std::remove_reference<T>::type& arg) noexcept
    {
        return static_cast<T&&>(arg);
    }
    
    template <class T>
    T&& forward(typename std::remove_reference<T>::type&& arg) noexcept
    {
        // static_assert
        // If the specified constant expression is false, 
        // the compiler displays the specified message
        static_assert(!std::is_lvalue_reference<T>::value, "bad forward");
        return static_cast<T&&>(arg);
    }

    // swap
    template <class Tp>
    void swap(Tp& lhs, Tp& rhs)
    {
        auto tmp(tinystl::move(lhs)); // tmp = Tp&& lhs
        lhs = tinystl::move(rhs);     // lhs = Tp&& rhs
        rhs = tinystl::move(tmp);     // rhs = Tp&& tmp
    }

    template <class ForwardIter1, class ForwardIter2>
    ForwardIter2 swap_range(ForwardIter1 first1, ForwardIter1 last1, ForwardIter2 first2)
    {
        for (; first1 != last1; ++first1, (void) ++first2)
            tinystl::swap(*first1, *first2);
        return first2;
    }
    
    template <class Tp, size_t N>
    void swap(Tp(&a)[N], Tp(&b)[N]) // Tp& a[N], Tp& b[N]
    {
        tinystl::swap_range(a, a + N, b);
    }

    //-------------- pair ---------------------//
    // The two template parameters represent the types of the two data respectively
    // Use first and second to fetch the first data and the second data respectively
    template<class Ty1, class Ty2>
    struct pair
    {
        typedef Ty1 first_type;
        typedef Ty2 second_type;

        first_type first;    // first data
        second_type second;  // second data

        // is_default_constructiable:
        // Tests if a type has a default constructor.
        // enable_if:
        // if (std::is_default_constructible<Other1>::value && std::is_default_constructible<Other2>::value)
        // is true, type = void
        template <class Other1 = Ty1, class Other2 = Ty2, typename std::enable_if<
        std::is_default_constructible<Other1>::value &&
        std::is_default_constructible<Other2>::value, 
        void>::type>
        constexpr pair(): first(), second()
        {
        }

        // implicit constructiable for this type
        template <class U1 = Ty1, class U2 = Ty2, typename std::enable_if<
        std::is_copy_constructible<U1>::value &&
        std::is_copy_constructible<U2>::value &&
        std::is_convertible<const U1&, Ty1>::value &&
        std::is_convertible<const U2&, Ty2>::value, int>::type = 0>
        constexpr pair(const Ty1& a, const Ty2& b)
        : first(a), second(b)
        {
        }

        // explicit constructible for this type
        template <class U1 = Ty1, class U2 = Ty2,typename std::enable_if<
        std::is_copy_constructible<U1>::value &&
        std::is_copy_constructible<U2>::value &&
        (!std::is_convertible<const U1&, Ty1>::value ||
        !std::is_convertible<const U2&, Ty2>::value), int>::type = 0>
        explicit constexpr pair(const Ty1& a, const Ty2& b)
        : first(a), second(b)
        {
        }
        
        // copy_constructor and move constructor
        // all are default
        pair(const pair& rhs) = default;
        pair(pair&& rhs) = default;

        // implicit constructiable for other type
        template <class Other1, class Other2,typename std::enable_if<
        std::is_constructible<Ty1, Other1>::value &&
        std::is_constructible<Ty2, Other2>::value &&
        std::is_convertible<Other1&&, Ty1>::value &&
        std::is_convertible<Other2&&, Ty2>::value, int>::type = 0>
        constexpr pair(Other1&& a, Other2&& b)
        : first(tinystl::forward<Other1>(a)),
        second(tinystl::forward<Other2>(b))
        {
        }

        // explicit constructiable for other type
        template <class Other1, class Other2, typename std::enable_if<
        std::is_constructible<Ty1, Other1>::value &&
        std::is_constructible<Ty2, Other2>::value &&
        (!std::is_convertible<Other1, Ty1>::value ||
        !std::is_convertible<Other2, Ty2>::value), int>::type = 0>
        explicit constexpr pair(Other1&& a, Other2&& b)
        : first(tinystl::forward<Other1>(a)),
        second(tinystl::forward<Other2>(b))
        {
        }

        // implicit constructiable for other pair
        template <class Other1, class Other2, typename std::enable_if<
        std::is_constructible<Ty1, const Other1&>::value &&
        std::is_constructible<Ty2, const Other2&>::value &&
        std::is_convertible<const Other1&, Ty1>::value &&
        std::is_convertible<const Other2&, Ty2>::value, int>::type = 0>
        constexpr pair(const pair<Other1, Other2>& other)
        : first(other.first),
        second(other.second)
        {
        }

        // explicit constructiable for other pair
        template <class Other1, class Other2, typename std::enable_if<
        std::is_constructible<Ty1, const Other1&>::value &&
        std::is_constructible<Ty2, const Other2&>::value &&
        (!std::is_convertible<const Other1&, Ty1>::value ||
        !std::is_convertible<const Other2&, Ty2>::value), int>::type = 0>
        explicit constexpr pair(const pair<Other1, Other2>& other)
        : first(other.first),
        second(other.second)
        {
        }

        // implicit constructiable for other pair
        template <class Other1, class Other2, typename std::enable_if<
        std::is_constructible<Ty1, Other1>::value &&
        std::is_constructible<Ty2, Other2>::value &&
        std::is_convertible<Other1, Ty1>::value &&
        std::is_convertible<Other2, Ty2>::value, int>::type = 0>
        constexpr pair(pair<Other1, Other2>&& other)
        : first(tinystl::forward<Other1>(other.first)),
        second(tinystl::forward<Other2>(other.second))
        {
        }

        // explicit constructiable for other pair
        template <class Other1, class Other2, typename std::enable_if<
        std::is_constructible<Ty1, Other1>::value &&
        std::is_constructible<Ty2, Other2>::value &&
        (!std::is_convertible<Other1, Ty1>::value ||
        !std::is_convertible<Other2, Ty2>::value), int>::type = 0>
        explicit constexpr pair(pair<Other1, Other2>&& other)
        : first(tinystl::forward<Other1>(other.first)),
        second(tinystl::forward<Other2>(other.second))
        {
        }

        // copy assign for this pair
        pair& operator=(const pair& rhs)
        {
            if (this != &rhs)
            {
            first = rhs.first;
            second = rhs.second;
            }
            return *this;
        }

        // move assign for this pair
        pair& operator=(pair&& rhs)
        {
            if (this != &rhs)
            {
            first = tinystl::move(rhs.first);
            second = tinystl::move(rhs.second);
            }
            return *this;
        }

        // copy assign for other pair
        template <class Other1, class Other2>
        pair& operator=(const pair<Other1, Other2>& other)
        {
            first = other.first;
            second = other.second;
            return *this;
        }

        // move assign for other pair
        template <class Other1, class Other2>
        pair& operator=(pair<Other1, Other2>&& other)
        {
            first = tinystl::forward<Other1>(other.first);
            second = tinystl::forward<Other2>(other.second);
            return *this;
        }

        // set destructor default
        ~pair() = default;

        void swap(pair& other)
        {
            if (this != &other)
            {
                tinystl::swap(first, other.first);
                tinystl::swap(second, other.second);
            }
        }
    };

    // overloaded comparison operator
    template <class Ty1, class Ty2>
    bool operator==(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
    {
        return lhs.first == rhs.first && lhs.second == rhs.second;
    }

    template <class Ty1, class Ty2>
    bool operator<(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
    {
        return lhs.first < rhs.first || (lhs.first == rhs.first && lhs.second < rhs.second);
    }

    template <class Ty1, class Ty2>
    bool operator!=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
    {
        return !(lhs == rhs);
    }

    template <class Ty1, class Ty2>
    bool operator>(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
    {
        return rhs < lhs;
    }

    template <class Ty1, class Ty2>
    bool operator<=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
    {
        return !(rhs < lhs);
    }

    template <class Ty1, class Ty2>
    bool operator>=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
    {
        return !(lhs < rhs);
    }

    // overload mystl 的 swap
    template <class Ty1, class Ty2>
    void swap(pair<Ty1, Ty2>& lhs, pair<Ty1, Ty2>& rhs)
    {
        lhs.swap(rhs);
    }

    // make two data to be a pair
    template <class Ty1, class Ty2>
    pair<Ty1, Ty2> make_pair(Ty1&& first, Ty2&& second)
    {
        return pair<Ty1, Ty2>(tinystl::forward<Ty1>(first), tinystl::forward<Ty2>(second));
    }

} // namespace tinystl

#endif 