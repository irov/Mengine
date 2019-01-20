#include "Win32ThreadSystem.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Kernel/Logger.h"

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
        m_factoryWin32ThreadIdentity = new FactoryPool<Win32ThreadIdentity, 16>();
        m_factoryWin32ThreadMutex = new FactoryPool<Win32ThreadMutex, 16>();
        m_factoryWin32ThreadConditionVariable = new FactoryPool<Win32ThreadConditionVariable, 16, FactoryWithMutex>();

        m_mutexConditionVariable = this->createMutex( __FILE__, __LINE__ );

        m_factoryWin32ThreadConditionVariable->setMutex( m_mutexConditionVariable );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadSystem::_finalizeService()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryWin32ThreadIdentity );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryWin32ThreadMutex );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryWin32ThreadConditionVariable );

        m_factoryWin32ThreadIdentity = nullptr;
        m_factoryWin32ThreadMutex = nullptr;
        m_factoryWin32ThreadConditionVariable = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadIdentityInterfacePtr Win32ThreadSystem::createThread( int32_t _priority, const Char * _doc, const Char * _file, uint32_t _line )
    {
        Win32ThreadIdentityPtr identity = m_factoryWin32ThreadIdentity->createObject();

        if( identity == nullptr )
        {
            LOGGER_ERROR( "invalid create identity"
            );

            return nullptr;
        }

        ThreadMutexInterfacePtr mutex = this->createMutex( _file, _line );

        if( identity->initialize( mutex, _priority, _doc, _file, _line ) == false )
        {
            LOGGER_ERROR( "invalid initialize"
            );

            return nullptr;
        }

        return identity;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadMutexInterfacePtr Win32ThreadSystem::createMutex( const Char * _file, uint32_t _line )
    {
        Win32ThreadMutexPtr mutex = m_factoryWin32ThreadMutex->createObject();

        MENGINE_ASSERTION_MEMORY_PANIC( mutex, nullptr )("invalid create mutex");

        mutex->initialize( _file, _line );

        return mutex;
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadConditionVariableInterfacePtr Win32ThreadSystem::createConditionVariable( const Char * _file, uint32_t _line )
    {
        Win32ThreadConditionVariablePtr conditionVariable = m_factoryWin32ThreadConditionVariable->createObject();

        MENGINE_ASSERTION_MEMORY_PANIC( conditionVariable, nullptr )("invalid create condition variable");

        conditionVariable->initialize( _file, _line );

        return conditionVariable;
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
