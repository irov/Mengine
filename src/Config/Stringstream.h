#pragma once

#include "Config/Char.h"

#include "stdex/stl_allocator.h"

#include <sstream>

namespace Mengine
{
    typedef std::basic_stringstream<Char, std::char_traits<Char>, stdex::stl_allocator<Char> > Stringstream;
    typedef std::basic_stringstream<WChar, std::char_traits<WChar>, stdex::stl_allocator<WChar> > WStringstream;
}