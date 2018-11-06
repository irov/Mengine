#include "OpenALSoundBufferBase.h"

#include "Interface/SoundCodecInterface.h"

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
    bool OpenALSoundBufferBase::initialize( OpenALSoundSystem * _soundSystem )
    {
        m_soundSystem = _soundSystem;

        bool succesful = this->_initialize();

        return succesful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundBufferBase::_initialize()
    {
        //Empty

        return true;
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
