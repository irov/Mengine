#include "AppLovinPlugin.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#   include "Interface/ScriptServiceInterface.h"

//#   include "AppLovinPluginScriptEmbedding.h"
#endif

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/NotificationHelper.h"

//////////////////////////////////////////////////////////////////////////
//SERVICE_EXTERN( AppLovinPluginService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AppLovinPlugin, Mengine::AppLovinPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppLovinPlugin::AppLovinPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppLovinPlugin::~AppLovinPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppLovinPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "noapplovin" ) == true )
        {
            return false;
        }

        if( HAS_OPTION( "appodeal" ) == false )
        {
            if( CONFIG_VALUE( "APPLOVIN", "Enable", true ) == false )
            {
                return false;
            }
        }
      
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppLovinPlugin::_initializePlugin()
    {
//        if( SERVICE_CREATE( AppLovinPluginService, MENGINE_DOCUMENT_FACTORABLE ) == false )
//        {
//            return false;
//        }

//#ifdef MENGINE_USE_SCRIPT_SERVICE
//        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
//        {
//            SCRIPT_SERVICE()
//                ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppLovinPluginScriptEmbedding" ), Helper::makeFactorableUnique<AppLovinPluginScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
//        }, MENGINE_DOCUMENT_FACTORABLE );
//
//        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
//        {
//            SCRIPT_SERVICE()
//                ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppLovinPluginScriptEmbedding" ) );
//        }, MENGINE_DOCUMENT_FACTORABLE );
//#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppLovinPlugin::_finalizePlugin()
    {
#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

//        SERVICE_FINALIZE( AppLovinPluginService );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppLovinPlugin::_destroyPlugin()
    {
//        SERVICE_DESTROY( AppLovinPluginService );
    }
    //////////////////////////////////////////////////////////////////////////
}
