#include "AppleSoundBufferBase.h"

#include "Interface/SoundSystemInterface.h"
#include "Interface/SoundCodecInterface.h"

#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleSoundBufferBase::AppleSoundBufferBase()
        : m_frequency( 0 )
        , m_channels( 0 )
        , m_duration( 0.f )
        , m_isStereo( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleSoundBufferBase::~AppleSoundBufferBase()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundBufferBase::acquireSoundBuffer()
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
    void AppleSoundBufferBase::releaseSoundBuffer()
    {
        uint32_t referenceCount = m_refacquire.decrefReferenceCount();

        if( referenceCount != 0 )
        {
            return;
        }

        this->_releaseSoundBuffer();
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundBufferBase::_acquireSoundBuffer()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSoundBufferBase::_releaseSoundBuffer()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    const SoundDecoderInterfacePtr & AppleSoundBufferBase::getDecoder() const
    {
        return m_soundDecoder;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSoundBufferBase::isStereo() const
    {
        return m_isStereo;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t AppleSoundBufferBase::getFrequency() const
    {
        return m_frequency;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t AppleSoundBufferBase::getChannels() const
    {
        return m_channels;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t AppleSoundBufferBase::getFrameSize() const
    {
        return m_channels * 2;
    }
    //////////////////////////////////////////////////////////////////////////
    float AppleSoundBufferBase::getTimeDuration() const
    {
        return m_duration;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t AppleSoundBufferBase::getFrameCount() const
    {
        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
}
