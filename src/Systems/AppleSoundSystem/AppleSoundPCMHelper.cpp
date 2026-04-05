#include "AppleSoundPCMHelper.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        Float32 resolveAppleSoundPCM16Sample( const int16_t * _srcFrame, uint32_t _sourceChannels, UInt32 _destinationChannel, UInt32 _destinationChannels )
        {
            if( _sourceChannels == 0 )
            {
                return 0.f;
            }

            if( _destinationChannels == 1 && _sourceChannels > 1 )
            {
                int32_t accumulator = 0;

                for( uint32_t channel = 0; channel != _sourceChannels; ++channel )
                {
                    accumulator += _srcFrame[channel];
                }

                return (Float32)(accumulator / (int32_t)_sourceChannels) / 32768.f;
            }

            UInt32 sourceChannel = _destinationChannel;

            if( sourceChannel >= _sourceChannels )
            {
                sourceChannel = _sourceChannels - 1;
            }

            return (Float32)_srcFrame[sourceChannel] / 32768.f;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}