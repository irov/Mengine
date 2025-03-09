#include "AppleFirebaseRemoteConfigPlugin.h"

#if defined(MENGINE_USE_SCRIPT_SERVICE)
#   include "Interface/ScriptServiceInterface.h"

#   include "AppleFirebaseRemoteConfigScriptEmbedding.h"
#endif

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AppleFirebaseRemoteConfigService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AppleFirebaseRemoteConfig, Mengine::AppleFirebaseRemoteConfigPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleFirebaseRemoteConfigPlugin::AppleFirebaseRemoteConfigPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleFirebaseRemoteConfigPlugin::~AppleFirebaseRemoteConfigPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFirebaseRemoteConfigPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "applefirebaseremoteconfig" ) == true )
        {
            return true;
        }

        if( HAS_OPTION( "noapplefirebaseremoteconfig" ) == true )
        {
            return false;
        }
        
        bool AppleFirebaseRemoteConfigPlugin_Available = CONFIG_VALUE_BOOLEAN( "AppleFirebaseRemoteConfigPlugin", "Available", true );

        if( AppleFirebaseRemoteConfigPlugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFirebaseRemoteConfigPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( AppleFirebaseRemoteConfigService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }
        
#if defined(MENGINE_USE_SCRIPT_SERVICE)
        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleFirebaseRemoteConfigScriptEmbedding" ), Helper::makeFactorableUnique<AppleFirebaseRemoteConfigScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleFirebaseRemoteConfigScriptEmbedding" ) );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFirebaseRemoteConfigPlugin::_finalizePlugin()
    {
#if defined(MENGINE_USE_SCRIPT_SERVICE)
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif
        
        SERVICE_FINALIZE( AppleFirebaseRemoteConfigService );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFirebaseRemoteConfigPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( AppleFirebaseRemoteConfigService );
    }
    //////////////////////////////////////////////////////////////////////////
}
