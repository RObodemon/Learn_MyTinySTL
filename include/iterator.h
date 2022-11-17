#ifndef TRYTINYSTL_ITERATOR_H_
#define TRYTINYSTL_ITERATOR_H_

// iterator 对数据进行读写操作，是算法与容器之间的桥梁

#include <cstddef>
#include "type_traits.h"

namespace mystl
{
	// 5 种类迭代器
	struct input_iterator_tag  // 对数据只读访问
	{
	}; 
	struct output_iterator_tag // 对数据只写访问
	{
	}; 
	struct forward_iterator_tag :public input_iterator_tag // 提供读写，并且向前推进
	{
	}; 
	struct bidirectional_iterator_tag :public forward_iterator_tag // 提供读写，可向前向后 
	{
	}; 
	struct random_access_iterator_tag :public bidirectional_iterator_tag // 提供读写，在数据中前后移动
	{
	}; 

	// iterator 模板
	template <class Category, class T, class Distance = ptrdiff_t, class Pointer = T*, class Reference = T&>
	struct iterator
	{
		typedef Category  iterator_category; // 迭代器类型
		typedef T         value_type;        // 迭代器所指对象的类型
		typedef Pointer   pointer;			 // 指针
		typedef Reference reference;		 // 引用
		typedef Distance  difference_type;	 // 两个迭代器之间的距离
	};

	// iterator traits
	template <class T>
	struct has_iterator_cat
	{
	private:
		struct two 
		{ 
			char a; 
			char b; 
		};
		// used for test
		template <class U> 
		static two test(...);

		template <class U> 
		static char test(typename U::iterator_category *= 0); 

	public:
		static const bool value = sizeof(test<T>(0) == sizeof(char));
	};

	template<class Iterator, bool>
	struct iterator_traits_impl 
	{
	};

	template<class Iterator>
	struct iterator_traits_impl<Iterator, true>
	{
		// typedef创建了存在类型的别名，而typename告诉编译器这就是类型， std::vector<T>::size_type是一个类型而不是一个成员。
		// 再次包装
		// typename主要用于对嵌套依赖类型进行提取(萃取).
		// 使用typename对参数类型的提取(萃取), 并且对参数类型在进行一次命名, 
		// 看上去对参数类型的使用有了一层间接性。
		typedef typename Iterator::iterator_category iterator_category;
		typedef typename Iterator::value_type        value_type;
		typedef typename Iterator::pointers          pointer;
		typedef typename Iterator::reference         reference;
		typedef typename Iterator::difference_type   difference_type;
	};

	template <class Iterator, bool>
	struct iterator_traits_helper {};

	template<class Iterator>
	// std::is_convertible模板用于检查是否可以将任何数据类型A隐式转换为任何数据类型B。
	// 它返回布尔值true或false。
	// 相当于：<Iterator, bool||bool>
	struct iterator_traits_helper<Iterator, true> :public iterator_traits_impl<Iterator,
		std::is_convertible<typename Iterator::iterator_category, input_iterator_tag>::value ||
		std::is_convertible<typename Iterator::iterator_category, output_iterator_tag>::value>
	{
	};

	// 萃取处迭代器的特性
	template<class Iterator>
	struct iterator_traits
		:public iterator_traits_helper<Iterator, has_iterator_cat<iteartor>::value> 
	{
	};

	// 针对原生指针的偏特化版本
	template<class T>
	struct iterator_traits<T*>
	{
		typedef random_access_iterator_tag  iterator_category;
		typedef T                           value_type;
		typedef T*							pointer;
		typedef T&							reference;
		typedef ptrdiff_t                   difference_type;
	};

	template<class T>
	struct iterator_traits<const T*>
	{
		typedef random_access_iterator_tag  iterator_category;
		typedef T                           value_type;
		typedef const T*					pointer;
		typedef const T&					reference;
		typedef ptrdiff_t					difference_type;
	};

	template<class T, class U, bool = has_iterator_cat<iterator_traits<T>>::value>
	struct has_iterator_cat_of
		:public m_bool_constant<std::is_convertible<typename iterator_traits<T>::iteator_category, U>::value>
	{};

