#pragma once

#include <AudioToolbox/AudioToolbox.h>
#include <stdint.h>

namespace Mengine
{
    namespace Helper
    {
        Float32 resolveAppleSoundPCM16Sample( const int16_t * _srcFrame, uint32_t _sourceChannels, UInt32 _destinationChannel, UInt32 _destinationChannels );
    }
}