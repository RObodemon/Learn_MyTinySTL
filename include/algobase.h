 #ifndef TRYTINYSTL_ALGOBASE_H_
#define TRYTINYSTL_ALGOBASE_H_

// 一些基本算法

#include <cstring>
// <cstring>是包含一些C字符串的操作函数，包含一些常用的C字符串处理函数，
// 比如strcmp、strlen、strcpy之类的函数与原来的<string.h>对应。但头文件的内容在名字空间std 中。
// <string > 包含的是C++的string类。
#include"iterator.h"
#include"util.h"

namespace mystl
{
#ifdef max
// 在编译窗口显示宏定义，一种提示，这里是显示定义了 max 这个宏
#pragma message("#undefing marco max");
#undef max
#endif // max

#ifdef min
#pragma message("undefing marco min");
#undef min
#endif // min

	/*************************************************************/
	// max
	// 取最大值，语义相等时保证返回第一个参数
	template<class T>
	const T& max(const T& lhs, const T& rhs) // 返回变量为常量，输入变量不可修改
	{
		return lhs < rhs ? rhs : lhs;
	}

	// 重载，comp仿函数代替比较操作
	// 其实这里的重载我觉得没啥必要，毕竟comp()可以使用其他逻辑
	template<class T, class Compare>
	const T& max(const T& lhs, const T& rhs, Compare comp)
	{
		return comp(lhs, rhs) ? rhs : lhs;
	}

	/***********************************************************************************************/
	// min
	// 取二者中较小的值，语义相等时返回第一个参数
	template<class T>
	const T& min(const T& lhs, const T& rhs)
	{
		return rhs < lhs ? rhs : lhs;
	}

	// 重载版本使用函数对象 comp 代替比较操作
	template<class T,class Compare>
	const T& min(const T& lhs, const T& rhs, Compare comp)
	{
		return comp(rhs, lhs) ? rhs : lhs;
	}

	/************************************************************/
	// iter_swap
	// 将两个迭代器所指对象对调
	template<class FIter1,class FIter2>
	void iter_swap(FIter1 lhs, FIter2 rhs)
	{
		mystl::swap(*lhs, *rhs); // in util.h
	}
	
	/*********************************************************/
	// copy
	// 把 [first,last] 区间内的元素拷贝到 [result,result+(last-first)]内
	// input_iterator_tag 版本
	template<class InputIter,class OutputIter>
	OutputIter unchecked_copy_cat(InputIter first, InputIter last, OutputIter result,
		mystl::input_iterator_tag)
	{
		for (; first != last; ++first, ++result)
		{
			*result = *first;
		}
		return result;
	}

	// random_access_iterator_tag 版本
	template<class RandomIter,class OutputIter>
	OutputIter unchecked_copy_cat(RandomIter first, RandomIter last, OutputIter result, 
		mystl::random_access_iterator_tag)
	{
		for (auto n = last - first; n > 0; --n, ++first, ++result)
		{
			*result = *first;
		}
		return result;
	}

	template<class InputIter,class OutputIter>
	OutputIter unchecked_copy(InputIter first, InputIter last, OutputIter result)
	{
		return unchecked_copy_cat(first, last, result, iterator_category(first));
	}

	// 为 trivialy_copy_assignable 类型提供特化版本, 判断类型是否具备拷贝构造运算符
	// remove_const: 从类型创建非 const 类型
	// memmove: 将一个缓冲区移到另一个缓冲区，安全版本 memmove_s
	template<class Tp, class Up> 
	typename std::enable_if<
		std::is_same<typename std::remove_const<Tp>::type,Up>::value &&
		std::is_trivially_copy_assignable<Up>::value,Up*>::type
		unchecked_copy(Tp* first, Tp* last, Up* result)
	{
		const auto n = static_cast<size_t>(last - first);
		if (n != 0)
		{
			std::memmove(result, first, n * sizeof(Up));
		}
		return result + n;
	}

