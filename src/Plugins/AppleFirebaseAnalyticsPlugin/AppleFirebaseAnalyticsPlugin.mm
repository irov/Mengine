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
