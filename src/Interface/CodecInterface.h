#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/Assertion.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct CodecDataInfo
    {
#ifdef MENGINE_ASSERTION_DEBUG_ENABLE
        CodecDataInfo() = default;
        virtual ~CodecDataInfo() = default;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
}

