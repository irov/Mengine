#include "Win32ThreadSystem.h"

#include "Win32ThreadIdentity.h"
#include "Win32ThreadProcessor.h"
#include "Win32ThreadMutex.h"
#include "Win32ThreadSharedMutex.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"

#include "Config/Algorithm.h"

#include <process.h>

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
        m_factoryWin32ThreadIdentity = Helper::makeFactoryPool<Win32ThreadIdentity, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryWin32ThreadProcessor = Helper::makeFactoryPool<Win32ThreadProcessor, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryWin32ThreadMutex = Helper::makeFactoryPool<Win32ThreadMutex, 16>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryWin32ThreadSharedMutex = Helper::makeFactoryPool<Win32ThreadSharedMutex, 16>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadSystem::_finalizeService()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryWin32ThreadIdentity );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryWin32ThreadProcessor );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryWin32ThreadMutex );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryWin32ThreadSharedMutex );

        m_factoryWin32ThreadIdentity = nullptr;
        m_factoryWin32ThreadProcessor = nullptr;
        m_factoryWin32ThreadMutex = nullptr;
        m_factoryWin32ThreadSharedMutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadIdentityInterfacePtr Win32ThreadSystem::createThreadIdentity( const ConstString & _name, EThreadPriority _priority, const DocumentInterfacePtr & _doc )
    {
        Win32ThreadIdentityPtr threadIdentity = m_factoryWin32ThreadIdentity->createObject( _doc );

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
        Win32ThreadProcessorPtr threadProcessor = m_factoryWin32ThreadProcessor->createObject( _doc );

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
        Win32ThreadMutexPtr mutex = m_factoryWin32ThreadMutex->createObject( _doc );

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
        Win32ThreadSharedMutexPtr mutex = m_factoryWin32ThreadSharedMutex->createObject( _doc );

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
