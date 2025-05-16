#include "Win32XlsExportPlugin.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/PlatformServiceInterface.h"

#define MENGINE_WINDOWS_MIN_VERSION_WIN8
#include "Environment/Windows/WindowsIncluder.h"
#include "Environment/Windows/Win32Register.h"
#include "Environment/Windows/Win32CreateProcess.h"
#include "Environment/Windows/Win32Helper.h"

#include "Plugins/Win32FindPython3Plugin/Win32FindPython3Interface.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/PluginHelper.h"

#include "Config/StdString.h"
#include "Config/StdIO.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Win32XlsExport, Mengine::Win32XlsExportPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32XlsExportPlugin::Win32XlsExportPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32XlsExportPlugin::~Win32XlsExportPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32XlsExportPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "noxlsexport" ) == true )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32XlsExportPlugin::_initializePlugin()
    {
        WIN32_FINDPYTHON3_SERVICE()
            ->getPython3ExecutablePathW( m_python3PathW );

        if( StdString::wcslen( m_python3PathW ) == 0 )
        {
            LOGGER_ERROR( "not found python3" );

            return false;
        }

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION, &Win32XlsExportPlugin::notifyBootstrapperCreateApplication_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_RELOAD_LOCALE, &Win32XlsExportPlugin::notifyReloadLocale_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32XlsExportPlugin::_finalizePlugin()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_RELOAD_LOCALE );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32XlsExportPlugin::notifyBootstrapperCreateApplication_()
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
    void Win32XlsExportPlugin::notifyReloadLocale_()
    {
        this->proccess_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32XlsExportPlugin::proccess_()
    {
        LOGGER_MESSAGE( "xls export process..." );

        const ConstString & Project_Codename = APPLICATION_SERVICE()
            ->getProjectCodename();

        if( Project_Codename.empty() == true )
        {
            LOGGER_MESSAGE( "not found project codename" );

            return true;
        }

        WPath currentDirectory = {L'\0'};
        DWORD currentDirectoryLen = ::GetCurrentDirectory( MENGINE_MAX_PATH, currentDirectory );

        if( currentDirectoryLen == 0 )
        {
            LOGGER_ERROR( "invalid get current directory" );

            return false;
        }

        WPath xlsxExporterModulePath = {L'\0'};
        ::PathCombine( xlsxExporterModulePath, currentDirectory, L"XlsxExport" );
        ::PathCombine( xlsxExporterModulePath, xlsxExporterModulePath, L"xlsxExporter.py" );

        WPath shortpath_xlsxExporterModulePath = {L'\0'};
        DWORD ShortPathNameLen = ::GetShortPathName( xlsxExporterModulePath, shortpath_xlsxExporterModulePath, MENGINE_MAX_PATH );

        if( ShortPathNameLen == 0 )
        {
            LOGGER_ERROR( "invalid GetShortPathName '%ls' get error: %ls"
                , xlsxExporterModulePath
                , Helper::Win32GetLastErrorMessageW()
            );

            return false;
        }
        
        WChar command[MENGINE_MAX_COMMAND_LENGTH + 1] = {'\0'};
        MENGINE_SWPRINTF( command, MENGINE_MAX_COMMAND_LENGTH, L"\"%ls\" %S"
            , shortpath_xlsxExporterModulePath
            , Project_Codename.c_str()
        );

        uint32_t exitCode;
        if( Helper::Win32CreateProcessW( m_python3PathW, command, true, &exitCode ) == false )
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
}