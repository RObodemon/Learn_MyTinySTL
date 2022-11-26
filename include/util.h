#ifndef TRYTINYSTL_UTIL_H_
#define TRYTINYSTL_UTIL_H_

// 包含一些通用工具，包括 move, forward, swarp 等函数，以及 pair 等

#include<cstddef>
#include"type_traits.h"

namespace mystl
{
	// move 移动语义
	
	// remove_reference: 移除类型中T的引用
	// std::remove_reference<int&>::type去掉了int&中的引用，结果是int
	template<class T>
	typename std::remove_reference<T>::type&& move(T&& arg) noexcept
	{
		// static_cast: 把 arg 转化成 std::remove_reference<T>::type&&
		return static_cast<typename std::remove_reference<T>::type&&>(arg);;
	}

	// forward
	// 输入的参数是左值，那么传递给下一个函数的参数的也是左值；
	// 输入的参数是右值，那么传递给下一个函数的参数的也是右值
	template<class T>
	T&& forward(typename std::remove_reference<T>::type& arg) noexcept
	{   
		return static_cast<T&&>(arg);;
	}

	template<class T>
	T&& forward(typename std::remove_reference<T>::type&& arg) noexcept
	{
		// static_assert: 编译期间的断言，因此叫做静态断言
		// 如果是左值引用，那么就 "bad forward"
		static_assert(!std::is_lvalue_reference<T>::value, "bad forward";
		return static_cast<T&&>(arg);
	}

	// swap
	template<class Tp>
	void swap(Tp& lhs, Tp& rhs)
	{
		auto tmp(mystl::move(lhs)); // 括号赋值
		lhs = mystl::move(rhs);
		rhs = mystl::move(tmp);
	}

	template<class ForwardIter1, class ForwardIter2>
	ForwardIter2 swap_range(ForwardIter1 first1, ForwardIter1 last1, ForwardIter2 first2)
	{
		for (; first != last1; ++first1, (void)++first2)
		{
			mystl::swap(*first1, *first2);
		}
		return first2;
	}

	template<class Tp, size_t N>
	void swap(Tp(&a)[N], Tp(&b)[N])
	{
		mystl::swap_range(a, a + N, b);
	}

	// ------------------------------------------------------------------------------------
	// pair

	// 结构体模板 : pair
	// 两个模板参数分别表示两个数据的类型
	// 用 first 和 second 来分别取出第一个数据和第二个数据

	template<class Ty1, class Ty2>
	struct pair
	{
		typedef Ty1 first_type;
		typedef Ty2 second_type;
		first_type first; // 保存第一个数据
		second_type second; // 保存第二个数据
	
		// default constructible： 如果类型 T 是具有默认构造函数的类类型，则类型谓词的实例为 true，否则为 false
		// 这里就是判断 Other1,Other2是否具有默认的构造函数，并且把结果赋值给first和second
		template<class Other1 = Ty1, class Other2 = Ty2,
			typename = typename std::enable_if<
			std::is_default_constructible<Other1>::value&&
			std::is_default_constructible<Other2>::value, void>::type> constexpr pair()
			: first(), second()
		{

		}

		// implicit constructible for this type
		
		// is_copy_constructible: 如果类型 U1 是具有复制构造函数的类，则类型谓词的实例为 true；否则为 false。
		// 这里判断U1和U2是否含有拷贝构造函数
		// is_convertible: 判断const U1& 是否可以转换成 Ty1类型
		template<class U1 = Ty1, class U2 = Ty2,
			typename std::enable_if<
			std::is_copy_constructible<U1>::value &&
			std::is_copy_constructible<U2>::value &&
			std::is_convertible<const U1&, Ty1>::value &&
			std::is_convertible<const U2&, Ty2>::value, int>::type = 0> 
		constexpr pair(const Ty1& a, const Ty2& b)
			:first(a), second(b)
		{
		}
		// C++11中的constexpr指定的函数返回值和参数必须要保证是字面值，
		// 而且必须有且只有一行return代码
		// 一旦以上任何元素被constexpr修饰，那么等于说是告诉编译器 
		// “请大胆地将我看成编译时就能得出常量值的表达式去优化我”

		// explicit constructible for this type
		template <class U1 = Ty1, class U2 = Ty2,
			typename std::enable_if<
			std::is_copy_constructible<U1>::value&&
			std::is_copy_constructible<U2>::value &&
			(!std::is_convertible<const U1&, Ty1>::value ||
				!std::is_convertible<const U2&, Ty2>::value), int>::type = 0>
		explicit constexpr pair(const Ty1& a, const Ty2& b)
			: first(a), second(b)
		{
		}

		pair(const pair& rhs) = default;
		pair(pair&& rhs) = default;

		// implicit constructible for other type
		template<class Other1, class Other2,
			typename std::enable_if <
			std::is_constructible<Ty1, Other1>::value&&
			std::is_constructible<Ty2, Other2>::value&&
			std::is_convertible<Other1&&, Ty1>::value&&
			std::is_convertible<Other2&&, Ty2>::value, int>::type = 0>
		constexpr pair(Other1&& a, Other2&& b)
			: first(mystl::forward<Other1>(a)),
			second(mystl::forward<Other2>(b))
		{

		}

		// explicit constructible for other type
		template<class Other1, class Other2,
			typename std::enable_if <
			std::is_constructible<Ty1, Other1>::value&&
			std::is_constructible<Ty2, Other2>::value&&
			std::is_constructible<const Other1&, Ty1>::value&&
			std::is_constructible<const Other2&, Ty2>::value, int>::type = 0>
		constexpr pair(const pair<Other1, Other2>& other)
			: first(mystl::forward<Other1>(a)),
			second(mystl::forward<Other2>(b))
		{
		}

		// implicit constructiable for other pair
		template <class Other1, class Other2,
			typename std::enable_if<
			std::is_constructible<Ty1, const Other1&>::value&&
			std::is_constructible<Ty2, const Other2&>::value&&
			std::is_convertible<const Other1&, Ty1>::value&&
			std::is_convertible<const Other2&, Ty2>::value, int>::type = 0>
		constexpr pair(const pair<Other1, Other2>& other)
			: first(other.first),
			second(other.second)
		{
		}

		// explicit constructible for other pair
		template<class Other1, class Other2,
			typename std::enable_if <
			std::is_constructible<Ty1, const Other1&>::value&&
			std::is_constructible<Ty2, const Other2&>::value &&
			(!std::is_convertible<const Other1&, Ty1>::value ||
				!std::is_convertible<const Other2&, Ty2>::value), int>::type = 0>
		explicit constexpr pair(const pair<Other1, Other2>& other)
			: first(other.first), 
			second(other.second)
		{

		}

		// implicit constructiable for other pair
		template <class Other1, class Other2,
			typename std::enable_if<
			std::is_constructible<Ty1, Other1>::value&&
			std::is_constructible<Ty2, Other2>::value&&
			std::is_convertible<Other1, Ty1>::value&&
			std::is_convertible<Other2, Ty2>::value, int>::type = 0>
		constexpr pair(pair<Other1, Other2>&& other)
			: first(mystl::forward<Other1>(other.first)),
			second(mystl::forward<Other2>(other.second))
		{
		}

		// explicit constructiable for other pair
		template <class Other1, class Other2,
			typename std::enable_if<
			std::is_constructible<Ty1, Other1>::value&&
			std::is_constructible<Ty2, Other2>::value &&
			(!std::is_convertible<Other1, Ty1>::value ||
				!std::is_convertible<Other2, Ty2>::value), int>::type = 0>
		explicit constexpr pair(pair<Other1, Other2>&& other)
			: first(mystl::forward<Other1>(other.first)),
			second(mystl::forward<Other2>(other.second))
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
				first = mystl::move(rhs.first);
				second = mystl::move(rhs.second);
			}
			return *this;
		}

