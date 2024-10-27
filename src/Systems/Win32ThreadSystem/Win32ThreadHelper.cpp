#include "Win32ThreadIdentity.h"

#include "Kernel/Assertion.h"

#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_DEBUG) && defined(MENGINE_TOOLCHAIN_MSVC) && defined(MENGINE_WINDOWS_SUPPORT_MIN_VERSION_VISTA)
const DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack(push,8)  
typedef struct tagTHREADNAME_INFO
{
    DWORD dwType; // Must be 0x1000.  
    LPCSTR szName; // Pointer to name (in user addr space).  
    DWORD dwThreadID; // Thread ID (-1=caller thread).  
    DWORD dwFlags; // Reserved for future use, must be zero.  
} THREADNAME_INFO;
#pragma pack(pop)
#endif
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_DEBUG) && defined(MENGINE_TOOLCHAIN_MSVC) && defined(MENGINE_WINDOWS_SUPPORT_MIN_VERSION_VISTA)
//////////////////////////////////////////////////////////////////////////
        static void __Win32SetThreadName( DWORD _dwThreadId, const Char * _threadName )
        {
            MENGINE_ASSERTION_FATAL( StdString::strlen( _threadName ) < MENGINE_MAX_THREAD_NAME, "thread name '%s' is too long (%d > %d)", _threadName, StdString::strlen( _threadName ), MENGINE_MAX_THREAD_NAME );

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
        void Win32SetThreadName( DWORD _dwThreadId, const Char * _threadName )
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
}