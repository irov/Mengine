#pragma once

#include "Config/StdInt.h"

namespace Mengine
{
    typedef uint32_t UniqueId;

    static constexpr UniqueId INVALID_UNIQUE_ID = 0U;
    static constexpr UniqueId REMOVE_UNIQUE_ID = ~0U;
}