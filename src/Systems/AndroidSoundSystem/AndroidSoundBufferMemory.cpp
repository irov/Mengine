#include "AndroidSoundBufferMemory.h"

#include "Interface/SoundCodecInterface.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/TimestampHelper.h"

#include "Config/StdMath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidSoundBufferMemory::AndroidSoundBufferMemory()
        : m_pcmDataSize( 0 )
        , m_looped( false )
        , m_playTimestamp( MENGINE_UINT64_C(0) )
        , m_pauseTimestamp( MENGINE_UINT64_C(0) )
        , m_positionOffset( 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidSoundBufferMemory::~AndroidSoundBufferMemory()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundBufferMemory::_acquireSoundBuffer()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundBufferMemory::_releaseSoundBuffer()
    {
        this->destroyPlayer_();

        m_pcmMemory = nullptr;
        m_soundDecoder = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundBufferMemory::updateSoundBuffer()
    {
        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundBufferMemory::load( const SoundDecoderInterfacePtr & _soundDecoder )
    {
        const SoundCodecDataInfo * dataInfo = _soundDecoder->getCodecDataInfo();

        m_frequency = dataInfo->frequency;
        m_channels = dataInfo->channels;
        m_duration = dataInfo->duration;
        size_t size = dataInfo->size;

        MemoryInterfacePtr pcmMemory = Helper::createMemoryCacheBuffer( size, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( pcmMemory, "invalid sound memory: %zu"
            , size
        );

        void * pcmBuffer = pcmMemory->getBuffer();

        SoundDecoderData data;
        data.buffer = pcmBuffer;
        data.size = size;

        size_t decodeSize = _soundDecoder->decode( &data );

        if( decodeSize == 0 )
        {
            LOGGER_WARNING( "invalid sound decode: %zu"
                , size
            );

            return false;
        }

        size_t correctDecodeSize = decodeSize - decodeSize % 4;

        switch( m_channels )
        {
        case 1:
            {
                m_isStereo = false;
            }break;
        case 2:
            {
                m_isStereo = true;
            }break;
        default:
            {
                LOGGER_WARNING( "invalid sound format channels: %u"
                    , m_channels
                );

                return false;
            }break;
        }

        m_soundDecoder = _soundDecoder;
        m_pcmMemory = pcmMemory;
        m_pcmDataSize = correctDecodeSize;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundBufferMemory::bufferQueueCallback_( SLAndroidSimpleBufferQueueItf _bq, void * _context )
    {
        MENGINE_UNUSED( _bq );

        AndroidSoundBufferMemory * buffer = static_cast<AndroidSoundBufferMemory *>(_context);
        buffer->onBufferConsumed_();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundBufferMemory::onBufferConsumed_()
    {
        if( m_looped == true )
        {
            void * pcmBuffer = m_pcmMemory->getBuffer();

            (*m_bufferQueueItf)->Enqueue( m_bufferQueueItf, pcmBuffer, (SLuint32)m_pcmDataSize );
        }
        else
        {
            m_finished = true;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundBufferMemory::playSource( bool _looped, float _position )
    {
        if( m_playerObject != nullptr )
        {
            this->destroyPlayer_();
        }

        if( this->createPlayer_() == false )
        {
            return false;
        }

        m_looped = _looped;
        m_finished = false;
        m_pauseTimestamp = MENGINE_UINT64_C(0);

        (*m_bufferQueueItf)->RegisterCallback( m_bufferQueueItf, &AndroidSoundBufferMemory::bufferQueueCallback_, this );

        void * pcmBuffer = m_pcmMemory->getBuffer();
        size_t dataSize = m_pcmDataSize;
        size_t dataOffset = 0;
        uint32_t frameSize = m_channels * 2;

        if( _position > 0.f )
        {
            float positionSec = _position * 0.001f;
            uint32_t bytesPerSecond = m_frequency * m_channels * 2;
            size_t byteOffset = (size_t)(positionSec * (float)bytesPerSecond);

            byteOffset -= byteOffset % frameSize;

            if( byteOffset >= dataSize )
            {
                m_playTimestamp = Helper::getSystemTimestamp();
                m_positionOffset = m_duration;
                m_finished = true;

                return true;
            }

            dataOffset = byteOffset;
        }

        uint8_t * startBuffer = static_cast<uint8_t *>(pcmBuffer) + dataOffset;
        SLuint32 enqueueSize = (SLuint32)(dataSize - dataOffset);

        SLresult result;

        result = (*m_bufferQueueItf)->Enqueue( m_bufferQueueItf, startBuffer, enqueueSize );

        if( result != SL_RESULT_SUCCESS )
        {
            LOGGER_ASSERTION( "failed to enqueue buffer [%d]"
                , result
            );

            this->destroyPlayer_();

            return false;
        }

        m_playTimestamp = Helper::getSystemTimestamp();
        m_positionOffset = _position;

        result = (*m_playItf)->SetPlayState( m_playItf, SL_PLAYSTATE_PLAYING );

        if( result != SL_RESULT_SUCCESS )
        {
            LOGGER_ASSERTION( "failed to set play state [%d]"
                , result
            );

            this->destroyPlayer_();

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundBufferMemory::stopSource()
    {
        if( m_playerObject == nullptr )
        {
            return;
        }

        (*m_playItf)->SetPlayState( m_playItf, SL_PLAYSTATE_STOPPED );
        (*m_bufferQueueItf)->Clear( m_bufferQueueItf );

        this->destroyPlayer_();

        m_playTimestamp = MENGINE_UINT64_C(0);
        m_pauseTimestamp = MENGINE_UINT64_C(0);
        m_positionOffset = 0.f;
        m_finished = false;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundBufferMemory::pauseSource()
    {
        if( m_playerObject == nullptr )
        {
            return;
        }

        (*m_playItf)->SetPlayState( m_playItf, SL_PLAYSTATE_PAUSED );

        m_pauseTimestamp = Helper::getSystemTimestamp();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundBufferMemory::resumeSource()
    {
        if( m_playerObject == nullptr )
        {
            return;
        }

        if( m_pauseTimestamp != MENGINE_UINT64_C(0) )
        {
            Timestamp pausedDuration = Helper::getSystemTimestamp() - m_pauseTimestamp;
            m_playTimestamp += pausedDuration;
            m_pauseTimestamp = MENGINE_UINT64_C(0);
        }

        (*m_playItf)->SetPlayState( m_playItf, SL_PLAYSTATE_PLAYING );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundBufferMemory::setTimePosition( float _position )
    {
        if( m_playerObject == nullptr )
        {
            return false;
        }

        bool paused = m_pauseTimestamp != MENGINE_UINT64_C(0);

        (*m_playItf)->SetPlayState( m_playItf, SL_PLAYSTATE_STOPPED );
        (*m_bufferQueueItf)->Clear( m_bufferQueueItf );

        void * pcmBuffer = m_pcmMemory->getBuffer();
        size_t dataSize = m_pcmDataSize;

        float positionSec = _position * 0.001f;
        uint32_t bytesPerSecond = m_frequency * m_channels * 2;
        size_t byteOffset = (size_t)(positionSec * (float)bytesPerSecond);

        uint32_t frameSize = m_channels * 2;
        byteOffset -= byteOffset % frameSize;

        if( byteOffset >= dataSize )
        {
            Timestamp currentTimestamp = Helper::getSystemTimestamp();

            m_playTimestamp = currentTimestamp;
            m_positionOffset = m_duration;
            m_pauseTimestamp = paused == true ? currentTimestamp : MENGINE_UINT64_C(0);
            m_finished = true;

            return true;
        }

        uint8_t * startBuffer = static_cast<uint8_t *>(pcmBuffer) + byteOffset;
        SLuint32 enqueueSize = (SLuint32)(dataSize - byteOffset);

        SLresult result;

        result = (*m_bufferQueueItf)->Enqueue( m_bufferQueueItf, startBuffer, enqueueSize );

        if( result != SL_RESULT_SUCCESS )
        {
            return false;
        }

        m_playTimestamp = Helper::getSystemTimestamp();
        m_positionOffset = _position;
        m_pauseTimestamp = paused == true ? m_playTimestamp : MENGINE_UINT64_C(0);
        m_finished = false;

        result = (*m_playItf)->SetPlayState( m_playItf, paused == true ? SL_PLAYSTATE_PAUSED : SL_PLAYSTATE_PLAYING );

        if( result != SL_RESULT_SUCCESS )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundBufferMemory::getTimePosition( float * const _position ) const
    {
        if( m_playTimestamp == MENGINE_UINT64_C(0) )
        {
            *_position = 0.f;

            return true;
        }

        Timestamp currentTime;

        if( m_pauseTimestamp != MENGINE_UINT64_C(0) )
        {
            currentTime = m_pauseTimestamp;
        }
        else
        {
            currentTime = Helper::getSystemTimestamp();
        }

        float elapsedMs = (float)(currentTime - m_playTimestamp);
        float position = m_positionOffset + elapsedMs;

        if( m_looped == true && m_duration > 0.f )
        {
            position = StdMath::fmodf( position, m_duration );
        }
        else if( position > m_duration )
        {
            position = m_duration;
        }

        *_position = position;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundBufferMemory::setVolume( float _gain )
    {
        if( m_volumeItf == nullptr )
        {
            return;
        }

        SLmillibel mb = AndroidSoundBufferBase::gainToMillibels_( _gain );

        (*m_volumeItf)->SetVolumeLevel( m_volumeItf, mb );
    }
    //////////////////////////////////////////////////////////////////////////
}