	// 萃取迭代器
	template <class T, class U>
	struct has_iterator_cat_of<T, U, false> : public m_false_type 
	{
	};

	template <class Iter>
	struct is_input_iterator :public has_iterator_cat_of<Iter, input_iterator_tag> 
	{
	};

	template<class Iter>
	struct is_output_iterator :public has_iterator_cat_of<Iter, output_iterator_tag> 
	{
	};

	template<class Iter>
	struct is_bidirectional_iterator :public has_iterator_cat_of<Iter, bidirectional_iterator_tag> 
	{
	};

	template<class Iter>
	struct is_forward_iterator : public has_iterator_cat_of<Iter, forward_iterator_tag> 
	{
	};

	template<class Iter>
	struct is_random_access_iterator :public has_iterator_cat_of<Iter, random_access_iterator_tag> 
	{
	};

	template<class Iterator>
	struct is_iterator :
		public m_bool_constant<is_input_iterator<Iterator>::value
		|| is_output_iterator<Iterator>::value>
	{};


	// 萃取迭代器的类型 category
	template<class Iterator>
	typename iterator_traits<Iterator>::iterator_category
		iterator_category(const Iterator&)
	{
		typedef typename iterator_traits<Iterator>::iterator_category Category;
		return Category();
	}

	/*
	　	const_cast，        字面上理解就是去const属性。
		static_cast，       命名上理解是静态类型转换。如int转换成char。用于类层次结构中基类和子类之间指针或引用的转换
		dynamic_cast，      命名上理解是动态类型转换。如子类和父类之间的多态类型转换。
		reinterpreter_cast，仅仅重新解释类型，但没有进行二进制的转换。
	*/
	// 萃取迭代器的 distance_type
	template<class Iterator>
	typename iterator_traits<Iterator>::difference_type*
		distance_type(const Iterator&)
	{
		return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
	}

	// 萃取某个迭代器的 value_type
	template <class Iterator>
	typename iterator_traits<Iterator>::value_type*
		value_type(const Iterator&)
	{
		return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
	}

	/***********************************************************************************************/
	// 以下函数用于计算迭代器间的距离
	// distance 的 input_iterrator_tag 的版本
	template<class InputIterator>
	typename iterator_traits<InputIterator>::difference_type
		distance_dispatch(InputIterator first, InputIterator last, input_iterator_tag)
	{
		typename iterator_traits<InputIterator>::difference_type n = 0;
		while (first != last)
		{
			++first;
			++n;
		}
		return n;
	}

	// distance 的 random_access_iterator_tag的版本
	template<class RandomIter>
	typename iterator_traits<RandomIter>::difference_type
		distance_dispatch(RandomIter first, RandomIter last, random_access_iterator_tag)
	{
		return last - first;
	}

	template<class InputIterator>
	typename iterator_traits<InputIterator>::difference_type
		distance(InputIterator first, InputIterator last)
	{
		return distance_dispatch(first, last, iterator_category(first));
	}

	/*********************************************************************/
	// 以下函数用于让迭代器前进 n 个距离
	// advance 的 input_iterator_tag 的版本
	template<class InputIterator, class Distance>
	void advance_dispatch(InputIterator& i, Distance n, input_iterator_tag)
	{
		while (n--)
		{
			// 单向迭代器
			++i;
		}
	}

	// advance 的 bidirectional_iterator_tag 的版本
	// 让迭代器向前或者向后移动 n 个距离
	template<class BidirectionalIterator, class Distance>
	void advance_dispatch(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag)
	{
		//  支持双向的移动且支持多次读写操作.
		if (n >= 0)
			while (n--)
				++i;
		else
			while (n++)
				--i;
	}

	// advance 的 random_access_iterator_tag 的版本
	template<class RandomIter, class Distance>
	void advance_dispatch(RandomIter& i, Distance n, random_access_iterator_tag)
	{	
		//random_access_iterator 支持双向移动且支持多次读写操作. p+n, p-n等.
		i += n;
	}

	// 使用iterator_category()萃取迭代器类型，调用不同的版本的 advance_dispatch()函数
	template <class InputIterator, class Distance>
	void advance(InputIterator& i, Distance n)
	{
		advance_dispatch(i, n, iterator_category(i));
	}

