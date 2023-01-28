#include "SDLThreadSystem.h"

#include "SDLThreadIdentity.h"
#include "SDLThreadProcessor.h"
#include "SDLThreadMutex.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ThreadSystem, Mengine::SDLThreadSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDLThreadSystem::SDLThreadSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLThreadSystem::~SDLThreadSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLThreadSystem::_initializeService()
    {
        m_factoryThreadIdentity = Helper::makeFactoryPool<SDLThreadIdentity, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryThreadProcessor = Helper::makeFactoryPool<SDLThreadProcessor, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryThreadMutex = Helper::makeFactoryPool<SDLThreadMutex, 16>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLThreadSystem::_finalizeService()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryThreadIdentity );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryThreadProcessor );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryThreadMutex );

        m_factoryThreadIdentity = nullptr;
        m_factoryThreadProcessor = nullptr;
        m_factoryThreadMutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadIdentityInterfacePtr SDLThreadSystem::createThreadIdentity( const ConstString & _name, EThreadPriority _priority, const DocumentPtr & _doc )
    {
        SDLThreadIdentityPtr identity = m_factoryThreadIdentity->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( identity, "invalid create thread '%s' (doc: %s)"
            , _name.c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        if( identity->initialize( _priority, _name, _doc ) == false )
        {
            LOGGER_ERROR( "invalid initialize thread '%s' (doc: %s)"
                , _name.c_str()
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return nullptr;
        }

        return identity;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadProcessorInterfacePtr SDLThreadSystem::createThreadProcessor( const ConstString & _name, EThreadPriority _priority, const DocumentPtr & _doc )
    {
        SDLThreadProcessorPtr identity = m_factoryThreadProcessor->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( identity, "invalid create thread '%s' (doc: %s)"
            , _name.c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        ThreadMutexInterfacePtr mutex = this->createMutex( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex, "invalid create mutex" );

        if( identity->initialize( _priority, _name, mutex, _doc ) == false )
        {
            LOGGER_ERROR( "invalid initialize thread '%s' (doc: %s)"
                , _name.c_str()
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return nullptr;
        }

        return identity;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLThreadSystem::sleep( uint32_t _ms )
    {
        SDL_Delay( _ms );
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadMutexInterfacePtr SDLThreadSystem::createMutex( const DocumentPtr & _doc )
    {
        SDLThreadMutexPtr mutex = m_factoryThreadMutex->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex, "invalid create (doc: '%s')"
            , MENGINE_DOCUMENT_STR( _doc )
        );

        if( mutex->initialize( _doc ) == false )
        {
            LOGGER_ERROR( "invalid initialize (doc: '%s')"
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return nullptr;
        }

        return mutex;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadId SDLThreadSystem::getCurrentThreadId() const
    {
        SDL_threadID threadId = SDL_ThreadID();

        return static_cast<ThreadId>(threadId);
    }
    //////////////////////////////////////////////////////////////////////////
}
