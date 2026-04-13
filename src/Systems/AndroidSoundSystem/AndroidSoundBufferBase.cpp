#include "AndroidSoundBufferBase.h"

#include "Interface/SoundSystemInterface.h"
#include "Interface/SoundCodecInterface.h"

#include "AndroidSoundSystemExtensionInterface.h"

#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"

#include "Config/StdMath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidSoundBufferBase::AndroidSoundBufferBase()
        : m_frequency( 0 )
        , m_channels( 0 )
        , m_duration( 0.f )
        , m_isStereo( false )
        , m_finished( false )
        , m_playerObject( nullptr )
        , m_playItf( nullptr )
        , m_volumeItf( nullptr )
        , m_bufferQueueItf( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidSoundBufferBase::~AndroidSoundBufferBase()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundBufferBase::acquireSoundBuffer()
    {
        uint32_t referenceCount = m_refacquire.increfReferenceCount();

        if( referenceCount != 0 )
        {
            return true;
        }

        if( this->_acquireSoundBuffer() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundBufferBase::releaseSoundBuffer()
    {
        uint32_t referenceCount = m_refacquire.decrefReferenceCount();

        if( referenceCount != 0 )
        {
            return;
        }

        this->_releaseSoundBuffer();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundBufferBase::_acquireSoundBuffer()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundBufferBase::_releaseSoundBuffer()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    const SoundDecoderInterfacePtr & AndroidSoundBufferBase::getDecoder() const
    {
        return m_soundDecoder;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundBufferBase::isStereo() const
    {
        return m_isStereo;
    }
    //////////////////////////////////////////////////////////////////////////
    float AndroidSoundBufferBase::getTimeDuration() const
    {
        return m_duration;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundBufferBase::isFinished() const
    {
        return m_finished.load();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundBufferBase::createPlayer_()
    {
        AndroidSoundSystemExtensionInterface * soundSystemExtension = SOUND_SYSTEM()
            ->getUnknown();

        SLEngineItf engineItf = soundSystemExtension->getEngineInterface();
        SLObjectItf outputMixObject = soundSystemExtension->getOutputMixObject();

        SLDataLocator_AndroidSimpleBufferQueue loc_bufq;
        loc_bufq.locatorType = SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;
        loc_bufq.numBuffers = 4;

        SLuint32 channelMask;

        if( m_channels == 1 )
        {
            channelMask = SL_SPEAKER_FRONT_CENTER;
        }
        else
        {
            channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
        }

        SLDataFormat_PCM format_pcm;
        format_pcm.formatType = SL_DATAFORMAT_PCM;
        format_pcm.numChannels = (SLuint32)m_channels;
        format_pcm.samplesPerSec = (SLuint32)m_frequency * 1000;
        format_pcm.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
        format_pcm.containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;
        format_pcm.channelMask = channelMask;
        format_pcm.endianness = SL_BYTEORDER_LITTLEENDIAN;

        SLDataSource audioSrc;
        audioSrc.pLocator = &loc_bufq;
        audioSrc.pFormat = &format_pcm;

        SLDataLocator_OutputMix loc_outmix;
        loc_outmix.locatorType = SL_DATALOCATOR_OUTPUTMIX;
        loc_outmix.outputMix = outputMixObject;

        SLDataSink audioSnk;
        audioSnk.pLocator = &loc_outmix;
        audioSnk.pFormat = nullptr;

        const SLInterfaceID ids[2] = { SL_IID_BUFFERQUEUE, SL_IID_VOLUME };
        const SLboolean req[2] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };

        SLresult result;

        result = (*engineItf)->CreateAudioPlayer( engineItf, &m_playerObject, &audioSrc, &audioSnk, 2, ids, req );

        if( result != SL_RESULT_SUCCESS )
        {
            LOGGER_ASSERTION( "failed to create audio player [%d]"
                , result
            );

            return false;
        }

        result = (*m_playerObject)->Realize( m_playerObject, SL_BOOLEAN_FALSE );

        if( result != SL_RESULT_SUCCESS )
        {
            LOGGER_ASSERTION( "failed to realize audio player [%d]"
                , result
            );

            (*m_playerObject)->Destroy( m_playerObject );
            m_playerObject = nullptr;

            return false;
        }

        result = (*m_playerObject)->GetInterface( m_playerObject, SL_IID_PLAY, &m_playItf );

        if( result != SL_RESULT_SUCCESS )
        {
            LOGGER_ASSERTION( "failed to get play interface [%d]"
                , result
            );

            (*m_playerObject)->Destroy( m_playerObject );
            m_playerObject = nullptr;

            return false;
        }

        result = (*m_playerObject)->GetInterface( m_playerObject, SL_IID_BUFFERQUEUE, &m_bufferQueueItf );

        if( result != SL_RESULT_SUCCESS )
        {
            LOGGER_ASSERTION( "failed to get buffer queue interface [%d]"
                , result
            );

            (*m_playerObject)->Destroy( m_playerObject );
            m_playerObject = nullptr;
            m_playItf = nullptr;

            return false;
        }

        result = (*m_playerObject)->GetInterface( m_playerObject, SL_IID_VOLUME, &m_volumeItf );

        if( result != SL_RESULT_SUCCESS )
        {
            LOGGER_ASSERTION( "failed to get volume interface [%d]"
                , result
            );

            (*m_playerObject)->Destroy( m_playerObject );
            m_playerObject = nullptr;
            m_playItf = nullptr;
            m_bufferQueueItf = nullptr;

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundBufferBase::destroyPlayer_()
    {
        if( m_playerObject != nullptr )
        {
            (*m_playerObject)->Destroy( m_playerObject );
            m_playerObject = nullptr;
            m_playItf = nullptr;
            m_volumeItf = nullptr;
            m_bufferQueueItf = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    SLmillibel AndroidSoundBufferBase::gainToMillibels_( float _gain )
    {
        if( _gain <= 0.0001f )
        {
            return SL_MILLIBEL_MIN;
        }

        float db = 20.f * StdMath::log10f( _gain );

        SLmillibel mb = (SLmillibel)(db * 100.f);

        if( mb < SL_MILLIBEL_MIN )
        {
            mb = SL_MILLIBEL_MIN;
        }

        return mb;
    }
    //////////////////////////////////////////////////////////////////////////
}