		// copy assign for other pair
		template<class Other1, class Other2>
		pair& operator=(const pair<Other1, Other2>& other)
		{
			first = other.first;
			second = other.second;
			return *this;
		}
		
		// move assign for other pair
		template<class Other1, class Other2>
		pair& operator=(pair<Other1, Other2>&& other)
		{
			first = mystl::forward<Other1>(other.first);
			second = mystl::forward<Other2>(other.second);
			return *this;
		}

		~pair() = default;//通过default关键字让默认析构函数恢复

		void swap(pair& other)
		{
			if (this != &other)
			{
				mystl::swap(first, other.first);
				mystl::swap(second, other.second);
			}
		}

	};

	// 重载比较操作符
	template<class Ty1, class Ty2>
	bool operator==(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
	{
		return lhs.first == rhs.first && lhs.second == rhs.second;
	}

	template<class Ty1, class Ty2>
	bool operator<(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
	{
		return lhs.first < rhs.first&& lhs.second < rhs.second;
	}

	template<class Ty1, class Ty2>
	bool operator!=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
	{
		return !(lhs == rhs);
	}

	template<class Ty1,class Ty2>
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

	// 重载 mystl 的 swap
	template<class Ty1, class Ty2>
	void swap(pair<Ty1, Ty2>& lhs, pair<Ty1, Ty2>& rhs)
	{
		lhs.swap(rhs);
	}

	// 全局函数，让两个数据成为一个pair
	template<class Ty1, class Ty2>
	pair<Ty1, Ty2> make_pair(Ty1&& first, Ty2&& second)
	{
		return pair<Ty1, Ty2>(mystl::forward<Ty1>(first), mystl::forward<Ty2>(second));
	}

}

#endif // !TRYTINYSTL_UTIL_H_