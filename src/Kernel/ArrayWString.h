#pragma once

#include "Kernel/ArrayTString.h"

namespace Mengine
{
    template<uint32_t Size>
    using ArrayWString = ArrayTString<WChar, Size>;
}