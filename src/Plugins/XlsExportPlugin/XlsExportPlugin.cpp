#include "XlsExportPlugin.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/PlatformServiceInterface.h"

#define MENGINE_WINDOWS_MIN_VERSION_WIN8
#include "Environment/Windows/WindowsIncluder.h"
#include "Environment/Windows/Win32Register.h"
#include "Environment/Windows/Win32CreateProcess.h"
#include "Environment/Windows/Win32Helper.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/NotificationHelper.h"

#include "Config/StdString.h"
#include "Config/StdIO.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( XlsExport, Mengine::XlsExportPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    XlsExportPlugin::XlsExportPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    XlsExportPlugin::~XlsExportPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool XlsExportPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "noxlsexport" ) == true )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool XlsExportPlugin::_initializePlugin()
    {
        if( this->findPython3Path_() == false )
        {
            return false;
        }

        LOGGER_MESSAGE( "Found python3: %s"
            , m_python3Path
        );

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION, &XlsExportPlugin::notifyBootstrapperCreateApplication_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_RELOAD_LOCALE, &XlsExportPlugin::notifyReloadLocale_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void XlsExportPlugin::_finalizePlugin()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_RELOAD_LOCALE );
    }
    //////////////////////////////////////////////////////////////////////////
    void XlsExportPlugin::notifyBootstrapperCreateApplication_()
    {
        if( this->proccess_() == false )
        {
            LOGGER_FATAL(
                "force send quit for application\n"
                "please registry application or remove xlsExport plugin"
            );

            APPLICATION_SERVICE()
                ->quit();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void XlsExportPlugin::notifyReloadLocale_()
    {
        this->proccess_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool XlsExportPlugin::proccess_()
    {
        const ConstString & Project_Codename = APPLICATION_SERVICE()
            ->getProjectCodename();

        if( Project_Codename.empty() == true )
        {
            return true;
        }

        WChar currentPath[MENGINE_MAX_PATH] = {L'\0'};
        DWORD currentPathLen = ::GetCurrentDirectory( MENGINE_MAX_PATH, currentPath );

        if( currentPathLen == 0 )
        {
            LOGGER_ERROR( "invalid get current directory" );

            return false;
        }

        WChar xlsxExporterModulePath[MENGINE_MAX_PATH] = {L'\0'};
        ::PathCombine( xlsxExporterModulePath, currentPath, L"XlsxExport" );
        ::PathCombine( xlsxExporterModulePath, xlsxExporterModulePath, L"xlsxExporter.py" );

        WChar shortpath_xlsxExporterModulePath[MENGINE_MAX_PATH] = {L'\0'};
        DWORD ShortPathNameLen = ::GetShortPathName( xlsxExporterModulePath, shortpath_xlsxExporterModulePath, MENGINE_MAX_PATH );

        if( ShortPathNameLen == 0 )
        {
            LOGGER_ERROR( "invalid GetShortPathName '%ls' get error %ls"
                , xlsxExporterModulePath
                , Helper::Win32GetLastErrorMessage()
            );

            return false;
        }
        
        WChar command[MENGINE_MAX_COMMAND_LENGTH] = {'\0'};
        MENGINE_WNSPRINTF( command, MENGINE_MAX_COMMAND_LENGTH, L"-m %ls %S"
            , shortpath_xlsxExporterModulePath
            , Project_Codename.c_str()
        );

        uint32_t exitCode;
        if( Helper::Win32CreateProcess( m_python3Path, command, true, &exitCode ) == false )
        {
            LOGGER_ERROR( "invalid xlsx exporter: %ls"
                , command
            );

            return true;
        }

        if( exitCode != 0 )
        {
            LOGGER_ERROR( "failed xlsx exporter: %ls code: %u"
                , command
                , exitCode
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool XlsExportPlugin::findPython3Path_()
    {
        HKEY hKey;
        if( ::RegOpenKeyEx( HKEY_LOCAL_MACHINE, L"SOFTWARE\\Python\\PythonCore", 0, KEY_READ | KEY_WOW64_64KEY, &hKey ) != ERROR_SUCCESS )
        {
            LOGGER_ERROR( "invalid open registry key 'SOFTWARE\\Python\\PythonCore' error: %ls"
                , Helper::Win32GetLastErrorMessage()
            );

            return false;
        }

        WChar subKeyName[MENGINE_MAX_PATH] = {L'\0'};
        WChar latestVersion[MENGINE_MAX_PATH] = {L'\0'};

        for( DWORD index = 0;; ++index )
        {
            DWORD subKeyNameSize = sizeof( subKeyName ) / sizeof( TCHAR );
            FILETIME lastWriteTime;
            if( ::RegEnumKeyEx( hKey, index, subKeyName, &subKeyNameSize, NULL, NULL, NULL, &lastWriteTime ) != ERROR_SUCCESS )
            {
                break;
            }

            WChar KeySysVersionPath[MENGINE_MAX_PATH] = {L'\0'};
            MENGINE_WNSPRINTF( KeySysVersionPath, MENGINE_MAX_PATH, L"SOFTWARE\\Python\\PythonCore\\%s", subKeyName );

            HKEY hKeySysVersion;
            if( ::RegOpenKeyEx( HKEY_LOCAL_MACHINE, KeySysVersionPath, 0, KEY_READ | KEY_WOW64_64KEY, &hKeySysVersion ) != ERROR_SUCCESS )
            {
                continue;
            }

            DWORD dataType;
            DWORD dataSize;
            if( RegQueryValueExW( hKeySysVersion, L"SysVersion", NULL, &dataType, NULL, &dataSize ) != ERROR_SUCCESS )
            {
                continue;
            }

            if( MENGINE_WCSNCMP( subKeyName, latestVersion, subKeyNameSize ) > 0 )
            {
                MENGINE_WCSNCPY( latestVersion, subKeyName, subKeyNameSize );
            }
        }

        if( MENGINE_WCSLEN( latestVersion ) == 0 )
        {
            LOGGER_ERROR( "not found python version" );

            return false;
        }

        WChar subKeyPath[MENGINE_MAX_PATH] = {L'\0'};
        MENGINE_WNSPRINTF( subKeyPath, MENGINE_MAX_PATH, L"SOFTWARE\\Python\\PythonCore\\%s\\InstallPath", latestVersion );

        Char latestVersionPath[MENGINE_MAX_PATH] = {'\0'};
        if( Helper::Win32GetLocalMachineRegValue( subKeyPath, NULL, latestVersionPath, MENGINE_MAX_PATH ) == false )
        {
            LOGGER_ERROR( "not found python version path" );

            return false;
        }

        ::PathCombineA( m_python3Path, latestVersionPath, "python.exe" );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}