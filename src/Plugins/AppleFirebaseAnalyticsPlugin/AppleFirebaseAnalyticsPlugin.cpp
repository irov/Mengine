#include "AppleFirebaseAnalyticsPlugin.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/NotificationHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AppleFirebaseAnalyticsService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AppleFirebaseAnalytics, Mengine::AppleFirebaseAnalyticsPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleFirebaseAnalyticsPlugin::AppleFirebaseAnalyticsPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleFirebaseAnalyticsPlugin::~AppleFirebaseAnalyticsPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFirebaseAnalyticsPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "applefirebaseanalytics" ) == true )
        {
            return true;
        }

        if( HAS_OPTION( "noapplefirebaseanalytics" ) == true )
        {
            return false;
        }
        
        bool AppleFirebaseAnalyticsPlugin_Available = CONFIG_VALUE( "AppleFirebaseAnalyticsPlugin", "Available", true );

        if( AppleFirebaseAnalyticsPlugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFirebaseAnalyticsPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( AppleFirebaseAnalyticsService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        //#ifdef MENGINE_USE_SCRIPT_SERVICE
        //        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
        //        {
        //            SCRIPT_SERVICE()
        //                ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleFirebaseAnalyticsScriptEmbedding" ), Helper::makeFactorableUnique<AppleFirebaseAnalyticsScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        //        }, MENGINE_DOCUMENT_FACTORABLE );
        //
        //        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
        //        {
        //            SCRIPT_SERVICE()
        //                ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleFirebaseAnalyticsScriptEmbedding" ) );
        //        }, MENGINE_DOCUMENT_FACTORABLE );
        //#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFirebaseAnalyticsPlugin::_finalizePlugin()
    {
        SERVICE_FINALIZE( AppleFirebaseAnalyticsService );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFirebaseAnalyticsPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( AppleFirebaseAnalyticsService );
    }
    //////////////////////////////////////////////////////////////////////////
}
