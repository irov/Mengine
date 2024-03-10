#include "SDLThreadSystem.h"

#include "SDLThreadIdentity.h"
#include "SDLThreadProcessor.h"
#include "SDLThreadMutex.h"
#include "SDLThreadSharedMutex.h"
#include "SDLThreadConditionVariable.h"

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
        SDLThreadMutexPtr mutex = Helper::makeFactorableUnique<SDLThreadMutex>( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex, "invalid create main mutex" );

        if( mutex->initialize() == false )
        {
            LOGGER_ERROR( "invalid initialize main mutex" );

            return false;
        }

        m_factoryThreadIdentity = Helper::makeFactoryPoolWithMutex<SDLThreadIdentity, 16>( mutex, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryThreadProcessor = Helper::makeFactoryPoolWithMutex<SDLThreadProcessor, 16>( mutex, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryThreadMutex = Helper::makeFactoryPoolWithMutex<SDLThreadMutex, 16>( mutex, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryThreadSharedMutex = Helper::makeFactoryPoolWithMutex<SDLThreadSharedMutex, 16>( mutex, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryThreadConditionVariable = Helper::makeFactoryPoolWithMutex<SDLThreadConditionVariable, 16>( mutex, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLThreadSystem::_finalizeService()
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
    ThreadIdentityInterfacePtr SDLThreadSystem::createThreadIdentity( const ConstString & _name, EThreadPriority _priority, const DocumentInterfacePtr & _doc )
    {
        SDLThreadIdentityPtr identity = m_factoryThreadIdentity->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( identity, "invalid create thread identity '%s' (doc: %s)"
            , _name.c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        if( identity->initialize( _priority, _name ) == false )
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
    ThreadProcessorInterfacePtr SDLThreadSystem::createThreadProcessor( const ConstString & _name, EThreadPriority _priority, const DocumentInterfacePtr & _doc )
    {
        SDLThreadProcessorPtr identity = m_factoryThreadProcessor->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( identity, "invalid create thread processor '%s' (doc: %s)"
            , _name.c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        ThreadMutexInterfacePtr mutex = this->createMutex( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex, "invalid create mutex" );

        if( identity->initialize( _priority, _name, mutex ) == false )
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
    ThreadMutexInterfacePtr SDLThreadSystem::createMutex( const DocumentInterfacePtr & _doc )
    {
        SDLThreadMutexPtr mutex = m_factoryThreadMutex->createObject( _doc );

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
    ThreadSharedMutexInterfacePtr SDLThreadSystem::createSharedMutex( const DocumentInterfacePtr & _doc )
    {
        SDLThreadSharedMutexPtr mutex = m_factoryThreadSharedMutex->createObject( _doc );

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
    ThreadConditionVariableInterfacePtr SDLThreadSystem::createConditionVariable( const DocumentInterfacePtr & _doc )
    {
        SDLThreadConditionVariablePtr conditionVariable = m_factoryThreadConditionVariable->createObject( _doc );

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
    ThreadId SDLThreadSystem::getCurrentThreadId() const
    {
        SDL_threadID threadId = SDL_ThreadID();

        return static_cast<ThreadId>(threadId);
    }
    //////////////////////////////////////////////////////////////////////////
}
