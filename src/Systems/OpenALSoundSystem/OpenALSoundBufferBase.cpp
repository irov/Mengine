#include "OpenALSoundBufferBase.h"

#include "Interface/SoundCodecInterface.h"

#include "Kernel/Assertion.h"
#include "Kernel/ThreadGuardScope.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OpenALSoundBufferBase::OpenALSoundBufferBase()
        : m_soundSystem( nullptr )
        , m_refacquire( 0 )
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
        MENGINE_ASSERTION_FATAL( m_refacquire == 0 );
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
        MENGINE_THREAD_GUARD_SCOPE( OpenALSoundBufferBase, this, "OpenALSoundBufferBase::acquire" );

        if( ++m_refacquire == 1 )
        {
            if( this->_acquireSoundBuffer() == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundBufferBase::releaseSoundBuffer()
    {
        MENGINE_THREAD_GUARD_SCOPE( OpenALSoundBufferBase, this, "OpenALSoundBufferBase::release" );

        MENGINE_ASSERTION_FATAL( m_refacquire > 0 );

        if( --m_refacquire == 0 )
        {
            this->_releaseSoundBuffer();
        }
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
