#include "OpenALSoundSystem.h"
#include "OpenALSoundErrorHelper.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/ThreadServiceInterface.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/FactoryPoolWithListener.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionStatistic.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/StatisticHelper.h"

#include "Config/Algorithm.h"

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
        ALCdevice * device = alcOpenDevice( nullptr );

        if( device == nullptr )
        {
            LOGGER_ASSERTION( "failed to open 'default' sound device" );

            device = alcOpenDevice( "Generic Software" );

            if( device == nullptr )
            {
                LOGGER_ASSERTION( "failed to open 'Generic Software' sound device" );

                device = alcOpenDevice( "Generic Hardware" );

                if( device == nullptr )
                {
                    LOGGER_ASSERTION( "failed to open 'Generic Hardware' sound device" );

                    return false;
                }
            }
        }

        m_device = device;

        ALCcontext * context = alcCreateContext( m_device, nullptr );

        if( context == nullptr )
        {
            LOGGER_ASSERTION( "failed to create context" );

            MENGINE_OPENAL_CALL( alcCloseDevice, (m_device) );
            m_device = nullptr;

            return false;
        }

        m_context = context;

        MENGINE_RET_OPENAL_CALL( ALCboolean, currentResult, alcMakeContextCurrent, (m_context) );

        if( currentResult == ALC_FALSE )
        {
            LOGGER_ASSERTION( "failed to make context current" );

            MENGINE_OPENAL_CALL( alcDestroyContext, (m_context) );
            m_context = nullptr;

            MENGINE_OPENAL_CALL( alcCloseDevice, (m_device) );
            m_device = nullptr;

            return false;
        }

        ALCint majorVersion;
        MENGINE_OPENAL_CALL( alcGetIntegerv, (m_device, ALC_MAJOR_VERSION, 1, &majorVersion) );

        ALCint minorVersion;
        MENGINE_OPENAL_CALL( alcGetIntegerv, (m_device, ALC_MINOR_VERSION, 1, &minorVersion) );

        LOGGER_MESSAGE( "OpenAL version: %d.%d"
            , majorVersion
            , minorVersion
        );

        MENGINE_RET_OPENAL_CALL( const ALCchar *, defaultDeviceSpecifier, alcGetString, (m_device, ALC_DEVICE_SPECIFIER) );

        LOGGER_MESSAGE( "OpenAL device specifier: %s"
            , defaultDeviceSpecifier
        );

        LOGGER_INFO( "openal", "OpenAL version: %s"
            , alGetString( AL_VERSION )
        );

        LOGGER_INFO( "openal", "OpenAL vendor: %s"
            , alGetString( AL_VENDOR )
        );

        LOGGER_INFO( "openal", "OpenAL renderer: %s"
            , alGetString( AL_RENDERER )
        );

        LOGGER_INFO( "openal", "OpenAL extensions: %s"
            , alGetString( AL_EXTENSIONS )
        );

        ALfloat lposition[] = { 0.f, 0.f, 0.f };
        MENGINE_OPENAL_CALL( alListenerfv, (AL_POSITION, lposition) );

        ALfloat lvelocity[] = { 0.f, 0.f, 0.f };
        MENGINE_OPENAL_CALL( alListenerfv, (AL_VELOCITY, lvelocity) );

        ALfloat lorient[] = { 0.f, 0.f, -1.f, 0.f, 1.f, 0.f };
        MENGINE_OPENAL_CALL( alListenerfv, (AL_ORIENTATION, lorient) );
        
        m_factoryOpenALSoundBuffer = Helper::makeFactoryPool<OpenALSoundBufferMemory, 32>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryOpenALSoundBufferStream = Helper::makeFactoryPool<OpenALSoundBufferStream, 32>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryOpenALSoundSource = Helper::makeFactoryPoolWithListener<OpenALSoundSource, 32>( this, &OpenALSoundSystem::onDestroyOpenALSoundSource_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundSystem::_finalizeService()
    {
        MENGINE_ASSERTION_STATISTIC_EMPTY( STATISTIC_SOUND_SOURCE_COUNT );
        MENGINE_ASSERTION_STATISTIC_EMPTY( STATISTIC_SOUND_BUFFER_COUNT );

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
    SoundSourceInterfacePtr OpenALSoundSystem::createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _buffer, const DocumentInterfacePtr & _doc )
    {
        OpenALSoundSourcePtr soundSource = m_factoryOpenALSoundSource->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( soundSource );

        soundSource->setSoundSystem( this );

        soundSource->setHeadMode( _isHeadMode );
        soundSource->setSoundBuffer( _buffer );

        if( soundSource->initialize() == false )
        {
            LOGGER_ASSERTION( "invalid sound source initialize" );

            return nullptr;
        }

        return soundSource;
    }
    //////////////////////////////////////////////////////////////////////////
    SoundBufferInterfacePtr OpenALSoundSystem::createSoundBuffer( const SoundDecoderInterfacePtr & _soundDecoder, bool _isStream, const DocumentInterfacePtr & _doc )
    {
        OpenALSoundBufferBasePtr base = nullptr;

        if( _isStream == false || SERVICE_IS_INITIALIZE( ThreadServiceInterface ) == false )
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
            LOGGER_ASSERTION( "failed to create sound buffer from stream" );

            return nullptr;
        }

        return base;
    }
    //////////////////////////////////////////////////////////////////////////
    ALuint OpenALSoundSystem::genSourceId()
    {
        ALuint sourceId = 0;
        MENGINE_OPENAL_CALL( alGenSources, (1, &sourceId) );

        if( sourceId == 0 )
        {
            return 0U;
        }

        STATISTIC_INC_INTEGER( STATISTIC_SOUND_SOURCE_COUNT );
        STATISTIC_INC_INTEGER( STATISTIC_SOUND_SOURCE_NEW );

        return sourceId;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundSystem::releaseSourceId( ALuint _sourceId )
    {
        if( _sourceId == 0 )
        {
            return;
        }

        MENGINE_OPENAL_CALL( alDeleteSources, (1, &_sourceId) );

        STATISTIC_DEC_INTEGER( STATISTIC_SOUND_SOURCE_COUNT );
        STATISTIC_INC_INTEGER( STATISTIC_SOUND_SOURCE_FREE );
    }
    //////////////////////////////////////////////////////////////////////////
    ALuint OpenALSoundSystem::genBufferId()
    {
        ALuint bufferId = 0;
        MENGINE_OPENAL_CALL( alGenBuffers, (1, &bufferId) );

        if( bufferId == 0 )
        {
            return 0U;
        }

        STATISTIC_INC_INTEGER( STATISTIC_SOUND_BUFFER_COUNT );
        STATISTIC_INC_INTEGER( STATISTIC_SOUND_BUFFER_NEW );

        return bufferId;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundSystem::releaseBufferId( ALuint _bufferId )
    {
        if( _bufferId == 0 )
        {
            return;
        }

        MENGINE_OPENAL_CALL( alDeleteBuffers, (1, &_bufferId) );

        STATISTIC_DEC_INTEGER( STATISTIC_SOUND_BUFFER_COUNT );
        STATISTIC_INC_INTEGER( STATISTIC_SOUND_BUFFER_FREE );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenALSoundSystem::onDestroyOpenALSoundSource_( OpenALSoundSource * _soundSource )
    {
        _soundSource->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
}
