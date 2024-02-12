#include "Win32Helper.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Config/StdString.h"
#include "Config/StdIO.h"

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
        ThreadId Win32GetCurrentThreadId()
        {
            DWORD id = ::GetCurrentThreadId();

            return (ThreadId)id;
        }
        //////////////////////////////////////////////////////////////////////////
        const WChar * Win32GetErrorMessage( uint32_t _messageId )
        {
            static MENGINE_THREAD_LOCAL WChar errorMessageBuffer[2048] = {L'\0'};

            if( ::FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS
                , NULL
                , (DWORD)_messageId
                , MAKELANGID( LANG_ENGLISH, SUBLANG_ENGLISH_US )
                , (LPTSTR)errorMessageBuffer
                , 2048
                , NULL ) == 0 )
            {
                MENGINE_WNSPRINTF( errorMessageBuffer, 2048, L"#Error FormatMessage [%u]#"
                    , _messageId
                );
            }

            return errorMessageBuffer;
        }
        //////////////////////////////////////////////////////////////////////////
        const WChar * Win32GetLastErrorMessage()
        {
            DWORD error = ::GetLastError();

            const WChar * errorMessage = Helper::Win32GetErrorMessage( error );

            static MENGINE_THREAD_LOCAL WChar errorMessageBufferWithErrorCode[2048] = {L'\0'};

            MENGINE_WNSPRINTF( errorMessageBufferWithErrorCode, 2048, L"%ls [%lu]"
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
        const Char * Win32GetVersionName()
        {
            HMODULE hNtdll = GetModuleHandleW( L"ntdll.dll" );
            
            if( hNtdll == NULL )
            {
                return "Unknown";
            }
            
            pRtlGetVersion rtlGetVersion = (pRtlGetVersion)::GetProcAddress( hNtdll, "RtlGetVersion" );

            if( rtlGetVersion == NULL )
            {
                return "Unknown";
            }

            OSVERSIONINFOEXW osInfo;
            osInfo.dwOSVersionInfoSize = sizeof( osInfo );
            if( rtlGetVersion( &osInfo ) < 0 )
            {
                return "Unknown";
            }

            // Check major version              
            switch( osInfo.dwMajorVersion )
            {
            case 10:
                {
                    switch( osInfo.dwMinorVersion )
                    {
                    case 0:
                        {
                            if( osInfo.wProductType == VER_NT_WORKSTATION )
                            {
                                if( osInfo.dwBuildNumber >= 22000 )
                                {
                                    return "11";
                                }
                                else
                                {
                                    return "10";
                                }
                            }
                            else
                            {
                                return "Server 2016";
                            }
                        }break;
                    }
                }break;
            case 6:
                {
                    switch( osInfo.dwMinorVersion )
                    {
                    case 3:
                        return "8.1";
                    case 2:
                        return "8";
                    case 1:
                        return "7";
                    case 0:
                        return "Vista";
                    }
                }break;
            case 5:
                {
                    switch( osInfo.dwMinorVersion )
                    {
                    case 2:
                        return "XP64";
                    case 1:
                        return "XP";
                    }
                }break;
            }
            
            return "Unknown";
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
