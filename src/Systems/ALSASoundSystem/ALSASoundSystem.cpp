#include "ALSASoundSystem.h"

#include "ALSASoundBufferMemory.h"
#include "ALSASoundBufferStream.h"

#include "Interface/ThreadServiceInterface.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/FactoryPoolWithListener.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/SpinLockScope.h"

#include "Config/StdAlgorithm.h"
#include "Config/StdMath.h"

#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cstdint>
#include <vector>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SoundSystem, Mengine::ALSASoundSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static int16_t floatToPCM16( float _sample )
        {
            if( _sample >= 1.f )
            {
                return 32767;
            }

            if( _sample <= -1.f )
            {
                return -32768;
            }

            return (int16_t)(_sample * 32767.f);
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    ALSASoundSystem::ALSASoundSystem()
        : m_pcm( nullptr )
        , m_renderRunning( false )
        , m_renderFailed( false )
        , m_renderFailureReported( false )
        , m_outputSampleRate( 48000 )
        , m_outputChannels( 2 )
        , m_periodFrames( 1024 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ALSASoundSystem::~ALSASoundSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ALSASoundSystem::_initializeService()
    {
        if( this->createAudioMixer_() == false )
        {
            LOGGER_ASSERTION( "failed to create ALSA audio mixer" );

            return false;
        }

        m_factoryALSASoundBuffer = Helper::makeFactoryPoolWithMutex<ALSASoundBufferMemory, 32>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryALSASoundBufferStream = Helper::makeFactoryPoolWithMutex<ALSASoundBufferStream, 32>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryALSASoundSource = Helper::makeFactoryPoolWithMutexAndListener<ALSASoundSource, 32>( this, &ALSASoundSystem::onDestroyALSASoundSource_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ALSASoundSystem::_finalizeService()
    {
        this->destroyAudioMixer_();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryALSASoundBuffer );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryALSASoundBufferStream );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryALSASoundSource );

        m_factoryALSASoundBuffer = nullptr;
        m_factoryALSASoundBufferStream = nullptr;
        m_factoryALSASoundSource = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void ALSASoundSystem::update()
    {
        if( m_renderFailed.load() == true && m_renderFailureReported == false )
        {
            m_renderFailureReported = true;

            LOGGER_ERROR( "ALSA render thread stopped after an unrecoverable device error" );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool ALSASoundSystem::isSilent() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    SoundSourceInterfacePtr ALSASoundSystem::createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _buffer, const DocumentInterfacePtr & _doc )
    {
        ALSASoundSourcePtr soundSource = m_factoryALSASoundSource->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( soundSource, "invalid sound source create" );

        soundSource->setHeadMode( _isHeadMode );
        soundSource->setSoundBuffer( _buffer );

        if( soundSource->initialize() == false )
        {
            LOGGER_ASSERTION( "invalid sound source initialize" );

            return nullptr;
        }

        return soundSource;
    }
    //////////////////////////////////////////////////////////////////////////
    SoundBufferInterfacePtr ALSASoundSystem::createSoundBuffer( const SoundDecoderInterfacePtr & _decoder, bool _isStream, const DocumentInterfacePtr & _doc )
    {
        ALSASoundBufferBasePtr base;

        if( _isStream == false || SERVICE_IS_INITIALIZE( ThreadServiceInterface ) == false )
        {
            ALSASoundBufferMemoryPtr buffer = m_factoryALSASoundBuffer->createObject( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( buffer, "invalid create sound buffer" );

            base = buffer;
        }
        else
        {
            ALSASoundBufferStreamPtr buffer = m_factoryALSASoundBufferStream->createObject( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( buffer, "invalid create sound stream buffer" );

            base = buffer;
        }

        base->setOutputFrequency( m_outputSampleRate );

        if( base->load( _decoder ) == false )
        {
            LOGGER_ASSERTION( "failed to create ALSA sound buffer" );

            return nullptr;
        }

        return base;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ALSASoundSystem::acquireSourceBus( const SoundSourceInterfacePtr & _source, uint32_t _frequency, uint32_t _channels, float _gain, uint32_t * const _busIndex )
    {
        uint32_t busIndex = MENGINE_UNIX_MIXER_INPUT_BUS_COUNT;

        {
            MENGINE_SPINLOCK_SCOPE( m_busLock );

            for( uint32_t index = 0; index != MENGINE_UNIX_MIXER_INPUT_BUS_COUNT; ++index )
            {
                if( m_mixerBuses[index].busy == true )
                {
                    continue;
                }

                m_mixerBuses[index].busy = true;
                busIndex = index;

                break;
            }
        }

        if( busIndex == MENGINE_UNIX_MIXER_INPUT_BUS_COUNT )
        {
            LOGGER_ASSERTION( "no free ALSA mixer bus" );

            return false;
        }

        {
            ALSASoundMixerBusDesc & busDesc = m_mixerBuses[busIndex];
            MENGINE_SPINLOCK_SCOPE( busDesc.lock );

            busDesc.source = _source;
            busDesc.frequency = _frequency;
            busDesc.channels = _channels;
        }

        this->setSourceBusVolume( busIndex, _gain );
        this->enableSourceBus( busIndex, true );

        *_busIndex = busIndex;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ALSASoundSystem::releaseSourceBus( uint32_t _busIndex )
    {
        if( _busIndex >= MENGINE_UNIX_MIXER_INPUT_BUS_COUNT )
        {
            return;
        }

        this->enableSourceBus( _busIndex, false );
        this->setSourceBusVolume( _busIndex, 0.f );

        {
            ALSASoundMixerBusDesc & busDesc = m_mixerBuses[_busIndex];
            MENGINE_SPINLOCK_SCOPE( busDesc.lock );

            busDesc.source = nullptr;
            busDesc.frequency = 0;
            busDesc.channels = 0;
        }

        {
            MENGINE_SPINLOCK_SCOPE( m_busLock );
            m_mixerBuses[_busIndex].busy = false;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool ALSASoundSystem::setSourceBusVolume( uint32_t _busIndex, float _gain )
    {
        if( _busIndex >= MENGINE_UNIX_MIXER_INPUT_BUS_COUNT )
        {
            return false;
        }

        m_mixerBuses[_busIndex].gain = _gain > 0.f ? _gain : 0.f;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ALSASoundSystem::enableSourceBus( uint32_t _busIndex, bool _enable )
    {
        if( _busIndex >= MENGINE_UNIX_MIXER_INPUT_BUS_COUNT )
        {
            return false;
        }

        m_mixerBuses[_busIndex].enabled = _enable;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ALSASoundSystem::createAudioMixer_()
    {
        snd_pcm_t * pcm = nullptr;
        int result = ::snd_pcm_open( &pcm, "default", SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK );

        if( result < 0 )
        {
            LOGGER_ERROR( "failed to open ALSA default playback device: %s"
                , ::snd_strerror( result )
            );

            return false;
        }

        result = ::snd_pcm_set_params( pcm, SND_PCM_FORMAT_S16_LE, SND_PCM_ACCESS_RW_INTERLEAVED, m_outputChannels, m_outputSampleRate, 1, 100000 );

        if( result < 0 )
        {
            LOGGER_ERROR( "failed to configure ALSA playback device: %s"
                , ::snd_strerror( result )
            );

            ::snd_pcm_close( pcm );

            return false;
        }

        snd_pcm_uframes_t bufferFrames = 0;
        snd_pcm_uframes_t periodFrames = 0;
        result = ::snd_pcm_get_params( pcm, &bufferFrames, &periodFrames );

        if( result >= 0 && periodFrames != 0 )
        {
            m_periodFrames = periodFrames;
        }

        m_pcm = pcm;
        m_renderFailed = false;
        m_renderFailureReported = false;
        m_renderRunning = true;

        try
        {
            m_renderThread = StdThread::thread( &ALSASoundSystem::renderThread_, this );
        }
        catch( ... )
        {
            m_renderRunning = false;
            ::snd_pcm_close( m_pcm );
            m_pcm = nullptr;

            LOGGER_ERROR( "failed to create ALSA render thread" );

            return false;
        }

        LOGGER_MESSAGE( "ALSASoundSystem initialized with ALSA device '%s', %u Hz, %u channels, period %lu frames"
            , ::snd_pcm_name( m_pcm )
            , m_outputSampleRate
            , m_outputChannels
            , (unsigned long)m_periodFrames
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ALSASoundSystem::destroyAudioMixer_()
    {
        m_renderRunning = false;

        if( m_pcm != nullptr )
        {
            ::snd_pcm_drop( m_pcm );
        }

        if( m_renderThread.joinable() == true )
        {
            m_renderThread.join();
        }

        if( m_pcm != nullptr )
        {
            ::snd_pcm_close( m_pcm );
            m_pcm = nullptr;
        }

        for( uint32_t index = 0; index != MENGINE_UNIX_MIXER_INPUT_BUS_COUNT; ++index )
        {
            ALSASoundMixerBusDesc & busDesc = m_mixerBuses[index];
            SoundSourceInterfacePtr source;

            busDesc.enabled = false;
            busDesc.gain = 0.f;

            {
                MENGINE_SPINLOCK_SCOPE( busDesc.lock );
                source = busDesc.source;
                busDesc.source = nullptr;
                busDesc.frequency = 0;
                busDesc.channels = 0;
            }

            {
                MENGINE_SPINLOCK_SCOPE( m_busLock );
                busDesc.busy = false;
            }

            source = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ALSASoundSystem::renderThread_()
    {
        const uint32_t outputChannels = m_outputChannels;
        const uint32_t periodFrames = (uint32_t)m_periodFrames;

        std::vector<float> mixBuffer( (size_t)periodFrames * outputChannels );
        std::vector<int16_t> deviceBuffer( (size_t)periodFrames * outputChannels );

        uint32_t pendingOffset = 0;
        uint32_t pendingFrames = 0;

        while( m_renderRunning.load() == true )
        {
            if( pendingFrames == 0 )
            {
                this->mixOutputFrames_( mixBuffer.data(), periodFrames, outputChannels );

                for( size_t index = 0; index != mixBuffer.size(); ++index )
                {
                    deviceBuffer[index] = Detail::floatToPCM16( mixBuffer[index] );
                }

                pendingOffset = 0;
                pendingFrames = periodFrames;
            }

            snd_pcm_sframes_t written = ::snd_pcm_writei( m_pcm, deviceBuffer.data() + (size_t)pendingOffset * outputChannels, pendingFrames );

            if( written > 0 )
            {
                pendingOffset += (uint32_t)written;
                pendingFrames -= (uint32_t)written;

                continue;
            }

            if( written == -EAGAIN || written == 0 )
            {
                ::snd_pcm_wait( m_pcm, 100 );

                continue;
            }

            if( m_renderRunning.load() == false )
            {
                break;
            }

            int recoverResult = ::snd_pcm_recover( m_pcm, (int)written, 1 );

            if( recoverResult >= 0 )
            {
                continue;
            }

            LOGGER_ERROR( "unrecoverable ALSA playback error: %s"
                , ::snd_strerror( recoverResult )
            );

            m_renderFailed = true;

            break;
        }

        m_renderRunning = false;
    }
    //////////////////////////////////////////////////////////////////////////
    void ALSASoundSystem::mixOutputFrames_( float * _mixBuffer, uint32_t _frames, uint32_t _outputChannels )
    {
        StdAlgorithm::fill_n( _mixBuffer, (size_t)_frames * _outputChannels, 0.f );

        for( uint32_t index = 0; index != MENGINE_UNIX_MIXER_INPUT_BUS_COUNT; ++index )
        {
            ALSASoundMixerBusDesc & busDesc = m_mixerBuses[index];

            if( busDesc.enabled.load() == false )
            {
                continue;
            }

            float gain = busDesc.gain.load();

            SoundSourceInterfacePtr source;

            {
                MENGINE_SPINLOCK_SCOPE( busDesc.lock );
                source = busDesc.source;
            }

            if( source == nullptr )
            {
                continue;
            }

            ALSASoundSource * unixSource = static_cast<ALSASoundSource *>(source.get());
            unixSource->renderMixerFrames( _frames, _outputChannels, _mixBuffer, gain );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ALSASoundSystem::onDestroyALSASoundSource_( ALSASoundSource * _soundSource )
    {
        _soundSource->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
}
