#include "AppleFirebaseRemoteConfigPlugin.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/NotificationHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AppleFirebaseRemoteConfigService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AppleFirebaseRemoteConfig, Mengine::AppleFirebaseRemoteConfigPlugin )
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
        
        bool AppleFirebaseRemoteConfigPlugin_Available = CONFIG_VALUE( "AppleFirebaseRemoteConfigPlugin", "Available", true );

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

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFirebaseRemoteConfigPlugin::_finalizePlugin()
    {
        SERVICE_FINALIZE( AppleFirebaseRemoteConfigService );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFirebaseRemoteConfigPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( AppleFirebaseRemoteConfigService );
    }
    //////////////////////////////////////////////////////////////////////////
}
