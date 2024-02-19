#pragma once

#include "Kernel/StlAllocator.h"

#include "Config/Char.h"

namespace Mengine
{
    struct Stringz
    {
        const Char * buff;
        size_t size;
    };
}
