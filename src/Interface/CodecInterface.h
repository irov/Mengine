#pragma once

#include "Config/Config.h"

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

