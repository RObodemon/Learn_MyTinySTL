#ifndef _ASTRING_H_
#define _ASTRING_H_

// string, wstring, u16string, u32string
#include "basic_string.h"

namespace tinystl
{

    using string    = tinystl::basic_string<char>;
    using wstring   = tinystl::basic_string<wchar_t>;
    using u16string = tinystl::basic_string<char16_t>;
    using u32string = tinystl::basic_string<char32_t>;

}
#endif