#include "Win32Helper.h"

#include "Config/StdString.h"

#ifdef MENGINE_PLATFORM_WINDOWS
#   include "Environment/Windows/WindowsIncluder.h"
#endif

namespace Mengine
{
    namespace Helper
    {
        namespace Detail
        {
            static bool __Win32GetCurrentDllPath( Char * const _path )
            {
#ifdef MENGINE_PLATFORM_WINDOWS
                WCHAR unicode_path[MENGINE_MAX_PATH] = { L'\0' };
                HMODULE hm = NULL;

                if( GetModuleHandleEx( GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                    GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                    L"dllCreatePlugin", &hm ) == 0 )
                {
                    return false;
                }

                if( GetModuleFileName( hm, unicode_path, sizeof( unicode_path ) ) == 0 )
                {
                    return false;
                }

#   if (WINVER >= 0x0600)
                DWORD dwConversionFlags = WC_ERR_INVALID_CHARS;
#   else
                DWORD dwConversionFlags = 0;
#   endif

                size_t unicode_path_len = MENGINE_WCSLEN( unicode_path );

                int utf8_size = ::WideCharToMultiByte(
                    CP_UTF8
                    , dwConversionFlags
                    , unicode_path
                    , (int)unicode_path_len
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
        }
        //////////////////////////////////////////////////////////////////////////
        const Char * Win32GetCurrentDllPath()
        {
            static Char dllPath[MENGINE_MAX_PATH] = { '\0' };

            if( dllPath[0] == '\0' )
            {
                if( Detail::__Win32GetCurrentDllPath( dllPath ) == false )
                {
                    MENGINE_STRCPY( dllPath, "Unsupport" );
                }
            }

            return dllPath;
        }
    }
}
