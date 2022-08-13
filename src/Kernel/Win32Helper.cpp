#include "Win32Helper.h"

#include "Config/StdString.h"
#include "Config/StdIO.h"

#ifdef MENGINE_PLATFORM_WINDOWS
#   include "Interface/PlatformInterface.h"
#   include "Interface/Win32PlatformExtensionInterface.h"

#   include "Environment/Windows/WindowsIncluder.h"
#endif

namespace Mengine
{
    namespace Helper
    {
        namespace Detail
        {
            //////////////////////////////////////////////////////////////////////////
            static bool __Win32GetCurrentDllPath( Char * const _path )
            {
                MENGINE_UNUSED( _path );

#if defined(MENGINE_PLATFORM_WINDOWS) && !defined(MENGINE_WINDOWS_UNIVERSAL)
                HMODULE hm = NULL;

                if( ::GetModuleHandleEx( GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                    GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                    L"dllCreatePlugin", &hm ) == 0 )
                {
                    return false;
                }

                WChar unicode_path[MENGINE_MAX_PATH] = {L'\0'};
                if( ::GetModuleFileName( hm, unicode_path, MENGINE_MAX_PATH ) == 0 )
                {
                    return false;
                }

#   if (WINVER >= 0x0600)
                DWORD dwConversionFlags = WC_ERR_INVALID_CHARS;
#   else
                DWORD dwConversionFlags = 0;
#   endif

                size_t unicode_path_len = MENGINE_WCSLEN( unicode_path );

                int32_t utf8_size = ::WideCharToMultiByte(
                    CP_UTF8
                    , dwConversionFlags
                    , unicode_path
                    , (int32_t)unicode_path_len
                    , _path
                    , MENGINE_MAX_PATH
                    , NULL
                    , NULL
                );

                _path[utf8_size] = '\0';

                return true;
#else
                return false;
#endif
            }
            //////////////////////////////////////////////////////////////////////////
        }
        //////////////////////////////////////////////////////////////////////////
        const Char * Win32GetCurrentDllPath()
        {
            static MENGINE_THREAD_LOCAL Char dllPath[MENGINE_MAX_PATH] = {'\0'};

            if( dllPath[0] == '\0' )
            {
                if( Detail::__Win32GetCurrentDllPath( dllPath ) == false )
                {
                    MENGINE_STRCPY( dllPath, "Unsupport" );
                }
            }

            return dllPath;
        }
        //////////////////////////////////////////////////////////////////////////
        uint64_t Win32GetCurrentThreadId()
        {
#if defined(MENGINE_PLATFORM_WINDOWS)
            DWORD id = ::GetCurrentThreadId();

            return (uint64_t)id;
#else
            return 0;
#endif
        }
        //////////////////////////////////////////////////////////////////////////
        const WChar * Win32GetErrorMessage( uint32_t _messageId )
        {
            MENGINE_UNUSED( _messageId );

#if defined(MENGINE_PLATFORM_WINDOWS) && defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32)
            static MENGINE_THREAD_LOCAL WChar errorMessageBuffer[2048] = {L'\0'};

            if( ::FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS
                , NULL
                , (DWORD)_messageId
                , MAKELANGID( LANG_ENGLISH, SUBLANG_ENGLISH_US )
                , (LPTSTR)errorMessageBuffer
                , 2048
                , NULL ) != 0 )
            {
                MENGINE_WSPRINTF( errorMessageBuffer, L"#Error FormatMessage [%u]#"
                    , _messageId
                );
            }

            return errorMessageBuffer;
#else
            return L"";
#endif
        }
        //////////////////////////////////////////////////////////////////////////
        const WChar * Win32GetLastErrorMessage()
        {
#if defined(MENGINE_PLATFORM_WINDOWS) && defined(MENGINE_ENVIRONMENT_PLATFORM_WIN32)
            DWORD error = ::GetLastError();

            const WChar * errorMessage = Win32GetErrorMessage( error );

            return errorMessage;
#else
            return L"";
#endif
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
