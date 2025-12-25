#include "AppleStoreInAppPurchasePlugin.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "Interface/ScriptServiceInterface.h"

#   include "AppleStoreInAppPurchaseScriptEmbedding.h"
#endif

#include "AppleStoreInAppPurchaseInterface.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AppleStoreInAppPurchaseService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AppleStoreInAppPurchase, Mengine::AppleStoreInAppPurchasePlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleStoreInAppPurchasePlugin::AppleStoreInAppPurchasePlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleStoreInAppPurchasePlugin::~AppleStoreInAppPurchasePlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleStoreInAppPurchasePlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "applestoreinapppurchase" ) == true )
        {
            return true;
        }
            
        if( HAS_OPTION( "noapplestoreinapppurchase" ) == true )
        {
            return false;
        }
        
        bool AppleStoreInAppPurchasePlugin_Available = CONFIG_VALUE_BOOLEAN( "AppleStoreInAppPurchasePlugin", "Available", true );
        
        if( AppleStoreInAppPurchasePlugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleStoreInAppPurchasePlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( AppleStoreInAppPurchaseService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }
        
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleStoreInAppPurchaseScriptEmbedding" ), Helper::makeFactorableUnique<AppleStoreInAppPurchaseScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleStoreInAppPurchaseScriptEmbedding" ) );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleStoreInAppPurchasePlugin::_finalizePlugin()
    {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif
        
        SERVICE_FINALIZE( AppleStoreInAppPurchaseService );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleStoreInAppPurchasePlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( AppleStoreInAppPurchaseService );
    }
    //////////////////////////////////////////////////////////////////////////
}