	template<class InputIter,class OutputIter>
	OutputIter copy(InputIter first, InputIter last, OutputIter result)
	{
		return unchecked_copy(first, last, result);
	}

	/*********************************************************************/
	// copy_backward
	// 将[first,last) 区间内的元素拷贝到 [result-(last-fisrt),result) 内
	// unchecked_copy_backward_cat 的 bidirecitional_iterator_tag 版本
	template<class BidirectionalIter1,class BidirectionalIter2>
	BidirectionalIter2 unchecked_copy_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
		BidirectionalIter2 result, mystl::bidirectional_iterator_tag)
	{
		while (first != last)
		{
			*--result = *--last;
		}
		return result;
	}

	// unchecked_copy_backward_cat 的 random_access_iterator_tag 版本
	template<class BidirectionalIter1,class BidirectionalIter2>
	BidirectionalIter2 unchecked_copy_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
		BidirectionalIter2 result, mystl::random_access_iterator_tag)
	{
		for (auto n = last - first; n > 0; --n)
		{
			*--result = *--last;
		}
		return result;
	}

	template<class BidirectionalIter1,class BidirectionalIter2>
	BidirectionalIter2 unchecked_copy_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
		BidirectionalIter2 result)
	{
		return unchecked_copy_backward_cat(first, last, result, iterator_category(first));
	}

	// 为 trivially_copy_assignable 类型提供特化版本
	template<class Tp,class Up>
	typename std::enable_if <
		std::is_same<typename std::remove_const<Tp>::type, Up>::value&&
		std::is_trivially_copy_assignable<Up>::value,
		Up*>::type unchecked_copy_backward(Tp* first, Tp* last, Up* result)
	{
		const auto n = static_cast<size_t>(last - first);
		if (n != 0)
		{
			result -= n;
			std::memmove(result, first, n * sizeof(Up));
		}
		return result;
	}

	template<class BidirectionalIter1,class BidirectionalIter2>
	BidirectionalIter2 copy_backward(BidirectionalIter1 first, BidirectionalIter1 last,
		BidirectionalIter2 result)
	{
		return unchecked_copy_backward(first, last, result);
	}

	/*****************************************************************************************/
	// copy_if
	// 把[first,last) 内满足一元操作 unary_pred 的元素拷贝到以 result 为起始的位置上

	template<class InputIter,class OutputIter,class UnaryPredicate>
	OutputIter copy_if(InputIter first, InputIter last, OutputIter result, UnaryPredicate unary_pred)
	{
		for (; first != last; ++first)
		{
			if (unary_pred(*first))
			{
				*result++ = *first;
			}
		}
		return result;
	}

	/***************************************************************************************/
	// copy_n
	// 把 [first,first+n)区间上的元素拷贝到[result,result+n)上
	// 返回一个pair分别指向拷贝结束的尾部
	template<class InputIter, class Size,class OutputIter>
	mystl::pair<InputIter,OutputIter>
		unchecked_copy_n(InputIter first, Size n, OutputIter result, mystl::input_iterator_tag)
	{
		for (; n > 0; --n, ++first, ++result)
		{
			*result = *first;
		}
		return mystl::pair<InputIter, OutputIter>(first, result);
	}

	template<class RandomIter,class Size,class OutputIter>
	mystl::pair<RandomIter,OutputIter>
		unchecked_copy_n(RandomIter first, Size n, OutputIter result, mystl::random_access_iterator_tag)
	{
		auto last = first + n;
		return mystl::pair<RandomIter, OutputIter>(last, mystl::copy(first, last, result));
	}

	template<class InputIter,class Size,class OutputIter>
	mystl::pair<InputIter,OutputIter>
		copy_n(InputIter first, Size n, OutputIter result)
	{
		return unchecked_copy_n(first, n, result, iterator_category(first));
	}

	/**********************************************************************************/
	// move
	// 把 [first last)区间内的元素移动到 [result,result+(last-first))内
	// input_iterator_tag 版本
	template<class InputIter, class OutputIter>
	OutputIter unchecked_move_cat(InputIter first, InputIter last, OutputIter result,
		mystl::input_iterator_tag)
	{
		for (; first != last; ++first, ++result)
		{
			*result = mystl::move(*first);
		}
		return result;
	}

	// random_access_iterator_tag 版本
	template<class RandomIter,class OutputIter>
	OutputIter unchecked_move_cat(RandomIter first, RandomIter last, OutputIter result,
		mystl::random_access_iterator_tag)
	{
		for (auto n = last - first; n > 0; --n, ++first, ++result)
		{
			*result = mystl::move(*first);
		}
		return result;
	}

	template<class InputIter, class OutputIter>
	OutputIter unchecked_move(InputIter first, InputIter last, OutputIter result)
	{
		return unchecked_move_cat(first, last, result, iterator_category(first));
	}

	// 为trivially_copy_assignable 类型提供特化版本, 判断类是否具备移动拷贝运算符
	template<class Tp,class Up>
	typename std::enable_if<
		std::is_same<typename std::remove_const<Tp>::type,Up>::value &&
		std::is_trivially_move_assignable<Up>::value,Up*>::type
		unchecked_move(Tp* first, Tp* last, Up* result)
	{
		const size_t n = static_cast<size_t>(last - first);
		if (n != 0)
		{
			std::memmove(result, first, n * sizeof(Up));
		}
		return result + n;
	}

	template<class InputIter,class OutputIter>
	OutputIter move(InputIter first, InputIter last, OutputIter result)
	{
		return unchecked_move(first, last, result);
	}

	/***************************************************************************************/
	// move_backward
	// 将 [first,last) 区间内的元素移动到 [result-(last-first),result) 内
	// bidirectional_iterator_tag 版本
	template<class BidirectionalIter1,class BidirectionalIter2>
	BidirectionalIter2 unchecked_move_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
		BidirectionalIter2 result, mystl::bidirectional_iterator_tag)
	{
		while (first != last)
		{
			*--result = mystl::move(*--last);
		}
		return result;
	}

	// random_access_iterator_tag 版本
	template<class RandomIter1,class RandomIter2>
	RandomIter2 unchecked_move_backward_cat(RandomIter1 first, RandomIter1 last,
		RandomIter2 result, mystl::random_access_iterator_tag)
	{
		for (auto n = last - first; n > 0; --n)
		{
			*--result = mystl::move(*--last);
		}
		return result;
	}

	template<class BidirectionalIter1, class BidirectionalIter2>
	BidirectionalIter2 unchecked_move_backward(BidirectionalIter1 first, BidirectionalIter1 last,
		BidirectionalIter2 result)
	{
		return unchecked_move_backward_cat(first, last, result, iterator_category(first));
	}

	// 为 trivially_copy_assignable 类型提供特化版本
	template<class Tp,class Up>
	typename std::enable_if <
		std::is_same<typename std::remove_const<Tp>::type,Up>::value &&
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

	template<class BidirectionalIter1,class BidirectionalIter2>
	BidirectionalIter2 move_backward(BidirectionalIter1 first, BidirectionalIter1 last,
		BidirectionalIter2 result)
	{
		return unchecked_move_backward(first, last, result);
	}

	/************************************************************************************/
	// equal
	// 比较第一序列 [first, last) 区间上的元素值得是否和第二序列相等
	template<class InputIter1,class InputIter2>
	bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2)
	{
		for (; first != last1; ++first1, ++first2)
		{
			if (*first != *first2)
			{
				return false;
			}
		}
		return true;
	}

	// 重载版本使用函数对象 comp 代替比较操作
	template<class InputIter1,class InputIter2,class Compared>
	bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2, Compared comp)
	{
		for (; first1 != last1; ++first1, ++first2)
		{
			if (!comp(*first1, *first2))
			{
				return false;
			}
		}
		return true;
	}

	/***********************************************************************************/
	// fill_n
	// 从 first 位置开始填充 n 个值，如果大于一个字节调用循环赋值的函数，否则调用one_byte类型，并且嗲用memset()
	template<class OutputIter,class Size,class T>
	OutputIter unchecked_fill_n(OutputIter first, Size n, const T& value)
	{
		for (; n > 0; --n, ++first)
		{
			*first = value;
		}
		return first;
	}

	// 为 one-byte 类型提供特化版本
	// is_same: 判断两个类型是否相同
	// is_integral: 判断类型是否为整型
	template<class Tp,class Size,class Up>
	typename std::enable_if<
		std::is_integral<Tp>::value&&sizeof(Tp)==1 &&
		!std::is_same<Tp,bool>::value &&
		std::is_integral<Up>::value && sizeof(Up)==1,
		Tp*>::type
		unchecked_fill_n(Tp* first, Size n, Up value)
	{
		if (n > 0)
		{
			std::memset(first, (unsigned char)value, (size_t)(n));
		}
		return first + n;
	}

	template<class OutputIter,class Size,class T>
	OutputIter fill_n(OutputIter first, Size n, const T& value)
	{
		return unchecked_fill_n(first, n, value);
	}

	/*************************************************************************************/
	// fill
	// 为 [first, last) 区间内的所有元素填充新值
	template<class ForwardIter,class T>
	void fill_cat(ForwardIter first, ForwardIter last, const T& value,
		mystl::forward_iterator_tag)
	{
		for (; first != last; ++first)
		{
			*first = value;
		}
	}

	template<class RandomIter,class T>
	void fill_cat(RandomIter first, RandomIter last, const T& value,
		mystl::random_access_iterator_tag)
	{
		fill_n(first, last - first, value);
	}

	template<class ForwardIter,class T>
	void fill(ForwardIter first, ForwardIter last, const T& value)
	{
		fill_cat(first, last, value, iterator_category(first));
	}

	/***********************************************************************************/
	// lexicographical_compare
	// 以字典序列对两个序列进行比较，当在某个位置发现第一组不相等元素时，有下列集中情况：
	// (1) 如果第一序列的元素较小，返回 true，否则返回 false
	// (2) 如果到达 last1 而尚未达到 last2 返回 true
	// (3) 如果达到 last2 而尚未到达 last1 返回 false
	// (4) 如果同时到达 last1 和 last2 返回 false
	template<class InputIter1,class InputIter2>
	bool lexicographical_compare(InputIter1 first1, InputIter1 last1,
		InputIter2 first2, InputIter2 last2)
	{
		for (; first != last1 && first2 != last2; ++first1, ++first2)
		{
			if (*first1 < *first2)
			{
				return true;
			}
			if (*first2 < *first1)
			{
				return false;
			}
		}
		return first1 == last1 && first2 != last2;
	}

	// 重载版本使用函数对象 comp 代替比较操作
	template<class InputIter1,class InputIter2,class Compred>
	bool lexicographical_compare(InputIter1 first1, InputIter1 last,
		InputIter2 first2, InputIter2 last2, Compred comp)
	{
		for (; first != last1 && first2 != last2; += first1, +=first2)
		{
			if (comp(*first1, *first2))
			{
				return true;
			}
			if (com(*first2, *first1))
			{
				return false;
			}
		}
		return first1 == last1 && first2 != last2;
	}

	// 针对 const unsigned char* 的特化版本
	bool lexicographical_compare(const unsigned char* first1,
		const unsigned char* last1,
		const unsigned char* first2,
		const unsigned char* last2)
	{
		const auto len1 = last1 - first1;
		const auto len2 = last2 - first2;
		// 先比较相同长度的部分
		const auto result = memcmp(first1, first2, mystl::min(len1, len2));
		// 若相等，长度较大的比较大
		return result != 0 ? result < 0 : len1 < len2;
	}

	/*****************************************************************************************/
	// mismatch
	// 平行比较两个序列，找到第一处失配的元素，返回一对迭代器，分别指向两个序列中失配的元素
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

	// 重载版本使用函数对象 comp 代替比较操作
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

}	 // namespace mystl
#endif TRYTINYSTL_ALGOBASE_H_