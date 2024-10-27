#include "Win32KernelService.h"

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
        : m_WINAPI_SetThreadDescription( NULL )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32KernelService::~Win32KernelService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32KernelService::_initializeService()
    {
        HMODULE hModuleKernel32 = ::GetModuleHandleW( L"kernel32.dll" );

        if( hModuleKernel32 != NULL )
        {
            m_WINAPI_SetThreadDescription = (FWINAPI_SetThreadDescription)::GetProcAddress( hModuleKernel32, "SetThreadDescription" );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32KernelService::_finalizeService()
    {
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
}