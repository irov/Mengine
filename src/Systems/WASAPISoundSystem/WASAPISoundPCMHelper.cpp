#include "WASAPISoundPCMHelper.h"

namespace Mengine
{
    namespace Helper
    {
        namespace Detail
        {
            static int16_t clampPCM16_( float _value )
            {
                if( _value > 32767.f )
                {
                    return 32767;
                }

                if( _value < -32768.f )
                {
                    return -32768;
                }

                return (int16_t)_value;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        float resolveWASAPISoundPCM16Sample( const int16_t * _srcFrame, uint32_t _sourceChannels, uint32_t _destinationChannel, uint32_t _destinationChannels )
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

                return (float)(accumulator / (int32_t)_sourceChannels) / 32768.f;
            }

            uint32_t sourceChannel = _destinationChannel;

            if( sourceChannel >= _sourceChannels )
            {
                sourceChannel = _sourceChannels - 1;
            }

            return (float)_srcFrame[sourceChannel] / 32768.f;
        }
        //////////////////////////////////////////////////////////////////////////
        int16_t interpolateWASAPISoundPCM16Sample( const int16_t * _frame0, const int16_t * _frame1, uint32_t _channels, uint32_t _channel, float _alpha )
        {
            if( _channels == 0 )
            {
                return 0;
            }

            uint32_t channel = _channel;

            if( channel >= _channels )
            {
                channel = _channels - 1;
            }

            float sample0 = (float)_frame0[channel];
            float sample1 = (float)_frame1[channel];
            float sample = sample0 + (sample1 - sample0) * _alpha;

            return Detail::clampPCM16_( sample );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
