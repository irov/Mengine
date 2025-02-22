#include "AppleUserNotificationCenterPlugin.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AppleUserNotificationCenterService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AppleUserNotificationCenter, Mengine::AppleUserNotificationCenterPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleUserNotificationCenterPlugin::AppleUserNotificationCenterPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleUserNotificationCenterPlugin::~AppleUserNotificationCenterPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleUserNotificationCenterPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "appleusernotificationcenter" ) == true )
        {
            return true;
        }

        if( HAS_OPTION( "noappleusernotificationcenter" ) == true )
        {
            return false;
        }
        
        bool AppleUserNotificationCenterPlugin_Available = CONFIG_VALUE_BOOLEAN( "AppleUserNotificationCenterPlugin", "Available", true );

        if( AppleUserNotificationCenterPlugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleUserNotificationCenterPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( AppleUserNotificationCenterService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleUserNotificationCenterPlugin::_finalizePlugin()
    {
        SERVICE_FINALIZE( AppleUserNotificationCenterService );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleUserNotificationCenterPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( AppleUserNotificationCenterService );
    }
    //////////////////////////////////////////////////////////////////////////
}
