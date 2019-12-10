#include "Win32ThreadSystem.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"

#include <algorithm>

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
        m_factoryWin32ThreadMutex = Helper::makeFactoryPool<Win32ThreadMutex, 16>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadSystem::_finalizeService()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryWin32ThreadIdentity );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryWin32ThreadMutex );

        m_factoryWin32ThreadIdentity = nullptr;
        m_factoryWin32ThreadMutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadIdentityInterfacePtr Win32ThreadSystem::createThread( const ConstString & _name, int32_t _priority, const DocumentPtr & _doc )
    {
        Win32ThreadIdentityPtr identity = m_factoryWin32ThreadIdentity->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( identity, nullptr, "invalid create identity" );

        ThreadMutexInterfacePtr mutex = this->createMutex( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex, nullptr, "invalid create mutex" );

        if( identity->initialize( _name, _priority, mutex, _doc ) == false )
        {
            LOGGER_ERROR( "invalid identity initialize"
            );

            return nullptr;
        }

        return identity;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadMutexInterfacePtr Win32ThreadSystem::createMutex( const DocumentPtr & _doc )
    {
        Win32ThreadMutexPtr mutex = m_factoryWin32ThreadMutex->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex, nullptr, "invalid create mutex" );

        mutex->initialize( _doc );

        return mutex;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadSystem::sleep( uint32_t _ms )
    {
        ::Sleep( _ms );
    }
    //////////////////////////////////////////////////////////////////////////
    ptrdiff_t Win32ThreadSystem::getCurrentThreadId() const
    {
        DWORD id = GetCurrentThreadId();

        return (ptrdiff_t)id;
    }
}
