#pragma once

#include "Kernel/StlAllocator.h"

#include "Config/Char.h"

#include <string>

namespace Mengine
{
    typedef std::basic_string<WChar, std::char_traits<WChar>, StlAllocator<WChar>> WString;
}