#include "AAudioSoundSystem.h"

#include "Interface/ThreadServiceInterface.h"

#include "AAudioSoundMixer.h"

#include "Kernel/MixerSoundBufferMemory.h"
#include "Kernel/MixerSoundBufferStream.h"
#include "Kernel/MixerSoundSource.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/FactoryPoolWithListener.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/TimestampHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SoundSystem, Mengine::AAudioSoundSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static aaudio_data_callback_result_t dataCallback( AAudioStream * _stream, void * _userData, void * _audioData, int32_t _frames )
        {
            AAudioSoundMixer * mixer = static_cast<AAudioSoundMixer *>(_userData);
            aaudio_data_callback_result_t result = mixer->onAAudioSoundMixerAudioReady( _stream, _audioData, _frames );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static void errorCallback( AAudioStream * _stream, void * _userData, aaudio_result_t _error )
        {
            AAudioSoundMixer * mixer = static_cast<AAudioSoundMixer *>(_userData);
            mixer->onAAudioSoundMixerAudioError( _stream, _error );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    const uint32_t OUTPUT_FREQUENCY = 48000;
    const Timestamp STREAM_REOPEN_DELAY = 1000;
    //////////////////////////////////////////////////////////////////////////
    AAudioSoundSystem::AAudioSoundSystem()
        : m_stream( nullptr )
        , m_reopenTimestamp( 0 )
        , m_startRequested( false )
        , m_active( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AAudioSoundSystem::~AAudioSoundSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AAudioSoundSystem::_initializeService()
    {
        m_mixer = Helper::makeFactorableUnique<AAudioSoundMixer>( MENGINE_DOCUMENT_FACTORABLE );

        if( this->openStream_() == false )
        {
            m_mixer = nullptr;

            return false;
        }

        m_factorySoundBuffer = Helper::makeFactoryPoolWithMutex<MixerSoundBufferMemory, 32>( MENGINE_DOCUMENT_FACTORABLE );
        m_factorySoundBufferStream = Helper::makeFactoryPoolWithMutex<MixerSoundBufferStream, 32>( MENGINE_DOCUMENT_FACTORABLE );
        m_factorySoundSource = Helper::makeFactoryPoolWithMutexAndListener<MixerSoundSource, 32>( this, &AAudioSoundSystem::onDestroyMixerSoundSource_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AAudioSoundSystem::_finalizeService()
    {
        this->closeStream_();
        m_mixer->clearSources();
        m_mixer = nullptr;

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factorySoundBuffer );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factorySoundBufferStream );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factorySoundSource );

        m_factorySoundBuffer = nullptr;
        m_factorySoundBufferStream = nullptr;
        m_factorySoundSource = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void AAudioSoundSystem::update()
    {
        aaudio_result_t error = m_mixer->takeError();

        if( error != AAUDIO_OK )
        {
            LOGGER_ERROR( "AAudio output error: %s, reopening stream"
                , AAudio_convertResultToText( error )
            );

            this->closeStream_();
        }

        if( m_stream == nullptr )
        {
            if( m_active == false )
            {
                return;
            }

            Timestamp now = Helper::getSystemTimestamp();

            if( now < m_reopenTimestamp )
            {
                return;
            }

            if( this->openStream_() == false )
            {
                m_reopenTimestamp = now + STREAM_REOPEN_DELAY;

                return;
            }
        }

        this->updateStreamState_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AAudioSoundSystem::isSilent() const
    {
        if( m_stream == nullptr )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    SoundSourceInterfacePtr AAudioSoundSystem::createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _buffer, const DocumentInterfacePtr & _doc )
    {
        MixerSoundSourcePtr source = m_factorySoundSource->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( source, "invalid sound source create" );

        source->setHeadMode( _isHeadMode );
        source->setSoundBuffer( _buffer );

        if( source->initialize() == false )
        {
            return nullptr;
        }

        return source;
    }
    //////////////////////////////////////////////////////////////////////////
    SoundBufferInterfacePtr AAudioSoundSystem::createSoundBuffer( const SoundDecoderInterfacePtr & _decoder, bool _streamable, const DocumentInterfacePtr & _doc )
    {
        MixerSoundBufferBasePtr buffer;

        if( _streamable == false )
        {
            buffer = m_factorySoundBuffer->createObject( _doc );
        }
        else if( SERVICE_IS_INITIALIZE( ThreadServiceInterface ) == false )
        {
            buffer = m_factorySoundBuffer->createObject( _doc );
        }
        else
        {
            buffer = m_factorySoundBufferStream->createObject( _doc );
        }

        MENGINE_ASSERTION_MEMORY_PANIC( buffer, "invalid sound buffer create" );

        buffer->setOutputFrequency( OUTPUT_FREQUENCY );

        if( buffer->load( _decoder ) == false )
        {
            return nullptr;
        }

        return buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AAudioSoundSystem::acquireSourceBus( const SoundSourceInterfacePtr & _source, uint32_t _frequency, uint32_t _channels, float _gain, uint32_t * const _busIndex )
    {
        if( _frequency != OUTPUT_FREQUENCY )
        {
            return false;
        }

        if( _channels == 0 )
        {
            return false;
        }

        if( _channels > AAUDIO_MIXER_OUTPUT_CHANNEL_COUNT )
        {
            return false;
        }

        if( m_mixer->acquireSourceBus( _source, _gain, _busIndex ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AAudioSoundSystem::releaseSourceBus( uint32_t _busIndex )
    {
        m_mixer->releaseSourceBus( _busIndex );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AAudioSoundSystem::setSourceBusVolume( uint32_t _busIndex, float _gain )
    {
        if( m_mixer->setSourceBusVolume( _busIndex, _gain ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AAudioSoundSystem::enableSourceBus( uint32_t _busIndex, bool _enable )
    {
        if( m_mixer->enableSourceBus( _busIndex, _enable ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AAudioSoundSystem::_onApplicationDidBecomeActive()
    {
        m_active = true;
        this->update();
    }
    //////////////////////////////////////////////////////////////////////////
    void AAudioSoundSystem::_onApplicationWillEnterForeground()
    {
        this->_onApplicationDidBecomeActive();
    }
    //////////////////////////////////////////////////////////////////////////
    void AAudioSoundSystem::_onApplicationDidEnterBackground()
    {
        this->_onApplicationWillResignActive();
    }
    //////////////////////////////////////////////////////////////////////////
    void AAudioSoundSystem::_onApplicationWillResignActive()
    {
        m_active = false;
        this->updateStreamState_();
    }
    //////////////////////////////////////////////////////////////////////////
    void AAudioSoundSystem::_onApplicationWillTerminate()
    {
        m_active = false;
        this->closeStream_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AAudioSoundSystem::openStream_()
    {
        AAudioStreamBuilder * builder = nullptr;
        aaudio_result_t result = AAudio_createStreamBuilder( &builder );

        if( result != AAUDIO_OK )
        {
            LOGGER_ERROR( "failed to create AAudio stream builder: %s"
                , AAudio_convertResultToText( result )
            );

            return false;
        }

        AAudioSoundMixer * mixer = m_mixer.get();

        AAudioStreamBuilder_setDirection( builder, AAUDIO_DIRECTION_OUTPUT );
        AAudioStreamBuilder_setPerformanceMode( builder, AAUDIO_PERFORMANCE_MODE_LOW_LATENCY );
        AAudioStreamBuilder_setSharingMode( builder, AAUDIO_SHARING_MODE_SHARED );
        AAudioStreamBuilder_setFormat( builder, AAUDIO_FORMAT_PCM_FLOAT );
        AAudioStreamBuilder_setChannelCount( builder, AAUDIO_MIXER_OUTPUT_CHANNEL_COUNT );
        AAudioStreamBuilder_setSampleRate( builder, OUTPUT_FREQUENCY );
        AAudioStreamBuilder_setDataCallback( builder, &Detail::dataCallback, mixer );
        AAudioStreamBuilder_setErrorCallback( builder, &Detail::errorCallback, mixer );

        result = AAudioStreamBuilder_openStream( builder, &m_stream );
        AAudioStreamBuilder_delete( builder );

        if( result != AAUDIO_OK )
        {
            LOGGER_ERROR( "failed to open AAudio output: %s"
                , AAudio_convertResultToText( result )
            );

            this->closeStream_();

            return false;
        }

        int32_t sampleRate = AAudioStream_getSampleRate( m_stream );
        int32_t channelCount = AAudioStream_getChannelCount( m_stream );
        aaudio_format_t format = AAudioStream_getFormat( m_stream );

        if( sampleRate != OUTPUT_FREQUENCY )
        {
            LOGGER_ERROR( "invalid AAudio output sample rate: %d"
                , sampleRate
            );

            this->closeStream_();

            return false;
        }

        if( channelCount != AAUDIO_MIXER_OUTPUT_CHANNEL_COUNT )
        {
            LOGGER_ERROR( "invalid AAudio output channel count: %d"
                , channelCount
            );

            this->closeStream_();

            return false;
        }

        if( format != AAUDIO_FORMAT_PCM_FLOAT )
        {
            LOGGER_ERROR( "invalid AAudio output format: %d"
                , format
            );

            this->closeStream_();

            return false;
        }

        m_mixer->setStream( m_stream );
        m_startRequested = false;
        m_reopenTimestamp = 0;

        LOGGER_MESSAGE( "AAudioSoundSystem initialized with %d Hz, %d channels"
            , sampleRate
            , channelCount
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AAudioSoundSystem::closeStream_()
    {
        m_mixer->setStream( nullptr );

        if( m_stream != nullptr )
        {
            AAudioStream_requestStop( m_stream );
            AAudioStream_close( m_stream );
            m_stream = nullptr;
        }

        m_startRequested = false;
    }
    //////////////////////////////////////////////////////////////////////////
    void AAudioSoundSystem::updateStreamState_()
    {
        if( m_stream == nullptr )
        {
            return;
        }

        bool active = m_active;

        if( active == m_startRequested )
        {
            return;
        }

        // Refresh asynchronous state transitions without blocking the engine thread.
        aaudio_stream_state_t state;
        aaudio_result_t result = AAudioStream_waitForStateChange( m_stream, AAUDIO_STREAM_STATE_UNKNOWN, &state, 0 );

        if( result != AAUDIO_OK )
        {
            AAudioStream * stream = m_stream;
            m_mixer->onAAudioSoundMixerAudioError( stream, result );

            return;
        }

        if( active == true )
        {
            if( state == AAUDIO_STREAM_STATE_STARTING )
            {
                return;
            }

            if( state == AAUDIO_STREAM_STATE_STOPPING )
            {
                return;
            }
        }

        result = active == true ? AAudioStream_requestStart( m_stream ) : AAudioStream_requestStop( m_stream );

        if( result != AAUDIO_OK )
        {
            AAudioStream * stream = m_stream;
            m_mixer->onAAudioSoundMixerAudioError( stream, result );

            return;
        }

        m_startRequested = active;
    }
    //////////////////////////////////////////////////////////////////////////
    void AAudioSoundSystem::onDestroyMixerSoundSource_( MixerSoundSource * _source )
    {
        _source->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
}
