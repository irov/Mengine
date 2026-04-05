#include "WASAPISoundBufferBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    WASAPISoundBufferBase::WASAPISoundBufferBase()
        : m_frequency( 0 )
        , m_sourceFrequency( 0 )
        , m_outputFrequency( 0 )
        , m_channels( 0 )
        , m_duration( 0.f )
        , m_isStereo( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    WASAPISoundBufferBase::~WASAPISoundBufferBase()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool WASAPISoundBufferBase::acquireSoundBuffer()
    {
        uint32_t referenceCount = m_refacquire.increfReferenceCount();

        if( referenceCount != 0 )
        {
            return true;
        }

        return this->_acquireSoundBuffer();
    }
    //////////////////////////////////////////////////////////////////////////
    void WASAPISoundBufferBase::releaseSoundBuffer()
    {
        uint32_t referenceCount = m_refacquire.decrefReferenceCount();

        if( referenceCount != 0 )
        {
            return;
        }

        this->_releaseSoundBuffer();
    }
    //////////////////////////////////////////////////////////////////////////
    bool WASAPISoundBufferBase::_acquireSoundBuffer()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void WASAPISoundBufferBase::_releaseSoundBuffer()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const SoundDecoderInterfacePtr & WASAPISoundBufferBase::getDecoder() const
    {
        return m_soundDecoder;
    }
    //////////////////////////////////////////////////////////////////////////
    void WASAPISoundBufferBase::setOutputFrequency( uint32_t _frequency )
    {
        m_outputFrequency = _frequency;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t WASAPISoundBufferBase::getOutputFrequency() const
    {
        return m_outputFrequency;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WASAPISoundBufferBase::isStereo() const
    {
        return m_isStereo;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t WASAPISoundBufferBase::getFrequency() const
    {
        return m_frequency;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t WASAPISoundBufferBase::getSourceFrequency() const
    {
        return m_sourceFrequency;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t WASAPISoundBufferBase::getChannels() const
    {
        return m_channels;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t WASAPISoundBufferBase::getFrameSize() const
    {
        return m_channels * 2;
    }
    //////////////////////////////////////////////////////////////////////////
    float WASAPISoundBufferBase::getTimeDuration() const
    {
        return m_duration;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t WASAPISoundBufferBase::getFrameCount() const
    {
        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
}
