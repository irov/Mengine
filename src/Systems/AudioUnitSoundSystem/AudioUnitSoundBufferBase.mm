#include "AudioUnitSoundBufferBase.h"

#include "Interface/SoundSystemInterface.h"
#include "Interface/SoundCodecInterface.h"

#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AudioUnitSoundBufferBase::AudioUnitSoundBufferBase()
        : m_frequency( 0 )
        , m_channels( 0 )
        , m_duration( 0.f )
        , m_isStereo( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AudioUnitSoundBufferBase::~AudioUnitSoundBufferBase()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AudioUnitSoundBufferBase::acquireSoundBuffer()
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
    void AudioUnitSoundBufferBase::releaseSoundBuffer()
    {
        uint32_t referenceCount = m_refacquire.decrefReferenceCount();

        if( referenceCount != 0 )
        {
            return;
        }

        this->_releaseSoundBuffer();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AudioUnitSoundBufferBase::_acquireSoundBuffer()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AudioUnitSoundBufferBase::_releaseSoundBuffer()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    const SoundDecoderInterfacePtr & AudioUnitSoundBufferBase::getDecoder() const
    {
        return m_soundDecoder;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AudioUnitSoundBufferBase::isStereo() const
    {
        return m_isStereo;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t AudioUnitSoundBufferBase::getFrequency() const
    {
        return m_frequency;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t AudioUnitSoundBufferBase::getChannels() const
    {
        return m_channels;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t AudioUnitSoundBufferBase::getFrameSize() const
    {
        return m_channels * 2;
    }
    //////////////////////////////////////////////////////////////////////////
    float AudioUnitSoundBufferBase::getTimeDuration() const
    {
        return m_duration;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t AudioUnitSoundBufferBase::getFrameCount() const
    {
        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
}
