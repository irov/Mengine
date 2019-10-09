#include "OpenALSoundSystem.h"
#include "OpenALSoundError.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/ThreadServiceInterface.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"

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

            OPENAL_CALL( alcCloseDevice, (m_device) );
            m_device = nullptr;

            return false;
        }

        RET_OPENAL_CALL( ALCboolean, currentResult, alcMakeContextCurrent, (m_context) );

        if( currentResult == ALC_FALSE )
        {
            LOGGER_ERROR( "Failed to make context current" );

            OPENAL_CALL( alcDestroyContext, (m_context) );
            m_context = nullptr;

            OPENAL_CALL( alcCloseDevice, (m_device) );
            m_device = nullptr;

            return false;
        }


        ALCint majorVersion;
        OPENAL_CALL( alcGetIntegerv, (m_device, ALC_MAJOR_VERSION, 1, &majorVersion) );

        ALCint minorVersion;
        OPENAL_CALL( alcGetIntegerv, (m_device, ALC_MINOR_VERSION, 1, &minorVersion) );

        LOGGER_MESSAGE_RELEASE( "OpenAL version %u.%u"
            , majorVersion
            , minorVersion
        );

        RET_OPENAL_CALL( const ALCchar *, defaultDeviceSprcifier, alcGetString, (m_device, ALC_DEVICE_SPECIFIER) );

        LOGGER_MESSAGE_RELEASE( "OpenAL default device specifier [%s]"
            , defaultDeviceSprcifier
        );

        RET_OPENAL_CALL( const ALCchar *, captureDeviceSpecifier, alcGetString, (m_device, ALC_CAPTURE_DEVICE_SPECIFIER) );

        LOGGER_MESSAGE_RELEASE( "OpenAL capture device specifier [%s]"
            , captureDeviceSpecifier
        );

        LOGGER_MESSAGE_RELEASE( "OpenAL driver properties" );

        LOGGER_MESSAGE_RELEASE( "Version: %s"
            , alGetString( AL_VERSION )
        );

        LOGGER_MESSAGE_RELEASE( "Vendor: %s"
            , alGetString( AL_VENDOR )
        );

        LOGGER_MESSAGE_RELEASE( "Renderer: %s"
            , alGetString( AL_RENDERER )
        );

        float lposition[] = { 0.f, 0.f, 0.f };
        OPENAL_CALL( alListenerfv, (AL_POSITION, lposition) );

        float lvelocity[] = { 0.f, 0.f, 0.f };
        OPENAL_CALL( alListenerfv, (AL_VELOCITY, lvelocity) );

        float lorient[] = { 0.f, 0.f, 1.f, 0.f, 1.f, 0.f };
        OPENAL_CALL( alListenerfv, (AL_ORIENTATION, lorient) );

        m_factoryOpenALSoundBuffer = Helper::makeFactoryPool<OpenALSoundBufferMemory, 32>();
        m_factoryOpenALSoundBufferStream = Helper::makeFactoryPool<OpenALSoundBufferStream, 32>();
        m_factoryOpenALSoundSource = Helper::makeFactoryPool<OpenALSoundSource, 32>();

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
        MENGINE_UNUSED( _turn );

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
        OpenALSoundSourcePtr soundSource = m_factoryOpenALSoundSource->createObject( MENGINE_DOCUMENT_FUNCTION );

        soundSource->setSoundSystem( this );

        soundSource->setHeadMode( _isHeadMode );
        soundSource->setSoundBuffer( _buffer );

        return soundSource;
    }
    //////////////////////////////////////////////////////////////////////////
    SoundBufferInterfacePtr OpenALSoundSystem::createSoundBuffer( const SoundDecoderInterfacePtr & _soundDecoder, bool _isStream )
    {
        OpenALSoundBufferBasePtr base = nullptr;

        if( _isStream == false || SERVICE_IS_AVAILABLE( ThreadServiceInterface ) == false )
        {
            OpenALSoundBufferMemoryPtr buffer = m_factoryOpenALSoundBuffer->createObject( MENGINE_DOCUMENT_FUNCTION );

            if( buffer->initialize( this ) == false )
            {
                return nullptr;
            }

            base = buffer;
        }
        else
        {
            OpenALSoundBufferStreamPtr buffer = m_factoryOpenALSoundBufferStream->createObject( MENGINE_DOCUMENT_FUNCTION );

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
        OPENAL_CALL( alGenSources, (1, &sourceId) );

        return sourceId;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundSystem::releaseSourceId( ALuint _sourceId )
    {
        OPENAL_CALL( alDeleteSources, (1, &_sourceId) );
    }
    //////////////////////////////////////////////////////////////////////////
    ALuint OpenALSoundSystem::genBufferId()
    {
        ALuint bufferId = 0;
        OPENAL_CALL( alGenBuffers, (1, &bufferId) );

        return bufferId;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundSystem::releaseBufferId( ALuint _bufferId )
    {
        OPENAL_CALL( alDeleteBuffers, (1, &_bufferId) );
    }
}
