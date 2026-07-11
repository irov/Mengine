#include "OpenSLESSoundSystem.h"

#include "OpenSLESSoundSource.h"
#include "OpenSLESSoundBufferMemory.h"
#include "OpenSLESSoundBufferStream.h"

#include "Interface/ThreadServiceInterface.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/FactoryPoolWithListener.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SoundSystem, Mengine::OpenSLESSoundSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OpenSLESSoundSystem::OpenSLESSoundSystem()
        : m_engineObject( nullptr )
        , m_engineItf( nullptr )
        , m_outputMixObject( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OpenSLESSoundSystem::~OpenSLESSoundSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenSLESSoundSystem::_initializeService()
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

        LOGGER_MESSAGE( "OpenSLESSoundSystem initialized with OpenSL ES" );

        m_factoryOpenSLESSoundBuffer = Helper::makeFactoryPoolWithMutex<OpenSLESSoundBufferMemory, 32>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryOpenSLESSoundBufferStream = Helper::makeFactoryPoolWithMutex<OpenSLESSoundBufferStream, 32>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryOpenSLESSoundSource = Helper::makeFactoryPoolWithMutexAndListener<OpenSLESSoundSource, 32>( this, &OpenSLESSoundSystem::onDestroyOpenSLESSoundSource_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenSLESSoundSystem::_finalizeService()
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

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryOpenSLESSoundBuffer );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryOpenSLESSoundBufferStream );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryOpenSLESSoundSource );

        m_factoryOpenSLESSoundBuffer = nullptr;
        m_factoryOpenSLESSoundBufferStream = nullptr;
        m_factoryOpenSLESSoundSource = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenSLESSoundSystem::update()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool OpenSLESSoundSystem::isSilent() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    SoundSourceInterfacePtr OpenSLESSoundSystem::createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _buffer, const DocumentInterfacePtr & _doc )
    {
        OpenSLESSoundSourcePtr soundSource = m_factoryOpenSLESSoundSource->createObject( _doc );

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
    SoundBufferInterfacePtr OpenSLESSoundSystem::createSoundBuffer( const SoundDecoderInterfacePtr & _decoder, bool _isStream, const DocumentInterfacePtr & _doc )
    {
        OpenSLESSoundBufferBasePtr base = nullptr;

        if( _isStream == false || SERVICE_IS_INITIALIZE( ThreadServiceInterface ) == false )
        {
            OpenSLESSoundBufferMemoryPtr buffer = m_factoryOpenSLESSoundBuffer->createObject( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( buffer, "invalid create sound buffer" );

            base = buffer;
        }
        else
        {
            OpenSLESSoundBufferStreamPtr buffer = m_factoryOpenSLESSoundBufferStream->createObject( _doc );

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
    SLEngineItf OpenSLESSoundSystem::getEngineInterface() const
    {
        return m_engineItf;
    }
    //////////////////////////////////////////////////////////////////////////
    SLObjectItf OpenSLESSoundSystem::getOutputMixObject() const
    {
        return m_outputMixObject;
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenSLESSoundSystem::onDestroyOpenSLESSoundSource_( OpenSLESSoundSource * _soundSource )
    {
        _soundSource->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
}
