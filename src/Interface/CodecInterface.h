#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/Assertion.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct CodecDataInfo
    {
#if MENGINE_ASSERTION_DEBUG
        CodecDataInfo() = default;
        virtual ~CodecDataInfo() = default;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    struct CodecOptions
    {
#if MENGINE_ASSERTION_DEBUG
        CodecOptions() = default;
        virtual ~CodecOptions() = default;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
}

