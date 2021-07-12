#include "OpenALSoundBufferBase.h"

#include "Interface/SoundCodecInterface.h"

#include "Kernel/Assertion.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OpenALSoundBufferBase::OpenALSoundBufferBase()
        : m_soundSystem( nullptr )
        , m_format( 0 )
        , m_frequency( 0 )
        , m_channels( 0 )
        , m_length( 0.f )
        , m_isStereo( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OpenALSoundBufferBase::~OpenALSoundBufferBase()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundBufferBase::setSoundSystem( OpenALSoundSystem * _soundSystem )
    {
        m_soundSystem = _soundSystem;
    }
    //////////////////////////////////////////////////////////////////////////
    OpenALSoundSystem * OpenALSoundBufferBase::getSoundSystem() const
    {
        return m_soundSystem;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferBase::acquireSoundBuffer()
    {
        if( m_refacquire.incref() == false )
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
        if( m_refacquire.decref() == false )
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
    float OpenALSoundBufferBase::getTimeTotal() const
    {
        return m_length;
    }
    //////////////////////////////////////////////////////////////////////////
}