	/*****************************************************************************/
	// 模板类： reverse_iterator
	// 这里维护的是一个正向迭代器，不过执行反向操作，++为--，<为>
	// 代表反向iterator
	template<class Iterator>
	class reverse_iterator
	{
	private:
		Iterator current; // 记录对应的正向迭代器
	public:
		// 反向迭代器的五种相应类型
		// 使用typename对参数类型的提取(萃取), 并且对参数类型在进行一次命名, 
		// 看上去对参数类型的使用有了一层间接性。
		typedef typename iterator_traits<Iterator>::iterator_category Iterator_category;
		typedef typename iterator_traits<Iterator>::value_type        value_type;
		typedef typename iterator_traits<Iterator>::difference_type   difference_type;
		typedef typename iterator_traits<Iterator>::pointer           pointer;
		typedef typename iterator_traits<Iterator>::reference         reference;

		typedef Iterator                                              iterator_type;
		typedef reverse_iterator<Iterator>                            self;

	public:
		// Constructor
		reverse_iterator()
		{
		}
		// explicit构造函数是用来防止隐式转换的
		explicit reverse_iterator(iterator_type i) :current(i)
		{
		}
		// 拷贝构造
		reverse_iteratorq(const self& rhs) :current(rhs.current)
		{
		}

	public:
		// 取出对应的正向迭代器
		iterator_type base() const
		{
			return current;
		}

		// 重载运算符
		// 类的成员函数后面加 const，表明这个函数不会对这个类对象的数据成员
		//（准确地说是非静态数据成员）作任何改变。
		reference operator*() const
		{
			// 实际对应正向迭代器的前一个位置
			auto tmp = current;
			return *--tmp;
		}
		pointer operator->() const
		{
			return &(operator*());
		}

		// 前进(++) 变为 后退(--)
		self& operator++()
		{
			--current;
			return *this;
		}
		// int: 表示重载是 后++
		self operator++(int)
		{
			self tmp = *this;
			--current;
			return tmp;
		}
		// 后退(--) 变为 前进(++)
		self& operator--()
		{
			++current;
			return *this;
		}
		self operator--(int)
		{
			self tmp = *this;
			++current;
			return tmp;
		}

		self& operator+=(difference_type n)
		{
			current -= n;
			return *this;
		}
		self operator+(difference_type n) const
		{
			return self(current - n);
		}
		self& operator-=(difference_type n)
		{
			current += n;
			return *this;
		}
		self operator-(difference_type n) const
		{
			return self(current + n);
		}
		reference operator[](difference_type n)const
		{
			return *(*this + n);
		}
	};
	// 重载 operator-
	template <class Iterator>
	typename reverse_iterator<Iterator>::difference_type
		operator-(const reverse_iterator<Iterator>& lhs, const reverse_iterator<Iterator>& rhs)
	{
		return rhs.base() - lhs.base();
	}

	// 重载比较操作符
	template<class Iterator>
	bool operator==(const reverse_iterator<Iterator>& lhs, const reverse_iterator<Iterator>& rhs)
	{
		return lhs.base() == rhs.base();
	}

	template<class Iterator>
	bool operator<(const reverse_iterator<Iterator>& lhs, const reverse_iterator<Iterator>& rhs)
	{
		return rhs.base() < lhs.base()
	}

	template<class Iterator>
	bool operator!=(const reverse_iterator<Iterator>& lhs, const reverse_iterator<Iterator>& rhs)
	{
		return !(lhs == rhs);
	}

	template<class Iterator>
	bool operator>(const reverse_iterator<Iterator>& lhs, const reverse_iterator<Iterator>& rhs)
	{
		return rhs < lhs;
	}

	template <class Iterator>
	bool operator<=(const reverse_iterator<Iterator>& lhs,
		const reverse_iterator<Iterator>& rhs)
	{
		return !(rhs < lhs);
	}

	template <class Iterator>
	bool operator>=(const reverse_iterator<Iterator>& lhs,
		const reverse_iterator<Iterator>& rhs)
	{
		return !(lhs < rhs);
	}

	
}
#endif 