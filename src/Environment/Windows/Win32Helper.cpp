#include "Win32Helper.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/PluginHelper.h"

#include "Config/StdString.h"
#include "Config/StdIO.h"
#include "Config/Path.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        typedef NTSTATUS( WINAPI * pRtlGetVersion )(LPOSVERSIONINFOEXW);
        //////////////////////////////////////////////////////////////////////////
        namespace Detail
        {
            //////////////////////////////////////////////////////////////////////////
            static bool __Win32GetCurrentDllPath( Char * const _path )
            {
                MENGINE_UNUSED( _path );

#if defined(MENGINE_PLATFORM_WINDOWS) && !defined(MENGINE_PLATFORM_UWP)
                HMODULE hm = NULL;

                if( ::GetModuleHandleExA( GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, PLUGIN_FACTORY_CREATE_FUNCTION_NAME, &hm ) == 0 )
                {
                    return false;
                }

                WPath unicode_path = {L'\0'};
                if( ::GetModuleFileName( hm, unicode_path, MENGINE_MAX_PATH ) == 0 )
                {
                    return false;
                }

#   if (WINVER >= 0x0600)
                DWORD dwConversionFlags = WC_ERR_INVALID_CHARS;
#   else
                DWORD dwConversionFlags = 0;
#   endif

                size_t unicode_path_len = StdString::wcslen( unicode_path );

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
        void Win32Sleep( uint32_t _ms )
        {
            ::Sleep( _ms );
        }
        //////////////////////////////////////////////////////////////////////////
        PathString Win32GetCurrentDllPath()
        {
            static MENGINE_THREAD_LOCAL Path dllPath = {'\0'};

            if( dllPath[0] == '\0' )
            {
                if( Detail::__Win32GetCurrentDllPath( dllPath ) == false )
                {
                    StdString::strcpy_safe( dllPath, "Unsupport", MENGINE_MAX_PATH );
                }
            }

            return PathString( dllPath );
        }
        //////////////////////////////////////////////////////////////////////////
        ThreadId Win32GetCurrentThreadId()
        {
            DWORD id = ::GetCurrentThreadId();

            return (ThreadId)id;
        }
        //////////////////////////////////////////////////////////////////////////
        void Win32ReadErrorMessageA( uint32_t _id, Char * const _message, size_t _capacity )
        {
            if( ::FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS
                , NULL
                , (DWORD)_id
                , MAKELANGID( LANG_ENGLISH, SUBLANG_ENGLISH_US )
                , (LPSTR)_message
                , (DWORD)_capacity
                , NULL ) == 0 )
            {
                DWORD lastError = ::GetLastError();

                MENGINE_SNPRINTF( _message, _capacity, "error format message [%u] error: %u"
                    , _id
                    , lastError
                );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void Win32ReadErrorMessageW( uint32_t _id, WChar * const _message, size_t _capacity )
        {
            if( ::FormatMessageW( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS
                , NULL
                , (DWORD)_id
                , MAKELANGID( LANG_ENGLISH, SUBLANG_ENGLISH_US )
                , (LPWSTR)_message
                , (DWORD)_capacity
                , NULL ) == 0 )
            {
                DWORD lastError = ::GetLastError();

                MENGINE_SWPRINTF( _message, _capacity, L"error format message [%u] error: %u"
                    , _id
                    , lastError
                );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        const Char * Win32GetErrorMessageA( uint32_t _id )
        {
            static MENGINE_THREAD_LOCAL Char errorMessageBuffer[2048 + 1] = {'\0'};

            Helper::Win32ReadErrorMessageA( _id, errorMessageBuffer, 2048 );

            return errorMessageBuffer;
        }
        //////////////////////////////////////////////////////////////////////////
        const WChar * Win32GetErrorMessageW( uint32_t _id )
        {
            static MENGINE_THREAD_LOCAL WChar errorMessageBuffer[2048 + 1] = {L'\0'};

            Helper::Win32ReadErrorMessageW( _id, errorMessageBuffer, 2048 );

            return errorMessageBuffer;
        }
        //////////////////////////////////////////////////////////////////////////
        const Char * Win32GetLastErrorMessageA()
        {
            DWORD error = ::GetLastError();

            const Char * errorMessage = Helper::Win32GetErrorMessageA( error );

            static MENGINE_THREAD_LOCAL Char errorMessageBufferWithErrorCode[2048 + 1] = {'\0'};

            MENGINE_SNPRINTF( errorMessageBufferWithErrorCode, 2048, "%s [%lu]"
                , errorMessage
                , error
            );

            return errorMessageBufferWithErrorCode;
        }
        //////////////////////////////////////////////////////////////////////////
        const WChar * Win32GetLastErrorMessageW()
        {
            DWORD error = ::GetLastError();

            const WChar * errorMessage = Helper::Win32GetErrorMessageW( error );

            static MENGINE_THREAD_LOCAL WChar errorMessageBufferWithErrorCode[2048 + 1] = {L'\0'};

            MENGINE_SWPRINTF( errorMessageBufferWithErrorCode, 2048, L"%ls [%lu]"
                , errorMessage
                , error
            );

            return errorMessageBufferWithErrorCode;
        }
        //////////////////////////////////////////////////////////////////////////
        void Win32Toast( const WChar * _message )
        {
            MENGINE_UNUSED( _message );

#if !defined(MENGINE_PLATFORM_UWP)
            ::MessageBoxW( NULL, _message, L"Mengine", MB_OK );
#endif
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
