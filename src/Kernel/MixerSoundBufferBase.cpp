#include "MixerSoundBufferBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MixerSoundBufferBase::MixerSoundBufferBase()
        : m_frequency( 0 )
        , m_sourceFrequency( 0 )
        , m_outputFrequency( 0 )
        , m_channels( 0 )
        , m_duration( 0.f )
        , m_isStereo( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MixerSoundBufferBase::~MixerSoundBufferBase()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerSoundBufferBase::acquireSoundBuffer()
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
    void MixerSoundBufferBase::releaseSoundBuffer()
    {
        uint32_t referenceCount = m_refacquire.decrefReferenceCount();

        if( referenceCount != 0 )
        {
            return;
        }

        this->_releaseSoundBuffer();
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerSoundBufferBase::_acquireSoundBuffer()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MixerSoundBufferBase::_releaseSoundBuffer()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    const SoundDecoderInterfacePtr & MixerSoundBufferBase::getDecoder() const
    {
        return m_soundDecoder;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerSoundBufferBase::setLoopSource( bool _looped )
    {
        MENGINE_UNUSED( _looped );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MixerSoundBufferBase::setOutputFrequency( uint32_t _frequency )
    {
        m_outputFrequency = _frequency;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MixerSoundBufferBase::getOutputFrequency() const
    {
        return m_outputFrequency;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerSoundBufferBase::isStereo() const
    {
        return m_isStereo;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MixerSoundBufferBase::getFrequency() const
    {
        return m_frequency;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MixerSoundBufferBase::getSourceFrequency() const
    {
        return m_sourceFrequency;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MixerSoundBufferBase::getChannels() const
    {
        return m_channels;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MixerSoundBufferBase::getFrameSize() const
    {
        uint32_t frameSize = m_channels * sizeof( int16_t );

        return frameSize;
    }
    //////////////////////////////////////////////////////////////////////////
    float MixerSoundBufferBase::getTimeDuration() const
    {
        return m_duration;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MixerSoundBufferBase::getFrameCount() const
    {
        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MixerSoundBufferBase::isEndOfStream() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}
