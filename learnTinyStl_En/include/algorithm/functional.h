#ifndef FUNCTIONAL_H_
#define FUNCTIONAL_H_

// function objects(functor) and hash functions

#include <cstddef>

namespace tinystl
{

    // Parameter types and return value types of unary functions
    template <class Arg, class Result>
    struct unarg_function
    {
        typedef Arg     argument_type;
        typedef Result  result_type;
    };

    // Parameter types and return value types of binary functions
    template <class Arg1, class Arg2, class Result>
    struct binary_function
    {
        typedef Arg1    first_argument_type;
        typedef Arg2    second_argument_type;
        typedef Result  result_type;
    };

    // functor: plus+
    template <class T>
    struct plus :public binary_function<T, T, T>
    {
        T operator()(const T& x, const T& y) const 
        { 
            return x + y; 
        }
    };

    // functor: minus-
    template <class T>
    struct minus :public binary_function<T, T, T>
    {
        T operator()(const T& x, const T& y) const 
        { 
            return x - y; 
        }
    };

    // multiply*
    template <class T>
    struct multiplies :public binary_function<T, T, T>
    {
        T operator()(const T& x, const T& y) const 
        { 
            return x * y; 
        }
    };

    // Division /
    template <class T>
    struct divides :public binary_function<T, T, T>
    {
        T operator()(const T& x, const T& y) const 
        { 
            return x / y; 
        }
    };

    // remainder %
    template <class T>
    struct modulus :public binary_function<T, T, T>
    {
        T operator()(const T& x, const T& y) const 
        { 
            return x % y; 
        }
    };

    // negative
    template <class T>
    struct negate :public unarg_function<T, T>
    {
        T operator()(const T& x) const 
        { 
            return -x; 
        }
    };

    // The identity element of addition is 0
    // The identity element of multiplication is 1
    template <class T>
    T identity_element(plus<T>) 
    { 
        return T(0); 
    }
    template <class T>
    T identity_element(multiplies<T>) 
    { 
        return T(1); 
    }

    // =
    template <class T>
    struct equal_to :public binary_function<T, T, bool>
    {
        bool operator()(const T& x, const T& y) const 
        { 
            return x == y; 
        }
    };

    // !=
    template <class T>
    struct not_equal_to :public binary_function<T, T, bool>
    {
        bool operator()(const T& x, const T& y) const 
        { 
            return x != y; 
        }
    };

    // >
    template <class T>
    struct greater :public binary_function<T, T, bool>
    {
        bool operator()(const T& x, const T& y) const 
        { 
            return x > y;
        }
    };

    // <
    template <class T>
    struct less :public binary_function<T, T, bool>
    {
        bool operator()(const T& x, const T& y) const 
        { 
            return x < y; 
        }
    };

    // >=
    template <class T>
    struct greater_equal :public binary_function<T, T, bool>
    {
        bool operator()(const T& x, const T& y) const 
        { 
            return x >= y; 
        }
    };

    // <=
    template <class T>
    struct less_equal :public binary_function<T, T, bool>
    {
        bool operator()(const T& x, const T& y) const 
        { 
            return x <= y; 
        }
    };

    // &&
    template <class T>
    struct logical_and :public binary_function<T, T, bool>
    {
        bool operator()(const T& x, const T& y) const 
        { 
            return x && y; 
        }
    };

    // ||
    template <class T>
    struct logical_or :public binary_function<T, T, bool>
    {
        bool operator()(const T& x, const T& y) const 
        { 
            return x || y; 
        }
    };

    // !
    template <class T>
    struct logical_not :public unarg_function<T, bool>
    {
        bool operator()(const T& x) const 
        { 
            return !x; 
        }
    };

    // Calling this function on any value will not change
    template <class T>
    struct identity :public unarg_function<T, bool>
    {
        const T& operator()(const T& x) const 
        { 
            return x; 
        }
    };

    // input: pair; return: first element
    template <class Pair>
    struct selectfirst :public unarg_function<Pair, typename Pair::first_type>
    {
        const typename Pair::first_type& operator()(const Pair& x) const
        {
            return x.first;
        }
    };

    // input: pair; return: second element
    template <class Pair>
    struct selectsecond :public unarg_function<Pair, typename Pair::second_type>
    {
        const typename Pair::second_type& operator()(const Pair& x) const
        {
            return x.second;
        }
    };

