#include "AppleFirebaseCrashlyticsPlugin.h"


#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/NotificationHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AppleFirebaseCrashlyticsService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AppleFirebaseCrashlytics, Mengine::AppleFirebaseCrashlyticsPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleFirebaseCrashlyticsPlugin::AppleFirebaseCrashlyticsPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleFirebaseCrashlyticsPlugin::~AppleFirebaseCrashlyticsPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFirebaseCrashlyticsPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "noAppleFirebaseCrashlytics" ) == true )
        {
            return false;
        }

        if( HAS_OPTION( "AppleFirebaseCrashlytics" ) == false )
        {
            if( CONFIG_VALUE( "AppleFirebaseCrashlytics", "Enable", false ) == false )
            {
                return false;
            }
        }
      
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFirebaseCrashlyticsPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( AppleFirebaseCrashlyticsService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

//#ifdef MENGINE_USE_SCRIPT_SERVICE
//        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
//        {
//            SCRIPT_SERVICE()
//                ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleFirebaseCrashlyticsScriptEmbedding" ), Helper::makeFactorableUnique<AppleFirebaseCrashlyticsScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
//        }, MENGINE_DOCUMENT_FACTORABLE );
//
//        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
//        {
//            SCRIPT_SERVICE()
//                ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleFirebaseCrashlyticsScriptEmbedding" ) );
//        }, MENGINE_DOCUMENT_FACTORABLE );
//#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFirebaseCrashlyticsPlugin::_finalizePlugin()
    {

        SERVICE_FINALIZE( AppleFirebaseCrashlyticsService );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFirebaseCrashlyticsPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( AppleFirebaseCrashlyticsService );
    }
    //////////////////////////////////////////////////////////////////////////
}
