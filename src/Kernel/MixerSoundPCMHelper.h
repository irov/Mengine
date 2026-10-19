#pragma once

#include "Config/StdInt.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        float resolveMixerSoundPCM16Sample( const int16_t * _srcFrame, uint32_t _sourceChannels, uint32_t _destinationChannel, uint32_t _destinationChannels );
        int16_t interpolateMixerSoundPCM16Sample( const int16_t * _frame0, const int16_t * _frame1, uint32_t _channels, uint32_t _channel, float _alpha );
        //////////////////////////////////////////////////////////////////////////
    }
}
