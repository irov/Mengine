#include "OpenALSoundSystem.h"
#include "OpenALSoundError.h"

#include "Interface/UnicodeInterface.h"
#include "Interface/ThreadSystemInterface.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/FactoryAssertion.h"
#include "Kernel/Logger.h"

#include <algorithm>
#include <stdio.h>
#include <stdarg.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SoundSystem, Mengine::OALSoundSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OALSoundSystem::OALSoundSystem()
        : m_context( nullptr )
        , m_device( nullptr )
        , m_threadAvaliable( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OALSoundSystem::~OALSoundSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool OALSoundSystem::_initializeService()
    {
        LOGGER_INFO( "Starting OpenAL Sound System..." );

        m_device = alcOpenDevice( nullptr );

        if( m_device == nullptr )
        {
            LOGGER_ERROR( "OALSoundSystem.initialize: Failed to open 'generic software' sound device try default..." );

            m_device = alcOpenDevice( "Generic Software" );

            OAL_CHECK_ERROR();

            if( m_device == nullptr )
            {
                LOGGER_ERROR( "OALSoundSystem.initialize: Failed to open default sound device try hardware" );

                m_device = alcOpenDevice( "Generic Hardware" );
                OAL_CHECK_ERROR();

                if( m_device == nullptr )
                {
                    LOGGER_ERROR( "OALSoundSystem.initialize: Failed to open hardware sound device.." );

                    return false;
                }
            }
        }

        m_context = alcCreateContext( m_device, nullptr );

        if( m_context == nullptr )
        {
            LOGGER_ERROR( "OALSoundSystem: Failed to create context" );

            alcCloseDevice( m_device );
            OAL_CHECK_ERROR();

            m_device = nullptr;

            return false;
        }

        ALCboolean currentResult = alcMakeContextCurrent( m_context );
        OAL_CHECK_ERROR();

        if( currentResult == ALC_FALSE )
        {
            LOGGER_ERROR( "OALSoundSystem: Failed to make context current" );

            alcDestroyContext( m_context );
            OAL_CHECK_ERROR();
            m_context = nullptr;

            alcCloseDevice( m_device );
            OAL_CHECK_ERROR();
            m_device = nullptr;

            return false;
        }


        ALCint majorVersion;
        alcGetIntegerv( m_device, ALC_MAJOR_VERSION, 1, &majorVersion );

        ALCint minorVersion;
        alcGetIntegerv( m_device, ALC_MINOR_VERSION, 1, &minorVersion );

        LOGGER_WARNING( "OpenAL version %d.%d"
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
        OAL_CHECK_ERROR();

        float lvelocity[] = { 0.f, 0.f, 0.f };
        alListenerfv( AL_VELOCITY, lvelocity );
        OAL_CHECK_ERROR();

        float lorient[] = { 0.f, 0.f, 1.f, 0.f, 1.f, 0.f };
        alListenerfv( AL_ORIENTATION, lorient );
        OAL_CHECK_ERROR();

        m_threadAvaliable = THREAD_SYSTEM()
            ->avaliable();

        m_factoryOALSoundBuffer = new FactoryPool<OALSoundBufferMemory, 32>();
        m_factoryOALSoundBufferStream = new FactoryPool<OALSoundBufferStream, 32>();
        m_factoryOALSoundSource = new FactoryPool<OALSoundSource, 32>();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OALSoundSystem::_finalizeService()
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

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryOALSoundBuffer );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryOALSoundBufferStream );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryOALSoundSource );

        m_factoryOALSoundBuffer = nullptr;
        m_factoryOALSoundBufferStream = nullptr;
        m_factoryOALSoundSource = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void OALSoundSystem::update()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool OALSoundSystem::isSilent() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void OALSoundSystem::onTurnSound( bool _turn )
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
    SoundSourceInterfacePtr OALSoundSystem::createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _buffer )
    {
        OALSoundSourcePtr soundSource = m_factoryOALSoundSource->createObject();

        soundSource->initialize( this );

        soundSource->setHeadMode( _isHeadMode );
        soundSource->setSoundBuffer( _buffer );

        return soundSource;
    }
    //////////////////////////////////////////////////////////////////////////
    SoundBufferInterfacePtr OALSoundSystem::createSoundBuffer( const SoundDecoderInterfacePtr & _soundDecoder, bool _isStream )
    {
        OALSoundBufferBasePtr base = nullptr;

        if( _isStream == false || m_threadAvaliable == false )
        {
            OALSoundBufferMemoryPtr buffer = m_factoryOALSoundBuffer->createObject();

            buffer->initialize( this );

            base = buffer;
        }
        else
        {
            OALSoundBufferStreamPtr buffer = m_factoryOALSoundBufferStream->createObject();

            buffer->initialize( this );

            base = buffer;
        }

        if( base->load( _soundDecoder ) == false )
        {
            LOGGER_ERROR( "OALSoundSystem: Failed to create sound buffer from stream"
            );

            return nullptr;
        }

        return base;
    }
    //////////////////////////////////////////////////////////////////////////
    ALuint OALSoundSystem::genSourceId()
    {
        ALuint sourceId = 0;
        alGenSources( 1, &sourceId );
        OAL_CHECK_ERROR();

        return sourceId;
    }
    //////////////////////////////////////////////////////////////////////////
    void OALSoundSystem::releaseSourceId( ALuint _sourceId )
    {
        alDeleteSources( 1, &_sourceId );
        OAL_CHECK_ERROR();
    }
    //////////////////////////////////////////////////////////////////////////
    ALuint OALSoundSystem::genBufferId()
    {
        ALuint bufferId = 0;
        alGenBuffers( 1, &bufferId );
        OAL_CHECK_ERROR();

        return bufferId;
    }
    //////////////////////////////////////////////////////////////////////////
    void OALSoundSystem::releaseBufferId( ALuint _bufferId )
    {
        alDeleteBuffers( 1, &_bufferId );
        OAL_CHECK_ERROR();
    }
}
