#include "AppleAppTrackingPlugin.h"

#if defined(MENGINE_USE_SCRIPT_SERVICE)
#   include "Interface/ScriptServiceInterface.h"

#   include "AppleAppTrackingScriptEmbedding.h"
#endif

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AppleAppTrackingService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AppleAppTracking, Mengine::AppleAppTrackingPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleAppTrackingPlugin::AppleAppTrackingPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleAppTrackingPlugin::~AppleAppTrackingPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleAppTrackingPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "appleapptracking" ) == true )
        {
            return true;
        }
            
        if( HAS_OPTION( "noappleapptracking" ) == true )
        {
            return false;
        }
        
        bool AppleAppTrackingPlugin_Available = CONFIG_VALUE_BOOLEAN( "AppleAppTrackingPlugin", "Available", true );
        
        if( AppleAppTrackingPlugin_Available == false )
        {
            return false;
        }
  
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleAppTrackingPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( AppleAppTrackingService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

#if defined(MENGINE_USE_SCRIPT_SERVICE)
        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleAppTrackingScriptEmbedding" ), Helper::makeFactorableUnique<AppleAppTrackingScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleAppTrackingScriptEmbedding" ) );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAppTrackingPlugin::_finalizePlugin()
    {
#if defined(MENGINE_USE_SCRIPT_SERVICE)
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

        SERVICE_FINALIZE( AppleAppTrackingService );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAppTrackingPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( AppleAppTrackingService );
    }
    //////////////////////////////////////////////////////////////////////////
}