    // Accepts a pair type, ignores the second element and returns the first element 
    // or ignores the first element and returns the second element
    template <class Arg1, class Arg2>
    struct projectfirst :public binary_function<Arg1, Arg2, Arg1>
    {
        Arg1 operator()(const Arg1& x, const Arg2&) const 
        { 
            return x; 
        }
    };
    template <class Arg1, class Arg2>
    struct projectsecond :public binary_function<Arg1, Arg2, Arg1>
    {
        Arg2 operator()(const Arg1&, const Arg2& y) const 
        { 
            return y; 
        }
    };

    //---------------------------------------------------------------------------------
    // hash functor

    // For most types, the hash function does nothing
    template <class Key>
    struct hash 
    {};

    // A partial specialization for pointers
    template <class T>
    struct hash<T*>
    {
        size_t operator()(T* p) const noexcept
        { 
            // Allows conversion of any pointer to any other pointer type.
            // Also allows conversion of any integer type to any pointer type and vice versa.
            // vonverse T* to size_t
            return reinterpret_cast<size_t>(p); 
        }
    };

    // For integral types, return the original value
    // There are reference macro definitions,
    // The function of the backslash(\) is to tell the compiler 
    // that a line break is required here (it is used more in macro definitions and printf())
    #define MYSTL_TRIVIAL_HASH_FCN(Type)           \
    template <> struct hash<Type>                  \
    {                                              \
        size_t operator()(Type val) const noexcept \
        { return static_cast<size_t>(val);}        \
    };

    MYSTL_TRIVIAL_HASH_FCN(bool)

    MYSTL_TRIVIAL_HASH_FCN(char)

    MYSTL_TRIVIAL_HASH_FCN(signed char)

    MYSTL_TRIVIAL_HASH_FCN(unsigned char)

    MYSTL_TRIVIAL_HASH_FCN(wchar_t)

    MYSTL_TRIVIAL_HASH_FCN(char16_t)

    MYSTL_TRIVIAL_HASH_FCN(char32_t)

    MYSTL_TRIVIAL_HASH_FCN(short)

    MYSTL_TRIVIAL_HASH_FCN(unsigned short)

    MYSTL_TRIVIAL_HASH_FCN(int)

    MYSTL_TRIVIAL_HASH_FCN(unsigned int)

    MYSTL_TRIVIAL_HASH_FCN(long)

    MYSTL_TRIVIAL_HASH_FCN(unsigned long)

    MYSTL_TRIVIAL_HASH_FCN(long long)

    MYSTL_TRIVIAL_HASH_FCN(unsigned long long)

    // #define----#undef
    // Make a macro definition, 
    // terminate the macro definition (terminate the scope of the macro)
    #undef MYSTL_TRIVIAL_HASH_FCN

    // For floating point numbers, bitwise hash
    inline size_t bitwise_hash(const unsigned char* first, size_t count)
    {
        // used to determine the compiler version (here is _GUNC_)
        #if (_MSC_VER && _WIN64) || ((__GNUC__ || __clang__) &&__SIZEOF_POINTER__ == 8)
        const size_t fnv_offset = 14695981039346656037ull;
        const size_t fnv_prime = 1099511628211ull;
        #else
        const size_t fnv_offset = 2166136261u;
        const size_t fnv_prime = 16777619u;
        #endif

        size_t result = fnv_offset;
        for (size_t i = 0; i < count; ++i)
        {
            // XOR operator (^) 0^0=0; 0^1=1; 1^0=1; 1^1=0;
            result ^= (size_t)first[i];
            result *= fnv_prime;
        }
        return result;
    }

    template <>
    struct hash<float>
    {
        size_t operator()(const float& val)
        { 
            // If val is not 0.0f, perform a bitwise hash
            return val == 0.0f ? 0 : bitwise_hash((const unsigned char*)&val, sizeof(float));
        }
    };

    template <>
    struct hash<double>
    {
        size_t operator()(const double& val)
        {
            return val == 0.0f ? 0 : bitwise_hash((const unsigned char*)&val, sizeof(double));
        }
    };

    template <>
    struct hash<long double>
    {
        size_t operator()(const long double& val)
        {
            return val == 0.0f ? 0 : bitwise_hash((const unsigned char*)&val, sizeof(long double));
        }
    };

} // namespace tinystl
#endif //
