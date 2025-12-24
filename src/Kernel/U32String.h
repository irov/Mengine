#pragma once

#include "Kernel/StlAllocator.h"

#include "Config/Char.h"

#include <string>

namespace Mengine
{
    typedef std::basic_string<Char32, std::char_traits<Char32>, StlAllocator<Char32>> U32String;
}