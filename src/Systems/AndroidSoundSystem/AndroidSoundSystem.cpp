#include "AndroidSoundSystem.h"

#include "AndroidSoundSource.h"
#include "AndroidSoundBufferMemory.h"
#include "AndroidSoundBufferStream.h"

#include "Interface/ThreadServiceInterface.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/FactoryPoolWithListener.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SoundSystem, Mengine::AndroidSoundSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidSoundSystem::AndroidSoundSystem()
        : m_engineObject( nullptr )
        , m_engineItf( nullptr )
        , m_outputMixObject( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidSoundSystem::~AndroidSoundSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundSystem::_initializeService()
    {
        SLresult result;

        result = slCreateEngine( &m_engineObject, 0, nullptr, 0, nullptr, nullptr );

        if( result != SL_RESULT_SUCCESS )
        {
            LOGGER_ASSERTION( "failed to create OpenSL engine [%d]"
                , result
            );

            return false;
        }

        result = (*m_engineObject)->Realize( m_engineObject, SL_BOOLEAN_FALSE );

        if( result != SL_RESULT_SUCCESS )
        {
            LOGGER_ASSERTION( "failed to realize OpenSL engine [%d]"
                , result
            );

            (*m_engineObject)->Destroy( m_engineObject );
            m_engineObject = nullptr;

            return false;
        }

        result = (*m_engineObject)->GetInterface( m_engineObject, SL_IID_ENGINE, &m_engineItf );

        if( result != SL_RESULT_SUCCESS )
        {
            LOGGER_ASSERTION( "failed to get engine interface [%d]"
                , result
            );

            (*m_engineObject)->Destroy( m_engineObject );
            m_engineObject = nullptr;

            return false;
        }

        result = (*m_engineItf)->CreateOutputMix( m_engineItf, &m_outputMixObject, 0, nullptr, nullptr );

        if( result != SL_RESULT_SUCCESS )
        {
            LOGGER_ASSERTION( "failed to create output mix [%d]"
                , result
            );

            (*m_engineObject)->Destroy( m_engineObject );
            m_engineObject = nullptr;
            m_engineItf = nullptr;

            return false;
        }

        result = (*m_outputMixObject)->Realize( m_outputMixObject, SL_BOOLEAN_FALSE );

        if( result != SL_RESULT_SUCCESS )
        {
            LOGGER_ASSERTION( "failed to realize output mix [%d]"
                , result
            );

            (*m_outputMixObject)->Destroy( m_outputMixObject );
            m_outputMixObject = nullptr;

            (*m_engineObject)->Destroy( m_engineObject );
            m_engineObject = nullptr;
            m_engineItf = nullptr;

            return false;
        }

        LOGGER_MESSAGE( "AndroidSoundSystem initialized with OpenSL ES" );

        m_factoryAndroidSoundBuffer = Helper::makeFactoryPoolWithMutex<AndroidSoundBufferMemory, 32>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryAndroidSoundBufferStream = Helper::makeFactoryPoolWithMutex<AndroidSoundBufferStream, 32>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryAndroidSoundSource = Helper::makeFactoryPoolWithMutexAndListener<AndroidSoundSource, 32>( this, &AndroidSoundSystem::onDestroyAndroidSoundSource_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundSystem::_finalizeService()
    {
        if( m_outputMixObject != nullptr )
        {
            (*m_outputMixObject)->Destroy( m_outputMixObject );
            m_outputMixObject = nullptr;
        }

        if( m_engineObject != nullptr )
        {
            (*m_engineObject)->Destroy( m_engineObject );
            m_engineObject = nullptr;
            m_engineItf = nullptr;
        }

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAndroidSoundBuffer );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAndroidSoundBufferStream );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAndroidSoundSource );

        m_factoryAndroidSoundBuffer = nullptr;
        m_factoryAndroidSoundBufferStream = nullptr;
        m_factoryAndroidSoundSource = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundSystem::update()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundSystem::isSilent() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    SoundSourceInterfacePtr AndroidSoundSystem::createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _buffer, const DocumentInterfacePtr & _doc )
    {
        AndroidSoundSourcePtr soundSource = m_factoryAndroidSoundSource->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( soundSource, "invalid sound source create" );

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
    SoundBufferInterfacePtr AndroidSoundSystem::createSoundBuffer( const SoundDecoderInterfacePtr & _decoder, bool _isStream, const DocumentInterfacePtr & _doc )
    {
        AndroidSoundBufferBasePtr base = nullptr;

        if( _isStream == false || SERVICE_IS_INITIALIZE( ThreadServiceInterface ) == false )
        {
            AndroidSoundBufferMemoryPtr buffer = m_factoryAndroidSoundBuffer->createObject( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( buffer, "invalid create sound buffer" );

            base = buffer;
        }
        else
        {
            AndroidSoundBufferStreamPtr buffer = m_factoryAndroidSoundBufferStream->createObject( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( buffer, "invalid create sound buffer" );

            base = buffer;
        }

        if( base->load( _decoder ) == false )
        {
            LOGGER_ASSERTION( "failed to create sound buffer from stream" );

            return nullptr;
        }

        return base;
    }
    //////////////////////////////////////////////////////////////////////////
    SLEngineItf AndroidSoundSystem::getEngineInterface() const
    {
        return m_engineItf;
    }
    //////////////////////////////////////////////////////////////////////////
    SLObjectItf AndroidSoundSystem::getOutputMixObject() const
    {
        return m_outputMixObject;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundSystem::onDestroyAndroidSoundSource_( AndroidSoundSource * _soundSource )
    {
        _soundSource->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
}
