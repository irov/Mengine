#include "OpenALSoundSystem.h"
#include "OpenALSoundError.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/ThreadServiceInterface.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/Logger.h"

#include <algorithm>
#include <stdio.h>
#include <stdarg.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SoundSystem, Mengine::OpenALSoundSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OpenALSoundSystem::OpenALSoundSystem()
        : m_context( nullptr )
        , m_device( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OpenALSoundSystem::~OpenALSoundSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundSystem::_initializeService()
    {
        LOGGER_INFO( "Starting OpenAL Sound System..." );

        m_device = alcOpenDevice( nullptr );

        if( m_device == nullptr )
        {
            LOGGER_ERROR( "Failed to open 'generic software' sound device try default..." );

            m_device = alcOpenDevice( "Generic Software" );

            if( m_device == nullptr )
            {
                LOGGER_ERROR( "Failed to open default sound device try hardware" );

                m_device = alcOpenDevice( "Generic Hardware" );

                if( m_device == nullptr )
                {
                    LOGGER_ERROR( "Failed to open hardware sound device.." );

                    return false;
                }
            }
        }

        m_context = alcCreateContext( m_device, nullptr );

        if( m_context == nullptr )
        {
            LOGGER_ERROR( "Failed to create context" );

            alcCloseDevice( m_device );

            m_device = nullptr;

            return false;
        }

        ALCboolean currentResult = alcMakeContextCurrent( m_context );
        OPENAL_CHECK_ERROR();

        if( currentResult == ALC_FALSE )
        {
            LOGGER_ERROR( "Failed to make context current" );

            alcDestroyContext( m_context );
            OPENAL_CHECK_ERROR();
            m_context = nullptr;

            alcCloseDevice( m_device );
            OPENAL_CHECK_ERROR();
            m_device = nullptr;

            return false;
        }


        ALCint majorVersion;
        alcGetIntegerv( m_device, ALC_MAJOR_VERSION, 1, &majorVersion );

        ALCint minorVersion;
        alcGetIntegerv( m_device, ALC_MINOR_VERSION, 1, &minorVersion );

        LOGGER_WARNING( "OpenAL version %u.%u"
            , majorVersion
            , minorVersion
        );

        const ALCchar* defaultDeviceSprcifier = alcGetString( m_device, ALC_DEVICE_SPECIFIER );

        LOGGER_WARNING( "OpenAL default device specifier [%s]"
            , defaultDeviceSprcifier
        );

        const ALCchar* captureDeviceSpecifier = alcGetString( m_device, ALC_CAPTURE_DEVICE_SPECIFIER );

        LOGGER_WARNING( "OpenAL capture device specifier [%s]"
            , captureDeviceSpecifier
        );

        LOGGER_WARNING( "OpenAL driver properties" );

        LOGGER_WARNING( "Version: %s"
            , alGetString( AL_VERSION )
        );

        LOGGER_WARNING( "Vendor: %s"
            , alGetString( AL_VENDOR )
        );

        LOGGER_WARNING( "Renderer: %s"
            , alGetString( AL_RENDERER )
        );

        float lposition[] = { 0.f, 0.f, 0.f };
        alListenerfv( AL_POSITION, lposition );
        OPENAL_CHECK_ERROR();

        float lvelocity[] = { 0.f, 0.f, 0.f };
        alListenerfv( AL_VELOCITY, lvelocity );
        OPENAL_CHECK_ERROR();

        float lorient[] = { 0.f, 0.f, 1.f, 0.f, 1.f, 0.f };
        alListenerfv( AL_ORIENTATION, lorient );
        OPENAL_CHECK_ERROR();

        m_factoryOpenALSoundBuffer = new FactoryPool<OpenALSoundBufferMemory, 32>();
        m_factoryOpenALSoundBufferStream = new FactoryPool<OpenALSoundBufferStream, 32>();
        m_factoryOpenALSoundSource = new FactoryPool<OpenALSoundSource, 32>();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundSystem::_finalizeService()
    {
        if( m_device != nullptr )
        {
            alcMakeContextCurrent( nullptr );

            if( m_context != nullptr )
            {
                alcDestroyContext( m_context );
                m_context = nullptr;
            }

            alcCloseDevice( m_device );
            m_device = nullptr;
        }

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryOpenALSoundBuffer );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryOpenALSoundBufferStream );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryOpenALSoundSource );

        m_factoryOpenALSoundBuffer = nullptr;
        m_factoryOpenALSoundBufferStream = nullptr;
        m_factoryOpenALSoundSource = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundSystem::update()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenALSoundSystem::isSilent() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundSystem::onTurnSound( bool _turn )
    {
        (void)_turn;
        //if( _turn == false )
        //{
        //    alcMakeContextCurrent( NULL );
        //    //OAL_CHECK_ERROR();

        //    alcSuspendContext( m_context );
        //    //OAL_CHECK_ERROR();
        //}
        //else
        //{
        //    alcMakeContextCurrent( m_context );
        //    //OAL_CHECK_ERROR();

        //    alcProcessContext( m_context );
        //    //OAL_CHECK_ERROR();
        //}
    }
    //////////////////////////////////////////////////////////////////////////
    SoundSourceInterfacePtr OpenALSoundSystem::createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _buffer )
    {
        OpenALSoundSourcePtr soundSource = m_factoryOpenALSoundSource->createObject();

        soundSource->initialize( this );

        soundSource->setHeadMode( _isHeadMode );
        soundSource->setSoundBuffer( _buffer );

        return soundSource;
    }
    //////////////////////////////////////////////////////////////////////////
    SoundBufferInterfacePtr OpenALSoundSystem::createSoundBuffer( const SoundDecoderInterfacePtr & _soundDecoder, bool _isStream )
    {
        OpenALSoundBufferBasePtr base = nullptr;

        if( _isStream == false || SERVICE_AVAILABLE( ThreadServiceInterface ) == false )
        {
            OpenALSoundBufferMemoryPtr buffer = m_factoryOpenALSoundBuffer->createObject();

            if( buffer->initialize( this ) == false )
            {
                return nullptr;
            }

            base = buffer;
        }
        else
        {
            OpenALSoundBufferStreamPtr buffer = m_factoryOpenALSoundBufferStream->createObject();

            if( buffer->initialize( this ) == false )
            {
                return nullptr;
            }

            base = buffer;
        }

        if( base->load( _soundDecoder ) == false )
        {
            LOGGER_ERROR( "Failed to create sound buffer from stream"
            );

            return nullptr;
        }

        return base;
    }
    //////////////////////////////////////////////////////////////////////////
    ALuint OpenALSoundSystem::genSourceId()
    {
        ALuint sourceId = 0;
        alGenSources( 1, &sourceId );
        OPENAL_CHECK_ERROR();

        return sourceId;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundSystem::releaseSourceId( ALuint _sourceId )
    {
        alDeleteSources( 1, &_sourceId );
        OPENAL_CHECK_ERROR();
    }
    //////////////////////////////////////////////////////////////////////////
    ALuint OpenALSoundSystem::genBufferId()
    {
        ALuint bufferId = 0;
        alGenBuffers( 1, &bufferId );
        OPENAL_CHECK_ERROR();

        return bufferId;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundSystem::releaseBufferId( ALuint _bufferId )
    {
        alDeleteBuffers( 1, &_bufferId );
        OPENAL_CHECK_ERROR();
    }
}
