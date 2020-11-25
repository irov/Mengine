#include "OpenALSoundBufferBase.h"

#include "Interface/SoundCodecInterface.h"

#include "Kernel/Assertion.h"

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
    bool OpenALSoundBufferBase::acquire()
    {
        if( ++m_refacquire == 1 )
        {
            if( this->_acquire() == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundBufferBase::release()
    {
        MENGINE_ASSERTION_FATAL( m_refacquire > 0 );

        if( --m_refacquire == 0 )
        {
            this->_release();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferBase::_acquire()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundBufferBase::_release()
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
