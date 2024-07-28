#pragma once

#include "Config/Char.h"
#include "Config/StdDef.h"

namespace Mengine
{
    struct AccountUID
    {
        typedef Char value_type;
        static constexpr size_t size_data = 20U;

        value_type data[AccountUID::size_data];
    };
}