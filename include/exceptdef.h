#ifndef TRYTINYSTL_EXCEPTDEF_H_
#define TRYTINYSTL_EXCEPTDEF_H_

#include <stdexcept>
// 包含 C 标准库标头 <assert.h> 并将关联名称添加到 std 命名空间。
// 包含此标头可确保使用 C 标准库标头中的外部链接声明的名称已在 std 命名空间中声明。
#include <cassert>

namespace mystl
{
	// \:换行符，告诉编译器这一行还没有结束
	#define MYSTL_DEBUG(expr) \
	  assert(expr)  
	// length_error:
	// 用作 引发报告尝试生成 对象太长 而难以指定的所有异常 的基类。
	#define THROW_LENGTH_ERROR_IF(expr, what) \
	  if ((expr)) throw std::length_error(what)
	// out_of_range
	// 该类用作抛出的所有异常的基类，以报告超出其有效范围的参数。
	#define THROW_OUT_OF_RANGE_IF(expr, what) \
	  if ((expr)) throw std::out_of_range(what)
	// runtime_error
	// 引发报告  仅在程序执行时 大概可检测的错误  的所有异常 的基类。
	#define THROW_RUNTIME_ERROR_IF(expr, what) \
	  if ((expr)) throw std::runtime_error(what)

} // namepsace mystl

#endif

