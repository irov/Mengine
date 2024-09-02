#include "Win32ThreadSystem.h"

#include "Win32ThreadIdentity.h"
#include "Win32ThreadProcessor.h"
#include "Win32ThreadMutex.h"
#include "Win32ThreadSharedMutex.h"
#include "Win32ThreadConditionVariable.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Kernel/Logger.h"

#include "Config/Algorithm.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ThreadSystem, Mengine::Win32ThreadSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32ThreadSystem::Win32ThreadSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32ThreadSystem::~Win32ThreadSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32ThreadSystem::_initializeService()
    {
        Win32ThreadMutexPtr mutex = Helper::makeFactorableUnique<Win32ThreadMutex>( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex, "invalid create mutex" );

        if( mutex->initialize() == false )
        {
            LOGGER_ERROR( "invalid initialize mutex" );

            return false;
        }

        m_factoryThreadMutex = Helper::makeFactoryPoolWithMutex<Win32ThreadMutex, 16>( mutex, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryThreadIdentity = Helper::makeFactoryPoolWithMutex<Win32ThreadIdentity, 16>( mutex, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryThreadProcessor = Helper::makeFactoryPoolWithMutex<Win32ThreadProcessor, 16>( mutex, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryThreadSharedMutex = Helper::makeFactoryPoolWithMutex<Win32ThreadSharedMutex, 16>( mutex, MENGINE_DOCUMENT_FACTORABLE );
        m_factoryThreadConditionVariable = Helper::makeFactoryPoolWithMutex<Win32ThreadConditionVariable, 16>( mutex, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadSystem::_finalizeService()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryThreadIdentity );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryThreadProcessor );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryThreadMutex );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryThreadSharedMutex );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryThreadConditionVariable );

        m_factoryThreadIdentity->setMutex( nullptr );
        m_factoryThreadProcessor->setMutex( nullptr );
        m_factoryThreadMutex->setMutex( nullptr );
        m_factoryThreadSharedMutex->setMutex( nullptr );
        m_factoryThreadConditionVariable->setMutex( nullptr );

        m_factoryThreadIdentity = nullptr;
        m_factoryThreadProcessor = nullptr;
        m_factoryThreadMutex = nullptr;
        m_factoryThreadSharedMutex = nullptr;
        m_factoryThreadConditionVariable = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadIdentityInterfacePtr Win32ThreadSystem::createThreadIdentity( const ConstString & _name, EThreadPriority _priority, const DocumentInterfacePtr & _doc )
    {
        Win32ThreadIdentityPtr threadIdentity = m_factoryThreadIdentity->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( threadIdentity, "invalid create identity" );

        if( threadIdentity->initialize( _name, _priority ) == false )
        {
            LOGGER_ERROR( "invalid thread identity initialize (doc: %s)"
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return nullptr;
        }

        return threadIdentity;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadProcessorInterfacePtr Win32ThreadSystem::createThreadProcessor( const ConstString & _name, EThreadPriority _priority, const DocumentInterfacePtr & _doc )
    {
        Win32ThreadProcessorPtr threadProcessor = m_factoryThreadProcessor->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( threadProcessor, "invalid create identity" );

        ThreadMutexInterfacePtr mutex = this->createMutex( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex, "invalid create mutex" );

        if( threadProcessor->initialize( _name, _priority, mutex ) == false )
        {
            LOGGER_ERROR( "invalid thread processor initialize (doc: %s)"
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return nullptr;
        }

        return threadProcessor;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadMutexInterfacePtr Win32ThreadSystem::createMutex( const DocumentInterfacePtr & _doc )
    {
        Win32ThreadMutexPtr mutex = m_factoryThreadMutex->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex, "invalid create mutex" );

        if( mutex->initialize() == false )
        {
            LOGGER_ERROR( "invalid initialize mutex (doc %s)"
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return nullptr;
        }

        return mutex;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadSharedMutexInterfacePtr Win32ThreadSystem::createSharedMutex( const DocumentInterfacePtr & _doc )
    {
        Win32ThreadSharedMutexPtr mutex = m_factoryThreadSharedMutex->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex, "invalid create shared mutex" );

        if( mutex->initialize() == false )
        {
            LOGGER_ERROR( "invalid initialize shared mutex (doc %s)"
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return nullptr;
        }

        return mutex;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadConditionVariableInterfacePtr Win32ThreadSystem::createConditionVariable( const DocumentInterfacePtr & _doc )
    {
        Win32ThreadConditionVariablePtr conditionVariable = m_factoryThreadConditionVariable->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( conditionVariable, "invalid create condition variable" );

        if( conditionVariable->initialize() == false )
        {
            LOGGER_ERROR( "invalid initialize condition variable (doc %s)"
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return nullptr;
        }

        return conditionVariable;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadSystem::sleep( uint32_t _ms )
    {
        ::Sleep( _ms );
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadId Win32ThreadSystem::getCurrentThreadId() const
    {
        DWORD id = ::GetCurrentThreadId();

        return (ThreadId)id;
    }
    //////////////////////////////////////////////////////////////////////////
}
