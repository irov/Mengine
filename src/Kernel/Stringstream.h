#pragma once

#include "Kernel/StlAllocator.h"

#include "Config/Char.h"

#include <sstream>

namespace Mengine
{
    typedef std::basic_stringstream<Char, std::char_traits<Char>, StlAllocator<Char> > Stringstream;
    typedef std::basic_stringstream<WChar, std::char_traits<WChar>, StlAllocator<WChar> > WStringstream;

    typedef std::basic_istringstream<Char, std::char_traits<Char>, StlAllocator<Char> > IStringstream;
    typedef std::basic_istringstream<Char, std::char_traits<Char>, StlAllocator<Char> > IWStringstream;
}