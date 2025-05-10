#include "SDL2ThreadSystem.h"

#include "SDL2ThreadIdentity.h"
#include "SDL2ThreadProcessor.h"
#include "SDL2ThreadMutex.h"
#include "SDL2ThreadSharedMutex.h"
#include "SDL2ThreadConditionVariable.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ThreadSystem, Mengine::SDL2ThreadSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDL2ThreadSystem::SDL2ThreadSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDL2ThreadSystem::~SDL2ThreadSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2ThreadSystem::_initializeService()
    {
        SDL2ThreadMutexPtr mutex = Helper::makeFactorableUnique<SDL2ThreadMutex>( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex, "invalid create main mutex" );

        if( mutex->initialize() == false )
        {
            LOGGER_ERROR( "invalid initialize main mutex" );

            return false;
        }

        m_factoryThreadIdentity = Helper::makeFactoryPoolWithMutex<SDL2ThreadIdentity, 16>( mutex, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryThreadProcessor = Helper::makeFactoryPoolWithMutex<SDL2ThreadProcessor, 16>( mutex, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryThreadMutex = Helper::makeFactoryPoolWithMutex<SDL2ThreadMutex, 16>( mutex, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryThreadSharedMutex = Helper::makeFactoryPoolWithMutex<SDL2ThreadSharedMutex, 16>( mutex, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryThreadConditionVariable = Helper::makeFactoryPoolWithMutex<SDL2ThreadConditionVariable, 16>( mutex, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2ThreadSystem::_finalizeService()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryThreadIdentity );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryThreadProcessor );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryThreadMutex );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryThreadSharedMutex );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryThreadConditionVariable );

        m_factoryThreadIdentity = nullptr;
        m_factoryThreadProcessor = nullptr;
        m_factoryThreadMutex = nullptr;
        m_factoryThreadSharedMutex = nullptr;
        m_factoryThreadConditionVariable = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadIdentityInterfacePtr SDL2ThreadSystem::createThreadIdentity( const ThreadDescription & _description, EThreadPriority _priority, const DocumentInterfacePtr & _doc )
    {
        SDL2ThreadIdentityPtr identity = m_factoryThreadIdentity->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( identity, "invalid create thread identity '%s' (doc: %s)"
            , _description.nameA
            , MENGINE_DOCUMENT_STR( _doc )
        );

        if( identity->initialize( _description, _priority ) == false )
        {
            LOGGER_ERROR( "invalid initialize thread '%s' (doc: %s)"
                , _description.nameA
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return nullptr;
        }

        return identity;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadProcessorInterfacePtr SDL2ThreadSystem::createThreadProcessor( const ThreadDescription & _description, EThreadPriority _priority, const DocumentInterfacePtr & _doc )
    {
        SDL2ThreadProcessorPtr identity = m_factoryThreadProcessor->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( identity, "invalid create thread processor '%s' (doc: %s)"
            , _description.nameA
            , MENGINE_DOCUMENT_STR( _doc )
        );

        ThreadMutexInterfacePtr mutex = this->createMutex( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex, "invalid create mutex" );

        if( identity->initialize( _description, _priority, mutex ) == false )
        {
            LOGGER_ERROR( "invalid initialize thread '%s' (doc: %s)"
                , _description.nameA
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return nullptr;
        }

        return identity;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2ThreadSystem::sleep( uint32_t _ms )
    {
        SDL_Delay( _ms );
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadMutexInterfacePtr SDL2ThreadSystem::createMutex( const DocumentInterfacePtr & _doc )
    {
        SDL2ThreadMutexPtr mutex = m_factoryThreadMutex->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex, "invalid create mutex (doc: '%s')"
            , MENGINE_DOCUMENT_STR( _doc )
        );

        if( mutex->initialize() == false )
        {
            LOGGER_ERROR( "invalid initialize mutex (doc: '%s')"
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return nullptr;
        }

        return mutex;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadSharedMutexInterfacePtr SDL2ThreadSystem::createSharedMutex( const DocumentInterfacePtr & _doc )
    {
        SDL2ThreadSharedMutexPtr mutex = m_factoryThreadSharedMutex->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex, "invalid create shared mutex (doc: '%s')"
            , MENGINE_DOCUMENT_STR( _doc )
        );

        if( mutex->initialize() == false )
        {
            LOGGER_ERROR( "invalid initialize shared mutex (doc: '%s')"
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return nullptr;
        }

        return mutex;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadConditionVariableInterfacePtr SDL2ThreadSystem::createConditionVariable( const DocumentInterfacePtr & _doc )
    {
        SDL2ThreadConditionVariablePtr conditionVariable = m_factoryThreadConditionVariable->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( conditionVariable, "invalid create condition variable" );

        if( conditionVariable->initialize() == false )
        {
            LOGGER_ERROR( "invalid initialize condition variable (doc: '%s')"
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return nullptr;
        }

        return conditionVariable;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadId SDL2ThreadSystem::getCurrentThreadId() const
    {
        SDL_threadID threadId = SDL_ThreadID();

        return static_cast<ThreadId>(threadId);
    }
    //////////////////////////////////////////////////////////////////////////
}
