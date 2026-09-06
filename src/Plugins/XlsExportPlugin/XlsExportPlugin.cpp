#include "XlsExportPlugin.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/FindPython3ServiceInterface.h"
#include "Interface/PlatformServiceInterface.h"

#include "Kernel/ArgumentStrings.h"
#include "Kernel/BuildMode.h"
#include "Kernel/Logger.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/PluginHelper.h"

#include "Config/StdLib.h"

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
    const ServiceRequiredList & XlsExportPlugin::requiredServices() const
    {
        static ServiceRequiredList required = {
            SERVICE_ID( FindPython3ServiceInterface ),
            SERVICE_ID( PlatformServiceInterface )
        };

        return required;
    }
    //////////////////////////////////////////////////////////////////////////
    bool XlsExportPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "nodataexport" ) == true )
        {
            return false;
        }

        if( HAS_OPTION( "noxlsexport" ) == true )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool XlsExportPlugin::_initializePlugin()
    {
        FINDPYTHON3_SERVICE()
            ->getPython3ExecutablePath( m_python3ExecutablePath );

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION, &XlsExportPlugin::notifyBootstrapperCreateApplication_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_RELOAD_LOCALE, &XlsExportPlugin::notifyReloadLocale_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void XlsExportPlugin::_finalizePlugin()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_RELOAD_LOCALE );

        m_python3ExecutablePath[0] = '\0';
    }
    //////////////////////////////////////////////////////////////////////////
    void XlsExportPlugin::notifyBootstrapperCreateApplication_()
    {
        if( this->process_() == false )
        {
            LOGGER_FATAL(
                "xls export failed\n"
                "fix the exporter error or run with --nodataexport"
            );

            APPLICATION_SERVICE()
                ->quit();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void XlsExportPlugin::notifyReloadLocale_()
    {
        if( this->process_() == false )
        {
            LOGGER_ERROR( "xls export failed during locale reload" );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool XlsExportPlugin::process_()
    {
        if( m_python3ExecutablePath[0] == '\0' )
        {
            LOGGER_ERROR( "xls export python3 executable was not found" );

            return false;
        }

        const ConstString & projectCodename = APPLICATION_SERVICE()
            ->getProjectCodename();

        if( projectCodename.empty() == true )
        {
            LOGGER_MESSAGE( "xls export skipped: project codename is empty" );

            return true;
        }

        const Char * projectName = projectCodename.c_str();
        const Char * exporterPath = "XlsxExport/xlsxExporter.py";

        ArgumentStrings arguments;
        arguments.append( exporterPath );
        arguments.append( projectName );

        bool strictExport = Helper::isDevelopmentMode() == false;

        if( strictExport == true )
        {
            arguments.append( "--strict" );
        }

        const Char * strictSuffix = strictExport == true ? " strict" : "";

        LOGGER_MESSAGE( "xls export started: '%s' project '%s'%s"
            , exporterPath
            , projectName
            , strictSuffix
        );

        uint32_t exitCode = EXIT_FAILURE;

        if( PLATFORM_SERVICE()->createProcess( m_python3ExecutablePath, arguments, &exitCode ) == false )
        {
            LOGGER_ERROR( "unable to start xls exporter '%s'"
                , exporterPath
            );

            return false;
        }

        if( exitCode != EXIT_SUCCESS )
        {
            LOGGER_ERROR( "xls exporter '%s' failed with code %u"
                , exporterPath
                , exitCode
            );

            return false;
        }

        LOGGER_MESSAGE( "xls export complete" );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
