#include "Win32Register.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/Logger.h"
#include "Kernel/UnicodeHelper.h"

namespace Mengine
{
    namespace Helper
    {
        bool Win32GetLocalMachineRegValue( const WChar * _path, const WChar * _key, Char * const _value, size_t _size )
        {
            HKEY hKey;
            LSTATUS lRes = ::RegOpenKeyExW( HKEY_LOCAL_MACHINE, _path, 0, KEY_READ, &hKey );

            if( lRes == ERROR_FILE_NOT_FOUND )
            {
                lRes = ::RegOpenKeyExW( HKEY_LOCAL_MACHINE, _path, 0, KEY_READ | KEY_WOW64_64KEY, &hKey );
            }

            if( lRes != ERROR_SUCCESS )
            {
                LOGGER_ERROR( "RegOpenKeyExW HKEY_LOCAL_MACHINE '%s' get Error [%ld]"
                    , _path
                    , lRes
                );

                return false;
            }

            WChar unicode_value[1024] = {L'\0'};
            DWORD dwBufferSize = 1024;
            LSTATUS nError = ::RegQueryValueExW( hKey, _key, 0, NULL, (LPBYTE)unicode_value, &dwBufferSize );

            ::RegCloseKey( hKey );

            if( nError != ERROR_SUCCESS )
            {
                LOGGER_ERROR( "RegQueryValueExW HKEY_LOCAL_MACHINE '%s' get Error [%ld]"
                    , _path
                    , nError
                );

                return false;
            }

            Helper::unicodeToUtf8( unicode_value, _value, _size );

            return true;
        }
    }
}