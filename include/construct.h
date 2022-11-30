#ifndef TRYTINYSTL_CONSTRUCT_H_
#define TRYTINYSTL_CONSTRUCT_H_

// 这个头文件包含两个函数 const	ruct, destory
// construct : 负责对象的构造 destory : 负责对象的析构
// https://github.com/gcc-mirror/gcc/blob/16e2427f50c208dfe07d07f18009969502c25dc8/libstdc%2B%2B-v3/include/bits/stl_construct.h
#include <new>
#include "type_traits.h"
#include "iterator.h"

#ifdef _MSC_VER
// #pragma warning(push)是保存当前的编译器警告状态；
// #pragma warning(pop)是恢复原先的警告状态。
#pragma warning(push)
#pragma warning(disable : 4100)  // unused parameter, 这里的意思是在编译的时候，warning 4100 不会出现
#endif // _MSC_VER

// C++中的new，至少代表以下三种含义：new operator、operator new、placement new。
// https://blog.csdn.net/u010318270/article/details/78608244
// placement new用来实现定位、构造。在取得了一块可以容纳指定类型对象的内存后，在这块内存上构造一个对象。
// 必须引用头文件<new>或<new.h>才能使用placement new。必须显示的调用析构函数
namespace mystl
{
	/***********************************************************************************************/
	// construct 构造对象
	template<class Ty>
	void construct(Ty* ptr)
	{
		// 放置操作符，用于初始化一块已经存在的内存 
		// ::作用域符号，这里是全局作用域符
		::new ((void*)ptr) Ty(); // p为Raw（原生）内存地址
	}

	template<class Ty1,class Ty2>
	void construct(Ty1* ptr, const Ty2& value)
	{
		::new((void*)ptr) Ty1(value); // p为Raw（原生）内存地址，value为存储内容
	}

	// 1,参数个数可变时候 使用 class... Args: 这个参数包中可以包含0到任意个模板参数；
	// 2,... 在模板定义的右边，可以将参数包展开成一个一个独立的参数。
	template<class Ty, class... Args>
	void construct(Ty* ptr, Args&&...args)
	{
		::new((void*)ptr) Ty(mystl::forward<Args>(args)...);
	}

	/***********************************************************************************************/
	// destory 将对象析构
	template<class Ty>
	void destory_one(Ty*, std::true_type)
	{
	}

	template<class Ty>
	void destroy_one(Ty* pointer, std::false_type)
	{
		if (pointer != nullptr)
		{
			// 在设计函数的时候，需要自己设计析构函数释放和摧毁内存
			pointer->~Ty(); // 调用对象析构函数摧毁对象
		}
	}

	template<class ForwardIter>
	void destroy_cat(ForwardIter, ForwardIter, std::true_type)
	{
	}

	template<class ForwardIter>
	void destroy_cat(ForwardIter first, ForwardIter last, std::false_type)
	{
		for (; first != last; ++first)
		{
			destory(&*first);
		}
	}

	// is_constructible 判断一个类型是不是可以用指定参数集构造的类型
	// is_trivial 判断一个类型是否是一个平凡的类型
	// is_trivially_destructible 判断一个类型是否是一个平凡的可销毁类型
	// is_nothrow_destructible 判断一个类型是否是可析构类型，并且不抛出任何异常
	template<class Ty>
	void destory(Ty* pointer)
	{
		// 如果通过使用 Args 中的参数类型可普通构造类型 T，则类型谓词的实例保持 true；否则保持 false。 
		destroy_one(pointer, std::is_trivially_constructible<Ty>{});
	}

	template<class ForwardIter>
	void destroy(ForwardIter first, ForwardIter last)
	{
		// 如果类型 T 是易损坏类型，且编译器已知此析构函数不会使用任何重要操作
		destroy_cat(first, last, std::is_trivially_destructible<
			typename iterator_traits<ForwardIter>::value_type>{});
	}
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

#endif // !TRYTINYSTL_CONSTRUCT_H_
