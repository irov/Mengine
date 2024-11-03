#include "DevToDebugPlugin.h"

#include "Interface/HttpServiceInterface.h"
#include "Interface/ThreadSystemInterface.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/BuildMode.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( DevToDebugService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( DevToDebug, Mengine::DevToDebugPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevToDebugPlugin::DevToDebugPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugPlugin::~DevToDebugPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugPlugin::_unimportantPlugin() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const ServiceRequiredList & DevToDebugPlugin::requiredServices() const
    {
        static ServiceRequiredList required = {
            SERVICE_ID( HttpServiceInterface ),
            SERVICE_ID( ThreadSystemInterface )
        };

        return required;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "devtodebug" ) == true )
        {
            return true;
        }

        if( HAS_OPTION( "nodevtodebug" ) == true )
        {
            return false;
        }

        if( Helper::isBuildPublish() == true )
        {
            return false;
        }
        
        if( Helper::isDebugMode() == false )
        {
            return false;
        }

        if( HAS_CONFIG( "DevToDebugPlugin", "DSN" ) == false )
        {
            LOGGER_MESSAGE( "DevToDebugPlugin unavailable [not setup DSN]" );

            return false;
        }

        bool DevToDebugPlugin_Available = CONFIG_VALUE( "DevToDebugPlugin", "Available", false );

        if( DevToDebugPlugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( DevToDebugService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugPlugin::_finalizePlugin()
    {
        SERVICE_FINALIZE( DevToDebugService );
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( DevToDebugService );
    }
    //////////////////////////////////////////////////////////////////////////
}
