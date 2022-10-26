#include "Win32ThreadIdentity.h"

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

        void Win32SetThreadName( DWORD _dwThreadID, const Char * _threadName )
        {
            MENGINE_UNUSED( _dwThreadID );
            MENGINE_UNUSED( _threadName );

#if defined(MENGINE_DEBUG) && defined(MENGINE_TOOLCHAIN_MSVC) && defined(MENGINE_WINDOWS_SUPPORT_MIN_VERSION_VISTA)
            THREADNAME_INFO info;
            info.dwType = 0x1000;
            info.szName = _threadName;
            info.dwThreadID = _dwThreadID;
            info.dwFlags = 0;
#pragma warning(push)
#pragma warning(disable: 6320 6322)  
            __try
            {
                ::RaiseException( MS_VC_EXCEPTION, 0, sizeof( info ) / sizeof( ULONG_PTR ), (ULONG_PTR *)&info );
            }
            __except( EXCEPTION_EXECUTE_HANDLER )
            {
            }
#pragma warning(pop)  
#endif
        }
        //////////////////////////////////////////////////////////////////////////
    }
}