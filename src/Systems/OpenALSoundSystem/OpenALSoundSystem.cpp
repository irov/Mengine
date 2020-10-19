#include "OpenALSoundSystem.h"
#include "OpenALSoundError.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/ThreadServiceInterface.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/FactoryPoolWithListener.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"

#include <algorithm>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SoundSystem, Mengine::OpenALSoundSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OpenALSoundSystem::OpenALSoundSystem()
        : m_context( nullptr )
        , m_device( nullptr )
        , m_sourcesCount( 0 )
        , m_buffersCount( 0 )
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

        ALCdevice * device = alcOpenDevice( nullptr );

        if( device == nullptr )
        {
            LOGGER_ERROR( "Failed to open 'generic software' sound device try default..." );

            device = alcOpenDevice( "Generic Software" );

            if( device == nullptr )
            {
                LOGGER_ERROR( "Failed to open default sound device try hardware" );

                device = alcOpenDevice( "Generic Hardware" );

                if( device == nullptr )
                {
                    LOGGER_ERROR( "Failed to open hardware sound device.." );

                    return false;
                }
            }
        }

        m_device = device;

        ALCcontext * context = alcCreateContext( m_device, nullptr );

        if( context == nullptr )
        {
            LOGGER_ERROR( "Failed to create context" );

            OPENAL_CALL( alcCloseDevice, (m_device) );
            m_device = nullptr;

            return false;
        }

        m_context = context;

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

        LOGGER_MESSAGE_RELEASE( "OpenAL version %d.%d"
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

        LOGGER_MESSAGE_RELEASE( "OpenAL version: %s"
            , alGetString( AL_VERSION )
        );

        LOGGER_MESSAGE_RELEASE( "OpenAL vendor: %s"
            , alGetString( AL_VENDOR )
        );

        LOGGER_MESSAGE_RELEASE( "OpenAL renderer: %s"
            , alGetString( AL_RENDERER )
        );

        ALfloat lposition[] = { 0.f, 0.f, 0.f };
        OPENAL_CALL( alListenerfv, (AL_POSITION, lposition) );

        ALfloat lvelocity[] = { 0.f, 0.f, 0.f };
        OPENAL_CALL( alListenerfv, (AL_VELOCITY, lvelocity) );

        ALfloat lorient[] = { 0.f, 0.f, -1.f, 0.f, 1.f, 0.f };
        OPENAL_CALL( alListenerfv, (AL_ORIENTATION, lorient) );
        
        m_factoryOpenALSoundBuffer = Helper::makeFactoryPool<OpenALSoundBufferMemory, 32>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryOpenALSoundBufferStream = Helper::makeFactoryPool<OpenALSoundBufferStream, 32>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryOpenALSoundSource = Helper::makeFactoryPoolWithListener<OpenALSoundSource, 32>( this, &OpenALSoundSystem::onDestroyOpenALSoundSource_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundSystem::_finalizeService()
    {
        MENGINE_ASSERTION_FATAL( m_sourcesCount == 0 );
        MENGINE_ASSERTION_FATAL( m_buffersCount == 0 );

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
    SoundSourceInterfacePtr OpenALSoundSystem::createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _buffer, const DocumentPtr & _doc )
    {
        OpenALSoundSourcePtr soundSource = m_factoryOpenALSoundSource->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( soundSource );

        soundSource->setSoundSystem( this );

        soundSource->setHeadMode( _isHeadMode );
        soundSource->setSoundBuffer( _buffer );

        if( soundSource->initialize() == false )
        {
            LOGGER_ERROR( "invalid sound source initialize" );

            return nullptr;
        }

        return soundSource;
    }
    //////////////////////////////////////////////////////////////////////////
    SoundBufferInterfacePtr OpenALSoundSystem::createSoundBuffer( const SoundDecoderInterfacePtr & _soundDecoder, bool _isStream, const DocumentPtr & _doc )
    {
        OpenALSoundBufferBasePtr base = nullptr;

        if( _isStream == false || SERVICE_IS_AVAILABLE( ThreadServiceInterface ) == false )
        {
            OpenALSoundBufferMemoryPtr buffer = m_factoryOpenALSoundBuffer->createObject( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( buffer );

            buffer->setSoundSystem( this );

            base = buffer;
        }
        else
        {
            OpenALSoundBufferStreamPtr buffer = m_factoryOpenALSoundBufferStream->createObject( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( buffer );

            buffer->setSoundSystem( this );

            base = buffer;
        }

        if( base->load( _soundDecoder ) == false )
        {
            LOGGER_ERROR( "Failed to create sound buffer from stream" );

            return nullptr;
        }

        return base;
    }
    //////////////////////////////////////////////////////////////////////////
    ALuint OpenALSoundSystem::genSourceId()
    {
        ALuint sourceId = 0;
        OPENAL_CALL( alGenSources, (1, &sourceId) );

        if( sourceId == 0 )
        {
            return 0U;
        }

        ++m_sourcesCount;

        return sourceId;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundSystem::releaseSourceId( ALuint _sourceId )
    {
        if( _sourceId == 0 )
        {
            return;
        }

        OPENAL_CALL( alDeleteSources, (1, &_sourceId) );

        --m_sourcesCount;
    }
    //////////////////////////////////////////////////////////////////////////
    ALuint OpenALSoundSystem::genBufferId()
    {
        ALuint bufferId = 0;
        OPENAL_CALL( alGenBuffers, (1, &bufferId) );

        if( bufferId == 0 )
        {
            return 0U;
        }

        ++m_buffersCount;

        return bufferId;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundSystem::releaseBufferId( ALuint _bufferId )
    {
        if( _bufferId == 0 )
        {
            return;
        }

        OPENAL_CALL( alDeleteBuffers, (1, &_bufferId) );

        --m_buffersCount;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t OpenALSoundSystem::getSourcesCount() const
    {
        return m_sourcesCount;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t OpenALSoundSystem::getBuffersCount() const
    {
        return m_buffersCount;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundSystem::onDestroyOpenALSoundSource_( OpenALSoundSource * _soundSource )
    {
        _soundSource->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
}
