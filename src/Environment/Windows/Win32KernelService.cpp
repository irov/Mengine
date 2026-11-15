#include "Win32KernelService.h"

#include "Interface/ThreadSystemInterface.h"

#include "Kernel/ThreadMutexScope.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( Win32KernelService, Mengine::Win32KernelService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        ////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_DEBUG) && defined(MENGINE_TOOLCHAIN_MSVC) && defined(MENGINE_WINDOWS_SUPPORT_MIN_VERSION_VISTA)
#endif
        //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_DEBUG) && defined(MENGINE_TOOLCHAIN_MSVC) && defined(MENGINE_WINDOWS_SUPPORT_MIN_VERSION_VISTA)
        //////////////////////////////////////////////////////////////////////////
        const DWORD MS_VC_EXCEPTION = 0x406D1388;
        //////////////////////////////////////////////////////////////////////////
#pragma pack(push,8)  
        typedef struct tagTHREADNAME_INFO
        {
            DWORD dwType; // Must be 0x1000.  
            LPCSTR szName; // Pointer to name (in user addr space).  
            DWORD dwThreadID; // Thread ID (-1=caller thread).  
            DWORD dwFlags; // Reserved for future use, must be zero.  
        } THREADNAME_INFO;
#pragma pack(pop)
        //////////////////////////////////////////////////////////////////////////
        static void __Win32SetThreadName( DWORD _dwThreadId, const Char * _threadName )
        {
            THREADNAME_INFO info;
            info.dwType = 0x1000;
            info.szName = _threadName;
            info.dwThreadID = _dwThreadId;
            info.dwFlags = 0;
#pragma warning(push)
#pragma warning(disable: 6320 6322)
            ::RaiseException( MS_VC_EXCEPTION, 0, sizeof( info ) / sizeof( ULONG_PTR ), (ULONG_PTR *)&info );
#pragma warning(pop)
        }
        //////////////////////////////////////////////////////////////////////////
#endif
        //////////////////////////////////////////////////////////////////////////
        static void Win32SetThreadName( DWORD _dwThreadId, const Char * _threadName )
        {
            MENGINE_UNUSED( _dwThreadId );
            MENGINE_UNUSED( _threadName );

#if defined(MENGINE_DEBUG) && defined(MENGINE_TOOLCHAIN_MSVC) && defined(MENGINE_WINDOWS_SUPPORT_MIN_VERSION_VISTA)
            __try
            {
                __Win32SetThreadName( _dwThreadId, _threadName );
            }
            __except( EXCEPTION_EXECUTE_HANDLER )
            {
            }
#endif
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    Win32KernelService::Win32KernelService()
        : m_WINAPI_SetThreadDescription( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32KernelService::~Win32KernelService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32KernelService::_initializeService()
    {
        m_semaphoresMutex = THREAD_SYSTEM()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        if( m_semaphoresMutex == nullptr )
        {
            return false;
        }

        HMODULE kernel32Module = ::GetModuleHandleW( L"kernel32.dll" );

        if( kernel32Module == nullptr )
        {
            return true;
        }

        m_WINAPI_SetThreadDescription = (FWINAPI_SetThreadDescription)::GetProcAddress( kernel32Module, "SetThreadDescription" );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32KernelService::_finalizeService()
    {
        this->clearSemaphores();

        m_semaphoresMutex = nullptr;
        m_WINAPI_SetThreadDescription = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32KernelService::setThreadDescription( HANDLE _thread, const ThreadDescription & _description ) const
    {
        if( m_WINAPI_SetThreadDescription != nullptr )
        {
            m_WINAPI_SetThreadDescription( _thread, _description.nameW );
        }
        else
        {
            if( ::IsDebuggerPresent() == TRUE )
            {
                DWORD threadId = ::GetThreadId( _thread );

                Detail::Win32SetThreadName( threadId, _description.nameA );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32KernelService::activateSemaphore( const ConstString & _name, const ParamVariant & _value )
    {
        VectorSemaphoreListeners listeners;
        ParamVariant value = _value;

        {
            MENGINE_THREAD_MUTEX_SCOPE( m_semaphoresMutex );

            SemaphoreDesc & semaphore = m_semaphores[_name];

            if( semaphore.activated == true )
            {
                return;
            }

            semaphore.activated = true;
            semaphore.value = value;
            listeners.swap( semaphore.listeners );
        }

        for( const SemaphoreListenerInterfacePtr & listener : listeners )
        {
            listener->invoke( value );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32KernelService::deactivateSemaphore( const ConstString & _name )
    {
        VectorSemaphoreListeners listeners;
        ParamVariant value;

        {
            MENGINE_THREAD_MUTEX_SCOPE( m_semaphoresMutex );

            MapSemaphores::iterator itSemaphore = m_semaphores.find( _name );

            if( itSemaphore == m_semaphores.end() )
            {
                return;
            }

            listeners.swap( itSemaphore->second.listeners );
            value = itSemaphore->second.value;
            m_semaphores.erase( itSemaphore );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32KernelService::waitSemaphore( const ConstString & _name, const SemaphoreListenerInterfacePtr & _listener )
    {
        ParamVariant value;

        {
            MENGINE_THREAD_MUTEX_SCOPE( m_semaphoresMutex );

            SemaphoreDesc & semaphore = m_semaphores[_name];

            if( semaphore.activated == false )
            {
                semaphore.listeners.emplace_back( _listener );

                return;
            }

            value = semaphore.value;
        }

        _listener->invoke( value );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32KernelService::clearSemaphores()
    {
        MapSemaphores semaphores;

        {
            MENGINE_THREAD_MUTEX_SCOPE( m_semaphoresMutex );

            semaphores.swap( m_semaphores );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
