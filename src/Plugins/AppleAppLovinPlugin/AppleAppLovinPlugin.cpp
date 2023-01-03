#include "AppleAppLovinPlugin.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#   include "Interface/ScriptServiceInterface.h"

#   include "AppleAppLovinScriptEmbedding.h"
#endif

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/NotificationHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AppleAppLovinService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AppleAppLovin, Mengine::AppleAppLovinPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleAppLovinPlugin::AppleAppLovinPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleAppLovinPlugin::~AppleAppLovinPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "appleapplovin" ) == true )
        {
            return true;
        }

        if( HAS_OPTION( "noappleapplovin" ) == true )
        {
            return false;
        }
        
        bool AppleAppLovinPlugin_Available = CONFIG_VALUE( "AppLovinPlugin", "Available", true );

        if( AppleAppLovinPlugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( AppleAppLovinService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleAppLovinScriptEmbedding" ), Helper::makeFactorableUnique<AppleAppLovinScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleAppLovinScriptEmbedding" ) );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAppLovinPlugin::_finalizePlugin()
    {
#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

        SERVICE_FINALIZE( AppleAppLovinService );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAppLovinPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( AppleAppLovinService );
    }
    //////////////////////////////////////////////////////////////////////////
}
