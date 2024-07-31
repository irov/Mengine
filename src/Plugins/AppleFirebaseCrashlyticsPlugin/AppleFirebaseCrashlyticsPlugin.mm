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
        if( HAS_OPTION( "applefirebasecrashlytics" ) == true )
        {
            return true;
        }

        if( HAS_OPTION( "noapplefirebasecrashlytics" ) == true )
        {
            return false;
        }
        
        bool AppleFirebaseCrashlyticsPlugin_Available = CONFIG_VALUE( "AppleFirebaseCrashlyticsPlugin", "Available", true );

        if( AppleFirebaseCrashlyticsPlugin_Available == false )
        {
            return false;
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
