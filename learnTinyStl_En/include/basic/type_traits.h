#ifndef TYPE_TRAITS_H_
#define TYPE_TRSITS_H_
// #ifndef #define #endif to prevent reuse
// first.h includes second.h. But in third.cpp include first.h and second.h

// Extract type information
#include<type_traits>

namespace tinystl
{
    template<class T, T v>
    struct m_integral_constant
    {
        // constexpr is the same as constant
        // Unlike const, 
        // constexpr can also be applied to functions and class constructors (constructors).
        // constexpr indicates that the value or return value is constant 
        // and will be evaluated at compile time if possible.
        static constexpr T value = v;
    };
    // using: Alias specification 
    // refers to members of the base class in the subclass 
    // using namespace ...
    template <bool b>
    using m_bool_constant = m_integral_constant<bool, b>;
    // two empty structs
    typedef m_bool_constant<true>  m_true_type;
    typedef m_bool_constant<false> m_false_type;

    // is_pair

    // --- forward declaration begin
    template <class T1, class T2>
    struct pair;
    // --- forward declaration end
    // If the type is pair<T1,T2>, it will go to m_true_type
    // Or it will go to the m_false_type
    // main template
    template <class T>
    struct is_pair : m_false_type {};
    // Partitial Specialization
    template <class T1, class T2>
    struct is_pair<pair<T1, T2>> : m_true_type {};

    // https://zhuanlan.zhihu.com/p/547313994
    // If the type is same, T == T, it will to the m_true_type
    // Or it will go to the m_false_type
    // is_same
    template<typename T, typename U>
    struct is_same:public m_false_type {};

    template<typename T>
    struct is_same<T, T>:public m_true_type{};

    template<class T, class U, bool=is_same<T,U>::value>
    struct Foo
    {
        void print()
        {
            printf("they are different type");
        }    
    };
    template<class T,class U>
    struct Foo<T,U,true>
    {
        void print()
        {

            printf("they are same type");
        }
    };

} 
#endif 
