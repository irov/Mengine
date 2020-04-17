#pragma once

#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct CodecDataInfo
    {
#ifdef MENGINE_ASSERTION_DEBUG
        CodecDataInfo() = default;
        virtual ~CodecDataInfo() = default;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    struct CodecOptions
    {
#ifdef MENGINE_ASSERTION_DEBUG
        CodecOptions() = default;
        virtual ~CodecOptions() = default;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
}

