#include "OpenALSoundBufferBase.h"

#include "Interface/SoundSystemInterface.h"
#include "Interface/SoundCodecInterface.h"

#include "OpenALSoundSystemExtensionInterface.h"

#include "Kernel/Assertion.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OpenALSoundBufferBase::OpenALSoundBufferBase()
        : m_format( 0 )
        , m_frequency( 0 )
        , m_channels( 0 )
        , m_duration( 0.f )
        , m_isStereo( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OpenALSoundBufferBase::~OpenALSoundBufferBase()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferBase::acquireSoundBuffer()
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
    void OpenALSoundBufferBase::releaseSoundBuffer()
    {
        uint32_t referenceCount = m_refacquire.decrefReferenceCount();

        if( referenceCount != 0 )
        {
            return;
        }

        this->_releaseSoundBuffer();
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferBase::_acquireSoundBuffer()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundBufferBase::_releaseSoundBuffer()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    const SoundDecoderInterfacePtr & OpenALSoundBufferBase::getDecoder() const
    {
        return m_soundDecoder;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferBase::isStereo() const
    {
        return m_isStereo;
    }
    //////////////////////////////////////////////////////////////////////////
    float OpenALSoundBufferBase::getTimeDuration() const
    {
        return m_duration;
    }
    //////////////////////////////////////////////////////////////////////////
    ALuint OpenALSoundBufferBase::genBufferId()
    {
        OpenALSoundSystemExtensionInterface * soundSystemExtension = SOUND_SYSTEM()
            ->getUnknown();

        ALuint bufferId = soundSystemExtension->genBufferId();

        return bufferId;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundBufferBase::releaseBufferId( ALuint _bufferId )
    {
        OpenALSoundSystemExtensionInterface * soundSystemExtension = SOUND_SYSTEM()
            ->getUnknown();

        soundSystemExtension->releaseBufferId( _bufferId );
    }
    //////////////////////////////////////////////////////////////////////////
}
