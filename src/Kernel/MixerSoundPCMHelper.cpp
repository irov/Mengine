#include "MixerSoundPCMHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        namespace Detail
        {
            //////////////////////////////////////////////////////////////////////////
            static int16_t clampPCM16( float _value )
            {
                if( _value > 32767.f )
                {
                    return 32767;
                }

                if( _value < -32768.f )
                {
                    return -32768;
                }

                int16_t value = (int16_t)_value;

                return value;
            }
            //////////////////////////////////////////////////////////////////////////
        }
        //////////////////////////////////////////////////////////////////////////
        float resolveMixerSoundPCM16Sample( const int16_t * _srcFrame, uint32_t _sourceChannels, uint32_t _destinationChannel, uint32_t _destinationChannels )
        {
            if( _sourceChannels == 0 )
            {
                return 0.f;
            }

            if( _sourceChannels == 1 )
            {
                if( _destinationChannels > 2 )
                {
                    if( _destinationChannel >= 2 )
                    {
                        return 0.f;
                    }
                }

                float sample = (float)_srcFrame[0] / 32768.f;

                return sample;
            }

            if( _destinationChannels == 1 )
            {
                int32_t accumulator = 0;

                for( uint32_t channel = 0; channel != _sourceChannels; ++channel )
                {
                    accumulator += _srcFrame[channel];
                }

                float sample = (float)(accumulator / (int32_t)_sourceChannels) / 32768.f;

                return sample;
            }

            if( _destinationChannel >= _sourceChannels )
            {
                return 0.f;
            }

            float sample = (float)_srcFrame[_destinationChannel] / 32768.f;

            return sample;
        }
        //////////////////////////////////////////////////////////////////////////
        int16_t interpolateMixerSoundPCM16Sample( const int16_t * _frame0, const int16_t * _frame1, uint32_t _channels, uint32_t _channel, float _alpha )
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

            int16_t result = Detail::clampPCM16( sample );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
