#ifndef TRYTINYSTL_ALLOCATOR_H_
#define TRYTINYSTL_ALLOCATOR_H_

// 这个头文件包含一个模板类 allocator, 用于管理内存的分配，释放，对象的构造，析构
// 并且取消了内存池
// new opeartor: 分配内存，构造对象
// operator new: 分配内存
// placement new: 构造对象
// new opeartor = operator new + placement new
#include "construct.h"
#include "util.h"

namespace mystl
{
	// 模板类：allocator
	// 模板函数代表数据类型

	template<class T>
	class allocator
	{
	public:
		typedef T         value_type;
		typedef T*		  pointer;
		typedef const T*  const_pointer;
		typedef T&        reference;
		typedef const T&  const_reference;
		typedef size_t    size_type;
		typedef ptrdiff_t difference_type;

		static T* allocate();
		static T* allocate(size_type n);

		static void deallocate(T* ptr);
		static void deallocate(T* ptr, size_type n);

		static void construct(T* ptr);
		static void construct(T* ptr, const T& value);
		static void construct(T* ptr, T&& value);

		template<class... Args>
		static void construct(T* ptr,Args&& ...args);

		static void destroy(T* ptr);
		static void destroy(T* first, T* last);
	};

	template<class T>
	T* allocator<T>::allocate()
	{
		// 分配内存使用operator new
		return static_cast<T*>(::operator new(size0f(T)));
	}
	template<class T>
	T* allocator<T>::allocate(size_type n)
	{
		if (n == 0)
		{
			return nullptr;
		}
		// 使用operator new 分配多个内存
		return static_cast<T*>(::operator new(n * sizeof(T)));
	}

	template<class T>
	void allocator<T>::deallocate(T* ptr)
	{
		if (ptr == nullptr)
		{
			return;
		}
		::operator delete(ptr);
	}

	// 回收内存使用 operator delete
	template<class T>
	void allocator<T>::deallocate(T* ptr, size_type)
	{
		if (ptr == nullptr)
		{
			return;
		}
		::operator delete(ptr);
	}

	template<class T>
	void allocator<T>::construct(T* ptr)
	{
		// 采用placement new
		mystl::construct(ptr);
	}

	template<class T>
	void allocator<T>::construct(T* ptr, const T& value)
	{
		// 见construc.h文件, 
		mystl::construct(ptr, value);
	}

	template<class T>
	void allocator<T>::construct(T* ptr, T&& value)
	{
		mystl::construct(ptr, mystl::move(value));
	}

	template<class T>
	template<class ...Args>
	void allocator<T>::construct(T* ptr, Args&& ...args)
	{
		mystl::construct(ptr, mystl::forward<Args>(args)...);
	}

	template<class T>
	void allocator<T>::destroy(T* ptr)
	{
		mystl::destory(ptr);
	}

	template<class T>
	void allocator<T>::destroy(T* first, T* last)
	{
		// 析构两个迭代器之间的对象
		mystl::destory(first, last);
	}
}
#endif