#pragma once

#include "Config/Char.h"

#include "stdex/stl_allocator.h"

#include <string>

namespace Mengine
{
    typedef std::basic_string<Char, std::char_traits<Char>, stdex::stl_allocator<Char> > String;
    typedef std::basic_string<WChar, std::char_traits<WChar>, stdex::stl_allocator<WChar> > WString;
    typedef std::basic_string<Char32, std::char_traits<Char32>, stdex::stl_allocator<Char32> > U32String;
}
