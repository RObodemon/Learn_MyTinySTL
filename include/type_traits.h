// #ifndef #define #endif 防止重复使用, include 嵌套造成
// first.h 中包含了 second.h。 但是在 third.cpp 中include first.h 和 second.h
#ifndef TRYTINYSTL_TYPE_TRAITS_H_
#define TRYTINYSTL_TYPE_TRAITS_H_

// 这个头文件用于提取类型信息
// 模板元基础库

#include<type_traits> // https://github.com/gcc-mirror/gcc/blob/master/libstdc%2B%2B-v3/include/std/type_traits
// type Traits是在编译时获取有关作为模板参数传入的 类型的信息的 一种方式
// https://blog.csdn.net/FlushHip/article/details/82993411
namespace mystl
{
	// https://www.kancloud.cn/digest/stl-sources/177264
	template <class T, T v>
	struct m_integral_constant
	{
		// C++11中的constexpr指定的函数返回值和参数必须要保证是字面值，
		// 而且必须有且只有一行return代码
		// 一旦以上任何元素被constexpr修饰，那么等于说是告诉编译器 
		// “请大胆地将我看成编译时就能得出常量值的表达式去优化我”----https://www.jianshu.com/p/34a2a79ea947
		static constexpr T value = v;
	};

	template<bool b>
	// using 代替typedef，c++11特性
	// using 用法： 别名指定 // 在子类中引用基类的成员 // 命名空间 https://blog.csdn.net/shift_wwx/article/details/78742459
	using m_bool_constant = m_integral_constant<bool, b>; 
	// 这个type_traits并没有用bool或者false来作为判断结果，因为这两个是value，而不是type，
	// 重载函数的时候需要type来区分的。所以，用了两个空的struct，true_type和false_type。
	typedef m_bool_constant<true> m_true_type;
	typedef m_bool_constant<false> m_false_type;


	// is_pair
	// 如果类型是pair, 便会采取 m_true_type, 反之亦然
	// ---- forward declaration begin
	template<class T1,class T2>
	struct pair;
	// ---- forward declaration end

	template<class T>
	struct is_pair : m_false_type 
	{
	};
	 
	template<class T1, class T2>
	struct is_pair<pair<T1, T2>> : m_true_type 
	{ 
	};

	// https://zhuanlan.zhihu.com/p/547313994
	// is_same
	template<typename T,typename U>
	struct is_same :public m_false_type {};

	template<typename T>
	struct is_same<T, T> :public m_true_type {};

	// template<class T, class U, bool=is_same<T,U>::value>
	// struct Foo
	// {
	//     void print()
	//     {
	//         printf("they are different type");
	//     }    
	// };

	// template<class T,class U>
	// struct Foo<T,U,true>
	// {
	//     void print()
	//     {

	//         printf("they are same type");
	//     }
	// };

}
#endif
