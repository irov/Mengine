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
        WChar ENVPython3Path[MENGINE_MAX_PATH] = {L'\0'};
        if( ::GetEnvironmentVariable( L"MENGINE_PYTHON3_PATH", ENVPython3Path, MENGINE_MAX_PATH ) != 0 )
        {
            if( ::PathFileExistsW( ENVPython3Path ) == FALSE )
            {
                LOGGER_ERROR( "invalid python3 path not found: %ls"
                    , ENVPython3Path
                );

                return false;
            }

            if( Helper::unicodeToUtf8( ENVPython3Path, m_python3Path, MENGINE_MAX_PATH, nullptr ) == false )
            {
                LOGGER_ERROR( "invalid convert python3 path to utf8: %ls"
                    , ENVPython3Path
                );

                return false;
            }

            LOGGER_MESSAGE( "found python3 path [Env]: %s", m_python3Path );

            return true;
        }

        HKEY hBases[] = {HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE};

        BOOL bInstallPathFound = FALSE;
        HKEY hInstallPathKeyFound = NULL;
        int32_t minorVersion = 0;
        int32_t majorVersion = 0;

        for( HKEY hBase : hBases )
        {
            HKEY hKey;
            if( ::RegOpenKeyEx( hBase, L"SOFTWARE\\Python\\PythonCore", 0, KEY_READ | KEY_WOW64_64KEY, &hKey ) != ERROR_SUCCESS )
            {
                continue;
            }

            for( DWORD index = 0;; ++index )
            {
                WChar subKeyName[MENGINE_MAX_PATH] = {L'\0'};
                DWORD subKeyNameSize = sizeof( subKeyName ) / sizeof( TCHAR );
                FILETIME lastWriteTime;
                if( ::RegEnumKeyEx( hKey, index, subKeyName, &subKeyNameSize, NULL, NULL, NULL, &lastWriteTime ) != ERROR_SUCCESS )
                {
                    break;
                }

                WChar KeySysVersionPath[MENGINE_MAX_PATH] = {L'\0'};
                MENGINE_WNSPRINTF( KeySysVersionPath, MENGINE_MAX_PATH, L"SOFTWARE\\Python\\PythonCore\\%s", subKeyName );

                HKEY hKeySysVersion;
                if( ::RegOpenKeyEx( hBase, KeySysVersionPath, 0, KEY_READ | KEY_WOW64_64KEY, &hKeySysVersion ) != ERROR_SUCCESS )
                {
                    continue;
                }

                DWORD dataType;
                WChar data[MENGINE_MAX_PATH] = {L'\0'};
                DWORD dataSize = MENGINE_MAX_PATH;
                if( ::RegQueryValueExW( hKeySysVersion, L"SysVersion", NULL, &dataType, (LPBYTE)data, &dataSize ) != ERROR_SUCCESS )
                {
                    continue;
                }

                int32_t major;
                int32_t minor;
                MENGINE_SWSCANF( data, L"%d.%d"
                    , &major
                    , &minor
                );

                if( Detail::isVersionGreater( major, minor, majorVersion, minorVersion ) == true )
                {
                    WChar subKeyPython3InstallPath[MENGINE_MAX_PATH] = {L'\0'};
                    MENGINE_WNSPRINTF( subKeyPython3InstallPath, MENGINE_MAX_PATH, L"SOFTWARE\\Python\\PythonCore\\%s\\InstallPath", subKeyName );

                    HKEY hInstallPathKey;
                    if( ::RegOpenKeyExW( hBase, subKeyPython3InstallPath, 0, KEY_READ | KEY_WOW64_64KEY, &hInstallPathKey ) != ERROR_SUCCESS )
                    {
                        LOGGER_ERROR( "invalid open registry key '%ls' error: %ls"
                            , subKeyPython3InstallPath
                            , Helper::Win32GetLastErrorMessageW()
                        );

                        return false;
                    }

                    majorVersion = major;
                    minorVersion = minor;
                    
                    hInstallPathKeyFound = hInstallPathKey;
                    bInstallPathFound = TRUE;
                }
            }
        }

        if( bInstallPathFound == FALSE )
        {
            LOGGER_ERROR( "not found python version" );

            return false;
        }

        WChar hPython3PathInstallPathValue[MENGINE_MAX_PATH] = {L'\0'};
        DWORD hPython3PathInstallPathLen = MENGINE_MAX_PATH;
        LSTATUS nError = ::RegQueryValueExW( hInstallPathKeyFound, NULL, 0, NULL, (LPBYTE)hPython3PathInstallPathValue, &hPython3PathInstallPathLen );

        ::RegCloseKey( hInstallPathKeyFound );

        if( nError != ERROR_SUCCESS )
        {
            LOGGER_ERROR( "RegQueryValueExW '%ls' get Error [%ld]"
                , hPython3PathInstallPathValue
                , nError
            );

            return false;
        }

        Char RegPython3Path[MENGINE_MAX_PATH] = {'\0'};
        Helper::unicodeToUtf8( hPython3PathInstallPathValue, RegPython3Path, MENGINE_MAX_PATH );

        ::PathCombineA( m_python3Path, RegPython3Path, "python.exe" );

        LOGGER_MESSAGE( "Found python3: %s"
            , m_python3Path
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32FindPython3Service::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32FindPython3Service::getPython3Path( Char * const _python3Path ) const
    {
        MENGINE_STRCPY( _python3Path, m_python3Path );
    }
    //////////////////////////////////////////////////////////////////////////
}