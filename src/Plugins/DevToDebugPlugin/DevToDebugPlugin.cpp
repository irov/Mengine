#include "DevToDebugPlugin.h"

#include "Interface/HttpServiceInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/ScriptServiceInterface.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/BuildMode.h"
#include "Kernel/PluginHelper.h"
#include "Kernel/NotificationHelper.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "DevToDebugScriptEmbedding.h"
#endif

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

        bool DevToDebugPlugin_Available = CONFIG_VALUE_BOOLEAN( "DevToDebugPlugin", "Available", false );

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

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( DevToDebugScriptEmbedding::getFactorableType(), Helper::makeFactorableUnique<DevToDebugScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( DevToDebugScriptEmbedding::getFactorableType() );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugPlugin::_finalizePlugin()
    {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

        SERVICE_FINALIZE( DevToDebugService );
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( DevToDebugService );
    }
    //////////////////////////////////////////////////////////////////////////
}
