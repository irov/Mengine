#include "SDL3ThreadSystem.h"

#include "SDL3ThreadIdentity.h"
#include "SDL3ThreadProcessor.h"
#include "SDL3ThreadMutex.h"
#include "SDL3ThreadSharedMutex.h"
#include "SDL3ThreadConditionVariable.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ThreadSystem, Mengine::SDL3ThreadSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDL3ThreadSystem::SDL3ThreadSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDL3ThreadSystem::~SDL3ThreadSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3ThreadSystem::_initializeService()
    {
        SDL3ThreadMutexPtr mutex = Helper::makeFactorableUnique<SDL3ThreadMutex>( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex, "invalid create main mutex" );

        if( mutex->initialize() == false )
        {
            LOGGER_ERROR( "invalid initialize main mutex" );

            return false;
        }

        m_factoryThreadIdentity = Helper::makeFactoryPoolWithMutex<SDL3ThreadIdentity, 16>( mutex, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryThreadProcessor = Helper::makeFactoryPoolWithMutex<SDL3ThreadProcessor, 16>( mutex, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryThreadMutex = Helper::makeFactoryPoolWithMutex<SDL3ThreadMutex, 16>( mutex, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryThreadSharedMutex = Helper::makeFactoryPoolWithMutex<SDL3ThreadSharedMutex, 16>( mutex, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryThreadConditionVariable = Helper::makeFactoryPoolWithMutex<SDL3ThreadConditionVariable, 16>( mutex, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3ThreadSystem::_finalizeService()
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
    ThreadIdentityInterfacePtr SDL3ThreadSystem::createThreadIdentity( const ThreadDescription & _description, EThreadPriority _priority, const DocumentInterfacePtr & _doc )
    {
        SDL3ThreadIdentityPtr identity = m_factoryThreadIdentity->createObject( _doc );

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
    ThreadProcessorInterfacePtr SDL3ThreadSystem::createThreadProcessor( const ThreadDescription & _description, EThreadPriority _priority, const DocumentInterfacePtr & _doc )
    {
        SDL3ThreadProcessorPtr identity = m_factoryThreadProcessor->createObject( _doc );

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
    void SDL3ThreadSystem::sleep( uint32_t _ms )
    {
        SDL_Delay( _ms );
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadMutexInterfacePtr SDL3ThreadSystem::createMutex( const DocumentInterfacePtr & _doc )
    {
        SDL3ThreadMutexPtr mutex = m_factoryThreadMutex->createObject( _doc );

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
    ThreadSharedMutexInterfacePtr SDL3ThreadSystem::createSharedMutex( const DocumentInterfacePtr & _doc )
    {
        SDL3ThreadSharedMutexPtr mutex = m_factoryThreadSharedMutex->createObject( _doc );

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
    ThreadConditionVariableInterfacePtr SDL3ThreadSystem::createConditionVariable( const DocumentInterfacePtr & _doc )
    {
        SDL3ThreadConditionVariablePtr conditionVariable = m_factoryThreadConditionVariable->createObject( _doc );

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
    ThreadId SDL3ThreadSystem::getCurrentThreadId() const
    {
        SDL_ThreadID threadId = SDL_GetCurrentThreadID();

        return static_cast<ThreadId>(threadId);
    }
    //////////////////////////////////////////////////////////////////////////
}



