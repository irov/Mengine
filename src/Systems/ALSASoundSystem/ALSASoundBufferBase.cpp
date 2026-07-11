#include "ALSASoundBufferBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ALSASoundBufferBase::ALSASoundBufferBase()
        : m_frequency( 0 )
        , m_sourceFrequency( 0 )
        , m_outputFrequency( 0 )
        , m_channels( 0 )
        , m_duration( 0.f )
        , m_isStereo( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ALSASoundBufferBase::~ALSASoundBufferBase()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ALSASoundBufferBase::acquireSoundBuffer()
    {
        uint32_t referenceCount = m_refacquire.increfReferenceCount();

        if( referenceCount != 0 )
        {
            return true;
        }

        if( this->_acquireSoundBuffer() == false )
        {
            m_refacquire.decrefReferenceCount();

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ALSASoundBufferBase::releaseSoundBuffer()
    {
        uint32_t referenceCount = m_refacquire.decrefReferenceCount();

        if( referenceCount != 0 )
        {
            return;
        }

        this->_releaseSoundBuffer();
    }
    //////////////////////////////////////////////////////////////////////////
    bool ALSASoundBufferBase::_acquireSoundBuffer()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ALSASoundBufferBase::_releaseSoundBuffer()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const SoundDecoderInterfacePtr & ALSASoundBufferBase::getDecoder() const
    {
        return m_soundDecoder;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ALSASoundBufferBase::setLoopSource( bool _looped )
    {
        MENGINE_UNUSED( _looped );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ALSASoundBufferBase::setOutputFrequency( uint32_t _frequency )
    {
        m_outputFrequency = _frequency;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ALSASoundBufferBase::getOutputFrequency() const
    {
        return m_outputFrequency;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ALSASoundBufferBase::isStereo() const
    {
        return m_isStereo;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ALSASoundBufferBase::getFrequency() const
    {
        return m_frequency;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ALSASoundBufferBase::getSourceFrequency() const
    {
        return m_sourceFrequency;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ALSASoundBufferBase::getChannels() const
    {
        return m_channels;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ALSASoundBufferBase::getFrameSize() const
    {
        return m_channels * 2;
    }
    //////////////////////////////////////////////////////////////////////////
    float ALSASoundBufferBase::getTimeDuration() const
    {
        return m_duration;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ALSASoundBufferBase::getFrameCount() const
    {
        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ALSASoundBufferBase::isEndOfStream() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}
