#pragma once

#include "Config/StdDef.h"

namespace Mengine
{
    struct DecoderData
    {
#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
        DecoderData() = default;
        virtual ~DecoderData() = default;
#endif

        void * buffer;
        size_t size;
    };
}