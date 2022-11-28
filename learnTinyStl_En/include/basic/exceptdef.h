#ifndef TINYSTL_EXCEPTDEF_H_
#define TINYSTL_EXCEPTDEF_H_

#include <stdexcept>

#include <cassert>

namespace mystl
{
    // \： Represents a newline character,
    //    telling the compiler that the line has not ended
    #define MYSTL_DEBUG(expr) \
    assert(expr)  

    // length_error:
    // The class serves as the base class for all exceptions thrown to
    // report an attempt to generate an object too long to be specified.
    #define THROW_LENGTH_ERROR_IF(expr, what) \
    if ((expr)) throw std::length_error(what)

    // out_of_range:
    // The class serves as the base class for all exceptions thrown to 
    // report an argument that is out of its valid range.
    #define THROW_OUT_OF_RANGE_IF(expr, what) \
    if ((expr)) throw std::out_of_range(what)

    // runtime_error:
    // The class serves as the base class for all exceptions thrown to 
    // report errors presumably detectable only when the program executes.
    #define THROW_RUNTIME_ERROR_IF(expr, what) \
    if ((expr)) throw std::runtime_error(what)

} // namepsace mystl

#endif 

