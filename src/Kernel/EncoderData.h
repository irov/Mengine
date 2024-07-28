#pragma once

#include "Config/StdDef.h"

namespace Mengine
{
    struct EncoderData
    {
#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
        EncoderData() = default;
        virtual ~EncoderData() = default;
#endif

        const void * buffer;
        size_t size;
    };
}