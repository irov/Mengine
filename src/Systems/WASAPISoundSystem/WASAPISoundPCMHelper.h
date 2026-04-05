#pragma once

#include <stdint.h>

namespace Mengine
{
    namespace Helper
    {
        float resolveWASAPISoundPCM16Sample( const int16_t * _srcFrame, uint32_t _sourceChannels, uint32_t _destinationChannel, uint32_t _destinationChannels );
        int16_t interpolateWASAPISoundPCM16Sample( const int16_t * _frame0, const int16_t * _frame1, uint32_t _channels, uint32_t _channel, float _alpha );
    }
}
