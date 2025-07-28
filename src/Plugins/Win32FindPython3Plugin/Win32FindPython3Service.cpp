#include "Win32FindPython3Service.h"

#include "Environment/Windows/WindowsIncluder.h"
#include "Environment/Windows/Win32Helper.h"

#include "Kernel/Logger.h"
#include "Kernel/UnicodeHelper.h"

#include "Config/StdIO.h"
#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( Win32FindPython3Service, Mengine::Win32FindPython3Service );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool isVersionGreater( int32_t _major, int32_t _minor, int32_t _major2, int32_t _minor2 )
        {
            if( _major > _major2 )
            {
                return true;
            }

            if( _major == _major2 && _minor > _minor2 )
            {
                return true;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    Win32FindPython3Service::Win32FindPython3Service()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32FindPython3Service::~Win32FindPython3Service()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FindPython3Service::_initializeService()
    {
        int32_t minorVersion = 0;
        int32_t majorVersion = 0;

        WPath Python3PathW = {L'\0'};
        HKEY hBases[] = {HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE};

        BOOL bPythonFound = FALSE;
        HKEY hInstallPathKeyFound = NULL;
        HKEY hPythonPathKeyFound = NULL;

        for( HKEY hBase : hBases )
        {
            HKEY hKey;
            if( ::RegOpenKeyEx( hBase, L"SOFTWARE\\Python\\PythonCore", 0, KEY_READ | KEY_WOW64_64KEY, &hKey ) != ERROR_SUCCESS )
            {
                continue;
            }

            for( DWORD index = 0;; ++index )
            {
                WPath subKeyName = {L'\0'};
                DWORD subKeyNameSize = MENGINE_MAX_PATH;
                FILETIME lastWriteTime;
                if( ::RegEnumKeyEx( hKey, index, subKeyName, &subKeyNameSize, NULL, NULL, NULL, &lastWriteTime ) != ERROR_SUCCESS )
                {
                    break;
                }

                WPath KeySysVersionPath = {L'\0'};
                MENGINE_SWPRINTF( KeySysVersionPath, MENGINE_MAX_PATH, L"SOFTWARE\\Python\\PythonCore\\%s", subKeyName );

                HKEY hKeySysVersion;
                if( ::RegOpenKeyEx( hBase, KeySysVersionPath, 0, KEY_READ | KEY_WOW64_64KEY, &hKeySysVersion ) != ERROR_SUCCESS )
                {
                    continue;
                }

                DWORD SysArchitectureType;
                WPath SysArchitecture = {L'\0'};
                DWORD SysArchitectureSize = MENGINE_MAX_PATH;
                if( ::RegQueryValueExW( hKeySysVersion, L"SysArchitecture", NULL, &SysArchitectureType, (LPBYTE)SysArchitecture, &SysArchitectureSize ) != ERROR_SUCCESS )
                {
                    continue;
                }

                if( StdString::wcscmp( SysArchitecture, L"64bit" ) != 0 )
                {
                    continue;
                }

                DWORD SysVersionType;
                WPath SysVersion = {L'\0'};
                DWORD SysVersionSize = MENGINE_MAX_PATH;
                if( ::RegQueryValueExW( hKeySysVersion, L"SysVersion", NULL, &SysVersionType, (LPBYTE)SysVersion, &SysVersionSize ) != ERROR_SUCCESS )
                {
                    continue;
                }

                int32_t major;
                int32_t minor;
                MENGINE_SWSCANF( SysVersion, L"%d.%d"
                    , &major
                    , &minor
                );

                if( Detail::isVersionGreater( major, minor, majorVersion, minorVersion ) == true )
                {
                    WPath subKeyPython3InstallPath = {L'\0'};
                    MENGINE_SWPRINTF( subKeyPython3InstallPath, MENGINE_MAX_PATH, L"SOFTWARE\\Python\\PythonCore\\%s\\InstallPath", subKeyName );

                    HKEY hInstallPathKey;
                    if( ::RegOpenKeyExW( hBase, subKeyPython3InstallPath, 0, KEY_READ | KEY_WOW64_64KEY, &hInstallPathKey ) != ERROR_SUCCESS )
                    {
                        LOGGER_ERROR( "invalid open registry key '%ls' error: %ls"
                            , subKeyPython3InstallPath
                            , Helper::Win32GetLastErrorMessageW()
                        );

                        return false;
                    }

                    WPath subKeyPython3PythonPath = {L'\0'};
                    MENGINE_SWPRINTF( subKeyPython3PythonPath, MENGINE_MAX_PATH, L"SOFTWARE\\Python\\PythonCore\\%s\\PythonPath", subKeyName );

                    HKEY hPythonPathKey;
                    if( ::RegOpenKeyExW( hBase, subKeyPython3PythonPath, 0, KEY_READ | KEY_WOW64_64KEY, &hPythonPathKey ) != ERROR_SUCCESS )
                    {
                        LOGGER_ERROR( "invalid open registry key '%ls' error: %ls"
                            , subKeyPython3PythonPath
                            , Helper::Win32GetLastErrorMessageW()
                        );

                        return false;
                    }

                    majorVersion = major;
                    minorVersion = minor;

                    hInstallPathKeyFound = hInstallPathKey;
                    hPythonPathKeyFound = hPythonPathKey;
                    bPythonFound = TRUE;
                }
            }
        }

        if( bPythonFound == FALSE )
        {
            LOGGER_ERROR( "not found python version" );

            return false;
        }

        WPath Python3PathInstallPathValue = {L'\0'};
        DWORD Python3PathInstallPathLen = MENGINE_MAX_PATH;
        LSTATUS nPython3PathInstallPathError = ::RegQueryValueExW( hInstallPathKeyFound, NULL, 0, NULL, (LPBYTE)Python3PathInstallPathValue, &Python3PathInstallPathLen );

        ::RegCloseKey( hInstallPathKeyFound );

        if( nPython3PathInstallPathError != ERROR_SUCCESS )
        {
            LOGGER_ERROR( "RegQueryValueExW 'hInstallPathKeyFound' get Error [%ld]"
                , nPython3PathInstallPathError
            );

            return false;
        }

        StdString::wcscpy_safe( Python3PathW, Python3PathInstallPathValue, MENGINE_MAX_PATH );

        WPath Python3PathPythonPathValue = {L'\0'};
        DWORD Python3PathPythonPathLen = MENGINE_MAX_PATH;
        LSTATUS nPython3PathPythonPathError = ::RegQueryValueExW( hPythonPathKeyFound, NULL, 0, NULL, (LPBYTE)Python3PathPythonPathValue, &Python3PathPythonPathLen );

        ::RegCloseKey( hPythonPathKeyFound );

        if( nPython3PathPythonPathError != ERROR_SUCCESS )
        {
            LOGGER_ERROR( "RegQueryValueExW 'hPythonPathKeyFound' get Error [%ld]"
                , nPython3PathPythonPathError
            );

            return false;
        }

        StdString::wcscpy_safe( m_python3LibraryPathW, Python3PathPythonPathValue, MENGINE_MAX_PATH );

        ::PathCombineW( m_python3ExecutablePathW, Python3PathW, L"python.exe" );

        if( ::PathFileExistsW( m_python3ExecutablePathW ) == FALSE )
        {
            LOGGER_ERROR( "invalid python3 lib path not found: %ls"
                , m_python3ExecutablePathW
            );

            return false;
        }

        WPath Python3DllNameW = {L'\0'};
        MENGINE_SWPRINTF( Python3DllNameW, MENGINE_MAX_PATH, L"python%d%d.dll", majorVersion, minorVersion );

        ::PathCombineW( m_python3DllPathW, Python3PathW, Python3DllNameW );

        if( ::PathFileExistsW( m_python3DllPathW ) == FALSE )
        {
            LOGGER_ERROR( "invalid python3 lib path not found: %ls"
                , m_python3DllPathW
            );

            return false;
        }

        LOGGER_MESSAGE( "Found python3 executable: %ls"
            , m_python3ExecutablePathW
        );

        LOGGER_MESSAGE( "Found python3 libraries: %ls"
            , m_python3LibraryPathW
        );

        LOGGER_MESSAGE( "Found python3 dll: %ls"
            , m_python3DllPathW
        );

        Helper::unicodeToUtf8( m_python3ExecutablePathW, m_python3ExecutablePathA, MENGINE_MAX_PATH );
        Helper::unicodeToUtf8( m_python3LibraryPathW, m_python3LibraryPathA, MENGINE_MAX_PATH );
        Helper::unicodeToUtf8( m_python3DllPathW, m_python3DllPathA, MENGINE_MAX_PATH );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32FindPython3Service::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32FindPython3Service::getPython3ExecutablePathW( WChar * const _path ) const
    {
        StdString::wcscpy_safe( _path, m_python3ExecutablePathW, MENGINE_MAX_PATH );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32FindPython3Service::getPython3LibraryPathW( WChar * const _path ) const
    {
        StdString::wcscpy_safe( _path, m_python3LibraryPathW, MENGINE_MAX_PATH );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32FindPython3Service::getPython3DllPathW( WChar * const _path ) const
    {
        StdString::wcscpy_safe( _path, m_python3DllPathW, MENGINE_MAX_PATH );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32FindPython3Service::getPython3ExecutablePathA( Char * const _path ) const
    {
        StdString::strcpy_safe( _path, m_python3ExecutablePathA, MENGINE_MAX_PATH );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32FindPython3Service::getPython3LibraryPathA( Char * const _path ) const
    {
        StdString::strcpy_safe( _path, m_python3LibraryPathA, MENGINE_MAX_PATH );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32FindPython3Service::getPython3DllPathA( Char * const _path ) const
    {
        StdString::strcpy_safe( _path, m_python3DllPathA, MENGINE_MAX_PATH );
    }
    //////////////////////////////////////////////////////////////////////////
}