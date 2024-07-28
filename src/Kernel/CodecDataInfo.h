#pragma once

#include "Config/Config.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct CodecDataInfo
    {
#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
        CodecDataInfo() = default;
        virtual ~CodecDataInfo() = default;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
}