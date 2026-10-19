#include "AAudioSoundMixer.h"

#include "Kernel/MixerSoundSource.h"
#include "Kernel/SpinLockScope.h"
#include "Kernel/SpinLockTryScope.h"

#include "Config/StdAlgorithm.h"
#include "Config/StdUtility.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AAudioSoundMixer::AAudioSoundMixer()
        : m_stream( nullptr )
        , m_error( AAUDIO_OK )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AAudioSoundMixer::~AAudioSoundMixer()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AAudioSoundMixer::setStream( AAudioStream * _stream )
    {
        MENGINE_SPINLOCK_SCOPE( m_busLock );

        m_stream = _stream;
        m_error = AAUDIO_OK;
    }
    //////////////////////////////////////////////////////////////////////////
    aaudio_result_t AAudioSoundMixer::takeError()
    {
        MENGINE_SPINLOCK_SCOPE( m_busLock );

        aaudio_result_t error = m_error;
        m_error = AAUDIO_OK;

        return error;
    }
    //////////////////////////////////////////////////////////////////////////
    void AAudioSoundMixer::clearSources()
    {
        for( uint32_t index = 0; index != MENGINE_ANDROID_MIXER_INPUT_BUS_COUNT; ++index )
        {
            this->releaseSourceBus( index );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool AAudioSoundMixer::acquireSourceBus( const SoundSourceInterfacePtr & _source, float _gain, uint32_t * const _busIndex )
    {
        MENGINE_SPINLOCK_SCOPE( m_busLock );

        for( uint32_t index = 0; index != MENGINE_ANDROID_MIXER_INPUT_BUS_COUNT; ++index )
        {
            AAudioSoundMixerBusDesc & bus = m_buses[index];

            if( bus.busy == true )
            {
                continue;
            }

            MENGINE_SPINLOCK_SCOPE( bus.lock );

            bus.source = _source;
            bus.gain = _gain > 0.f ? _gain : 0.f;
            bus.busy = true;
            bus.enabled = true;

            *_busIndex = index;

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void AAudioSoundMixer::releaseSourceBus( uint32_t _busIndex )
    {
        if( _busIndex >= MENGINE_ANDROID_MIXER_INPUT_BUS_COUNT )
        {
            return;
        }

        SoundSourceInterfacePtr source = this->detachSourceBus_( _busIndex );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AAudioSoundMixer::setSourceBusVolume( uint32_t _busIndex, float _gain )
    {
        if( _busIndex >= MENGINE_ANDROID_MIXER_INPUT_BUS_COUNT )
        {
            return false;
        }

        m_buses[_busIndex].gain = _gain > 0.f ? _gain : 0.f;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AAudioSoundMixer::enableSourceBus( uint32_t _busIndex, bool _enable )
    {
        if( _busIndex >= MENGINE_ANDROID_MIXER_INPUT_BUS_COUNT )
        {
            return false;
        }

        m_buses[_busIndex].enabled = _enable;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    aaudio_data_callback_result_t AAudioSoundMixer::onAAudioSoundMixerAudioReady( AAudioStream * _stream, void * _audioData, int32_t _frames )
    {
        if( _frames <= 0 )
        {
            return AAUDIO_CALLBACK_RESULT_STOP;
        }

        float * samples = static_cast<float *>(_audioData);
        size_t sampleCount = (size_t)_frames * AAUDIO_MIXER_OUTPUT_CHANNEL_COUNT;
        StdAlgorithm::fill_n( samples, sampleCount, 0.f );

        if( m_stream.load() != _stream )
        {
            return AAUDIO_CALLBACK_RESULT_STOP;
        }

        for( AAudioSoundMixerBusDesc & bus : m_buses )
        {
            if( bus.enabled.load() == false )
            {
                continue;
            }

            SpinLockTryScope busScope( bus.lock );

            if( busScope.isLocked() == false )
            {
                continue;
            }

            if( bus.enabled.load() == false )
            {
                continue;
            }

            if( bus.source == nullptr )
            {
                continue;
            }

            MixerSoundSource * source = static_cast<MixerSoundSource *>(bus.source.get());
            float gain = bus.gain.load();
            uint32_t frames = (uint32_t)_frames;
            source->renderMixerFrames( frames, AAUDIO_MIXER_OUTPUT_CHANNEL_COUNT, samples, gain );
        }

        for( size_t index = 0; index != sampleCount; ++index )
        {
            samples[index] = StdAlgorithm::clamp( samples[index], -1.f, 1.f );
        }

        return AAUDIO_CALLBACK_RESULT_CONTINUE;
    }
    //////////////////////////////////////////////////////////////////////////
    void AAudioSoundMixer::onAAudioSoundMixerAudioError( AAudioStream * _stream, aaudio_result_t _error )
    {
        MENGINE_SPINLOCK_SCOPE( m_busLock );

        if( m_stream.load() != _stream )
        {
            return;
        }

        m_error = _error;
    }
    //////////////////////////////////////////////////////////////////////////
    SoundSourceInterfacePtr AAudioSoundMixer::detachSourceBus_( uint32_t _busIndex )
    {
        MENGINE_SPINLOCK_SCOPE( m_busLock );

        AAudioSoundMixerBusDesc & bus = m_buses[_busIndex];
        MENGINE_SPINLOCK_SCOPE( bus.lock );

        bus.enabled = false;
        bus.gain = 0.f;
        SoundSourceInterfacePtr source = StdUtility::move( bus.source );
        bus.busy = false;

        return source;
    }
    //////////////////////////////////////////////////////////////////////////
}
