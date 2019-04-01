#pragma once

#include "Config/Config.h"

#include "stdex/array_string.h"

namespace Mengine
{
    template<uint32_t Size>
    using ArrayString = stdex::array_string<Size>;
}