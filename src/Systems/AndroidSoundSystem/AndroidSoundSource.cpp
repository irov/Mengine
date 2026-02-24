#include "AndroidSoundSource.h"

#include "AndroidSoundSystem.h"

#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"

#include <math.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidSoundSource::AndroidSoundSource()
        : m_soundSystem( nullptr )
        , m_volume( 1.f )
        , m_loop( false )
        , m_headMode( true )
        , m_playing( false )
        , m_pausing( false )
        , m_position( 0.f )
        , m_needRestart( false )
        , m_playerObject( nullptr )
        , m_playerPlay( nullptr )
        , m_playerBufferQueue( nullptr )
        , m_playerVolume( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidSoundSource::~AndroidSoundSource()
    {
        this->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundSource::initialize( AndroidSoundSystem * _soundSystem )
    {
        m_soundSystem = _soundSystem;

        m_soundSystem->registerSoundSource( this );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundSource::finalize()
    {
        this->destroyPlayer_();

        if( m_soundSystem != nullptr )
        {
            m_soundSystem->unregisterSoundSource( this );
            m_soundSystem = nullptr;
        }

        m_soundBuffer = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundSource::play()
    {
        if( m_playing == true )
        {
            return true;
        }

        if( m_soundBuffer == nullptr )
        {
            LOGGER_ASSERTION( "AndroidSoundSource invalid play without buffer" );

            return false;
        }

        if( m_pausing == false )
        {
            if( this->createPlayer_() == false )
            {
                return false;
            }

            uint32_t bits = m_soundBuffer->getBits();
            uint32_t channels = m_soundBuffer->getChannels();

            uint32_t bytesPerSample = (bits / 8) * channels;

            size_t offset = 0;

            if( m_position > 0.f && bytesPerSample > 0 )
            {
                uint32_t frequency = m_soundBuffer->getFrequency();
                size_t frame = (size_t)(m_position * 0.001f * frequency);
                offset = frame * bytesPerSample;
            }

            if( this->enqueueBuffer_( offset ) == false )
            {
                LOGGER_ERROR( "AndroidSoundSource invalid enqueue buffer" );

                this->destroyPlayer_();

                return false;
            }

            this->applyVolume_();
            this->applyLooping_();

            SLresult result_play = ( *m_playerPlay )->SetPlayState( m_playerPlay, SL_PLAYSTATE_PLAYING );

            if( result_play != SL_RESULT_SUCCESS )
            {
                LOGGER_ERROR( "AndroidSoundSource invalid play state [%d]", result_play );

                this->destroyPlayer_();

                return false;
            }
        }
        else
        {
            SLresult result_play = ( *m_playerPlay )->SetPlayState( m_playerPlay, SL_PLAYSTATE_PLAYING );

            if( result_play != SL_RESULT_SUCCESS )
            {
                LOGGER_ERROR( "AndroidSoundSource invalid resume state [%d]", result_play );

                return false;
            }
        }

        m_playing = true;
        m_pausing = false;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundSource::stop()
    {
        if( m_playing == false && m_pausing == false )
        {
            return;
        }

        if( m_playerPlay != nullptr )
        {
            ( *m_playerPlay )->SetPlayState( m_playerPlay, SL_PLAYSTATE_STOPPED );
        }

        this->destroyPlayer_();

        m_playing = false;
        m_pausing = false;
        m_position = 0.f;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundSource::pause()
    {
        if( m_playing == false )
        {
            return;
        }

        if( m_pausing == true )
        {
            return;
        }

        if( m_playerPlay == nullptr )
        {
            return;
        }

        SLresult result_state = ( *m_playerPlay )->SetPlayState( m_playerPlay, SL_PLAYSTATE_PAUSED );

        if( result_state != SL_RESULT_SUCCESS )
        {
            LOGGER_ERROR( "AndroidSoundSource invalid pause state [%d]", result_state );

            return;
        }

        SLmillisecond position = 0;
        SLresult result_position = ( *m_playerPlay )->GetPosition( m_playerPlay, &position );

        if( result_position == SL_RESULT_SUCCESS )
        {
            m_position = (float)position;
        }

        m_playing = false;
        m_pausing = true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundSource::resume()
    {
        if( m_pausing == false )
        {
            return false;
        }

        if( m_playerPlay == nullptr )
        {
            return false;
        }

        SLresult result_state = ( *m_playerPlay )->SetPlayState( m_playerPlay, SL_PLAYSTATE_PLAYING );

        if( result_state != SL_RESULT_SUCCESS )
        {
            LOGGER_ERROR( "AndroidSoundSource invalid resume state [%d]", result_state );

            return false;
        }

        m_playing = true;
        m_pausing = false;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundSource::isPlay() const
    {
        return m_playing;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundSource::isPause() const
    {
        return m_pausing;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundSource::setVolume( float _volume )
    {
        m_volume = _volume;

        this->applyVolume_();
    }
    //////////////////////////////////////////////////////////////////////////
    float AndroidSoundSource::getVolume() const
    {
        return m_volume;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundSource::setLoop( bool _loop )
    {
        m_loop = _loop;

        this->applyLooping_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundSource::getLoop() const
    {
        return m_loop;
    }
    //////////////////////////////////////////////////////////////////////////
    float AndroidSoundSource::getDuration() const
    {
        if( m_soundBuffer == nullptr )
        {
            LOGGER_ASSERTION( "AndroidSoundSource invalid get duration without buffer" );

            return 0.f;
        }

        return m_soundBuffer->getDuration();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundSource::setPosition( float _position )
    {
        if( m_playing == true )
        {
            return false;
        }

        m_position = _position;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    float AndroidSoundSource::getPosition() const
    {
        if( m_playerPlay != nullptr && m_playing == true )
        {
            SLmillisecond position = 0;
            SLresult result = ( *m_playerPlay )->GetPosition( m_playerPlay, &position );

            if( result == SL_RESULT_SUCCESS )
            {
                return (float)position;
            }
        }

        return m_position;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundSource::setSoundBuffer( const SoundBufferInterfacePtr & _soundBuffer )
    {
        if( m_soundBuffer != nullptr )
        {
            this->stop();
        }

        m_soundBuffer = AndroidSoundBufferPtr::from( _soundBuffer );
    }
    //////////////////////////////////////////////////////////////////////////
    const SoundBufferInterfacePtr & AndroidSoundSource::getSoundBuffer() const
    {
        return m_soundBuffer;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundSource::setHeadMode( bool _headMode )
    {
        m_headMode = _headMode;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundSource::getHeadMode() const
    {
        return m_headMode;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundSource::updateFromSystem()
    {
        if( m_playerPlay == nullptr )
        {
            return;
        }

        if( m_playing == false )
        {
            if( m_needRestart == true )
            {
                m_needRestart = false;

                this->destroyPlayer_();
            }

            return;
        }

        SLuint32 playState = SL_PLAYSTATE_STOPPED;
        SLresult result_state = ( *m_playerPlay )->GetPlayState( m_playerPlay, &playState );

        if( result_state != SL_RESULT_SUCCESS )
        {
            return;
        }

        if( playState == SL_PLAYSTATE_STOPPED )
        {
            m_playing = false;
            m_pausing = false;
            m_position = 0.f;
            m_needRestart = true;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundSource::createPlayer_()
    {
        this->destroyPlayer_();

        if( m_soundSystem == nullptr )
        {
            return false;
        }

        SLEngineItf engine = m_soundSystem->getSLEngine();
        SLObjectItf outputMix = m_soundSystem->getOutputMixObject();

        if( engine == nullptr || outputMix == nullptr )
        {
            LOGGER_ERROR( "AndroidSoundSource invalid engine interfaces" );

            return false;
        }

        SLDataLocator_AndroidSimpleBufferQueue locatorBufferQueue = { SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 1 };

        SLuint32 channels = m_soundBuffer->getChannels();

        SLuint32 channelMask;

        if( channels == 1 )
        {
            channelMask = SL_SPEAKER_FRONT_CENTER;
        }
        else if( channels == 2 )
        {
            channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
        }
        else
        {
            LOGGER_ERROR( "AndroidSoundSource invalid channel count %u", channels );

            return false;
        }

        SLDataFormat_PCM formatPCM;
        formatPCM.formatType = SL_DATAFORMAT_PCM;
        formatPCM.numChannels = channels;
        formatPCM.samplesPerSec = m_soundBuffer->getFrequency() * 1000;
        formatPCM.bitsPerSample = m_soundBuffer->getBits();
        formatPCM.containerSize = m_soundBuffer->getBits();
        formatPCM.channelMask = channelMask;
        formatPCM.endianness = SL_BYTEORDER_LITTLEENDIAN;

        SLDataSource audioSrc = { &locatorBufferQueue, &formatPCM };

        SLDataLocator_OutputMix locatorOutputMix = { SL_DATALOCATOR_OUTPUTMIX, outputMix };
        SLDataSink audioSnk = { &locatorOutputMix, nullptr };

        const SLInterfaceID ids[] = { SL_IID_BUFFERQUEUE, SL_IID_VOLUME };
        const SLboolean req[] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };

        SLresult result_player = ( *engine )->CreateAudioPlayer( engine, &m_playerObject, &audioSrc, &audioSnk, 2, ids, req );

        if( result_player != SL_RESULT_SUCCESS )
        {
            LOGGER_ERROR( "AndroidSoundSource invalid create audio player [%d]", result_player );

            m_playerObject = nullptr;

            return false;
        }

        SLresult result_realize = ( *m_playerObject )->Realize( m_playerObject, SL_BOOLEAN_FALSE );

        if( result_realize != SL_RESULT_SUCCESS )
        {
            LOGGER_ERROR( "AndroidSoundSource invalid realize audio player [%d]", result_realize );

            this->destroyPlayer_();

            return false;
        }

        SLresult result_play = ( *m_playerObject )->GetInterface( m_playerObject, SL_IID_PLAY, &m_playerPlay );

        if( result_play != SL_RESULT_SUCCESS )
        {
            LOGGER_ERROR( "AndroidSoundSource invalid get play interface [%d]", result_play );

            this->destroyPlayer_();

            return false;
        }

        SLresult result_buffer = ( *m_playerObject )->GetInterface( m_playerObject, SL_IID_BUFFERQUEUE, &m_playerBufferQueue );

        if( result_buffer != SL_RESULT_SUCCESS )
        {
            LOGGER_ERROR( "AndroidSoundSource invalid get buffer queue interface [%d]", result_buffer );

            this->destroyPlayer_();

            return false;
        }

        ( *m_playerBufferQueue )->RegisterCallback( m_playerBufferQueue, &AndroidSoundSource::s_callbackBufferQueue_, this );

        SLresult result_volume = ( *m_playerObject )->GetInterface( m_playerObject, SL_IID_VOLUME, &m_playerVolume );

        if( result_volume != SL_RESULT_SUCCESS )
        {
            m_playerVolume = nullptr;
        }

        m_needRestart = false;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundSource::destroyPlayer_()
    {
        if( m_playerPlay != nullptr )
        {
            ( *m_playerPlay )->SetPlayState( m_playerPlay, SL_PLAYSTATE_STOPPED );
        }

        if( m_playerBufferQueue != nullptr )
        {
            ( *m_playerBufferQueue )->Clear( m_playerBufferQueue );
            m_playerBufferQueue = nullptr;
        }

        m_playerVolume = nullptr;
        m_playerPlay = nullptr;

        if( m_playerObject != nullptr )
        {
            ( *m_playerObject )->Destroy( m_playerObject );
            m_playerObject = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundSource::applyVolume_()
    {
        if( m_playerVolume == nullptr )
        {
            return;
        }

        if( m_volume <= 0.f )
        {
            ( *m_playerVolume )->SetVolumeLevel( m_playerVolume, SL_MILLIBEL_MIN );

            return;
        }

        float volume = m_volume;

        if( volume < 0.f )
        {
            volume = 0.f;
        }
        else if( volume > 1.f )
        {
            volume = 1.f;
        }

        SLmillibel level = (SLmillibel)(2000.f * log10f( volume ));

        ( *m_playerVolume )->SetVolumeLevel( m_playerVolume, level );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundSource::applyLooping_()
    {
        if( m_playerPlay == nullptr )
        {
            return;
        }

        SLuint32 playState = SL_PLAYSTATE_STOPPED;
        ( *m_playerPlay )->GetPlayState( m_playerPlay, &playState );

        if( playState == SL_PLAYSTATE_PLAYING && m_loop == false )
        {
            // do nothing, callback will stop playback once buffer consumed
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundSource::enqueueBuffer_( size_t _offsetBytes )
    {
        if( m_playerBufferQueue == nullptr )
        {
            return false;
        }

        const uint8_t * data = m_soundBuffer->getData();
        size_t dataSize = m_soundBuffer->getDataSize();

        if( data == nullptr || dataSize == 0 )
        {
            return false;
        }

        if( _offsetBytes >= dataSize )
        {
            return false;
        }

        const void * bufferData = data + _offsetBytes;
        size_t bufferSize = dataSize - _offsetBytes;

        SLresult result_clear = ( *m_playerBufferQueue )->Clear( m_playerBufferQueue );

        if( result_clear != SL_RESULT_SUCCESS )
        {
            LOGGER_ERROR( "AndroidSoundSource invalid clear buffer queue [%d]", result_clear );

            return false;
        }

        SLresult result_enqueue = ( *m_playerBufferQueue )->Enqueue( m_playerBufferQueue, bufferData, bufferSize );

        if( result_enqueue != SL_RESULT_SUCCESS )
        {
            LOGGER_ERROR( "AndroidSoundSource invalid enqueue buffer [%d]", result_enqueue );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundSource::onBufferQueueEnd_()
    {
        if( m_loop == true )
        {
            if( this->enqueueBuffer_( 0 ) == false )
            {
                LOGGER_ERROR( "AndroidSoundSource invalid re-enqueue loop buffer" );

                m_playing = false;
                m_pausing = false;
                m_position = 0.f;
                m_needRestart = true;

                return;
            }

            SLresult result_play = ( *m_playerPlay )->SetPlayState( m_playerPlay, SL_PLAYSTATE_PLAYING );

            if( result_play != SL_RESULT_SUCCESS )
            {
                LOGGER_ERROR( "AndroidSoundSource invalid restart loop [%d]", result_play );

                m_playing = false;
                m_pausing = false;
                m_position = 0.f;
                m_needRestart = true;
            }

            return;
        }

        m_playing = false;
        m_pausing = false;
        m_position = 0.f;
        m_needRestart = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundSource::s_callbackBufferQueue_( SLAndroidSimpleBufferQueueItf _bufferQueue, void * _context )
    {
        MENGINE_UNUSED( _bufferQueue );

        AndroidSoundSource * soundSource = static_cast<AndroidSoundSource *>( _context );

        soundSource->onBufferQueueEnd_();
    }
    //////////////////////////////////////////////////////////////////////////
}

