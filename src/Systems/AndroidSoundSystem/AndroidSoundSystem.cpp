#include "AndroidSoundSystem.h"

#include "AndroidSoundBuffer.h"
#include "AndroidSoundSource.h"

#include "Kernel/AssertionFactory.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/Logger.h"
#include "Kernel/ThreadMutexHelper.h"
#include "Kernel/ThreadMutexScope.h"

#include <algorithm>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SERVICE_FACTORY( SoundSystem, Mengine::AndroidSoundSystem );
    //////////////////////////////////////////////////////////////////////////
    AndroidSoundSystem::AndroidSoundSystem()
        : m_engineObject( nullptr )
        , m_engineEngine( nullptr )
        , m_outputMixObject( nullptr )
        , m_soundEnabled( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidSoundSystem::~AndroidSoundSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundSystem::_initializeService()
    {
        m_factorySoundBuffer = Helper::makeFactoryPoolWithMutex<AndroidSoundBuffer, 32>( MENGINE_DOCUMENT_FACTORABLE );
        m_factorySoundSource = Helper::makeFactoryPoolWithMutex<AndroidSoundSource, 32>( MENGINE_DOCUMENT_FACTORABLE );

        m_sourcesMutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        if( m_sourcesMutex == nullptr )
        {
            LOGGER_ERROR( "AndroidSoundSystem invalid create sources mutex" );

            return false;
        }

        SLresult result_create = slCreateEngine( &m_engineObject, 0, nullptr, 0, nullptr, nullptr );

        if( result_create != SL_RESULT_SUCCESS )
        {
            LOGGER_ERROR( "AndroidSoundSystem invalid create engine [%d]", result_create );

            return false;
        }

        SLresult result_realize_engine = ( *m_engineObject )->Realize( m_engineObject, SL_BOOLEAN_FALSE );

        if( result_realize_engine != SL_RESULT_SUCCESS )
        {
            LOGGER_ERROR( "AndroidSoundSystem invalid realize engine [%d]", result_realize_engine );

            ( *m_engineObject )->Destroy( m_engineObject );
            m_engineObject = nullptr;

            return false;
        }

        SLresult result_engine_interface = ( *m_engineObject )->GetInterface( m_engineObject, SL_IID_ENGINE, &m_engineEngine );

        if( result_engine_interface != SL_RESULT_SUCCESS )
        {
            LOGGER_ERROR( "AndroidSoundSystem invalid get engine interface [%d]", result_engine_interface );

            ( *m_engineObject )->Destroy( m_engineObject );
            m_engineObject = nullptr;
            m_engineEngine = nullptr;

            return false;
        }

        SLresult result_create_output_mix = ( *m_engineEngine )->CreateOutputMix( m_engineEngine, &m_outputMixObject, 0, nullptr, nullptr );

        if( result_create_output_mix != SL_RESULT_SUCCESS )
        {
            LOGGER_ERROR( "AndroidSoundSystem invalid create output mix [%d]", result_create_output_mix );

            ( *m_engineObject )->Destroy( m_engineObject );
            m_engineObject = nullptr;
            m_engineEngine = nullptr;

            return false;
        }

        SLresult result_realize_output = ( *m_outputMixObject )->Realize( m_outputMixObject, SL_BOOLEAN_FALSE );

        if( result_realize_output != SL_RESULT_SUCCESS )
        {
            LOGGER_ERROR( "AndroidSoundSystem invalid realize output mix [%d]", result_realize_output );

            ( *m_outputMixObject )->Destroy( m_outputMixObject );
            m_outputMixObject = nullptr;

            ( *m_engineObject )->Destroy( m_engineObject );
            m_engineObject = nullptr;
            m_engineEngine = nullptr;

            return false;
        }

        LOGGER_INFO( "android_sound", "AndroidSoundSystem initialized" );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundSystem::_finalizeService()
    {
        m_sources.clear();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factorySoundSource );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factorySoundBuffer );

        m_factorySoundSource = nullptr;
        m_factorySoundBuffer = nullptr;

        m_sourcesMutex = nullptr;

        if( m_outputMixObject != nullptr )
        {
            ( *m_outputMixObject )->Destroy( m_outputMixObject );
            m_outputMixObject = nullptr;
        }

        if( m_engineObject != nullptr )
        {
            ( *m_engineObject )->Destroy( m_engineObject );
            m_engineObject = nullptr;
            m_engineEngine = nullptr;
        }

        LOGGER_INFO( "android_sound", "AndroidSoundSystem finalized" );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundSystem::_stopService()
    {
        this->foreachSoundSource_( []( AndroidSoundSource * _source )
        {
            _source->stop();
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundSystem::update()
    {
        this->foreachSoundSource_( []( AndroidSoundSource * _source )
        {
            _source->updateFromSystem();
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidSoundSystem::isSilent() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundSystem::onTurnSound( bool _turn )
    {
        m_soundEnabled = _turn;

        this->foreachSoundSource_( [this]( AndroidSoundSource * _source )
        {
            if( m_soundEnabled == false )
            {
                _source->pause();
            }
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    SoundBufferInterfacePtr AndroidSoundSystem::createSoundBuffer( const SoundDecoderInterfacePtr & _decoder, bool _streamable, const DocumentInterfacePtr & _doc )
    {
        AndroidSoundBufferPtr buffer = m_factorySoundBuffer->createObject( _doc );

        if( buffer->load( _decoder, _streamable ) == false )
        {
            LOGGER_ERROR( "AndroidSoundSystem invalid load sound buffer" );

            return nullptr;
        }

        return buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    SoundSourceInterfacePtr AndroidSoundSystem::createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _sample, const DocumentInterfacePtr & _doc )
    {
        AndroidSoundSourcePtr source = m_factorySoundSource->createObject( _doc );

        source->initialize( this );
        source->setHeadMode( _isHeadMode );
        source->setSoundBuffer( _sample );

        return source;
    }
    //////////////////////////////////////////////////////////////////////////
    SLEngineItf AndroidSoundSystem::getSLEngine() const
    {
        return m_engineEngine;
    }
    //////////////////////////////////////////////////////////////////////////
    SLObjectItf AndroidSoundSystem::getOutputMixObject() const
    {
        return m_outputMixObject;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundSystem::registerSoundSource( AndroidSoundSource * _source )
    {
        ThreadMutexScope scope( m_sourcesMutex );

        m_sources.emplace_back( _source );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundSystem::unregisterSoundSource( AndroidSoundSource * _source )
    {
        ThreadMutexScope scope( m_sourcesMutex );

        VectorAndroidSoundSources::iterator it_found = std::find( m_sources.begin(), m_sources.end(), _source );

        if( it_found == m_sources.end() )
        {
            return;
        }

        m_sources.erase( it_found );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidSoundSystem::foreachSoundSource_( const Lambda<void( AndroidSoundSource * )> & _lambda )
    {
        ThreadMutexScope scope( m_sourcesMutex );

        for( AndroidSoundSource * source : m_sources )
        {
            _lambda( source );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}

