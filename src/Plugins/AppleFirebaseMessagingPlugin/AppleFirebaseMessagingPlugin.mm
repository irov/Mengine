#include "AppleFirebaseMessagingPlugin.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/NotificationHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AppleFirebaseMessagingService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AppleFirebaseMessaging, Mengine::AppleFirebaseMessagingPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleFirebaseMessagingPlugin::AppleFirebaseMessagingPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleFirebaseMessagingPlugin::~AppleFirebaseMessagingPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFirebaseMessagingPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "applefirebasemessaging" ) == true )
        {
            return true;
        }

        if( HAS_OPTION( "noapplefirebasemessaging" ) == true )
        {
            return false;
        }
        
        bool AppleFirebaseMessagingPlugin_Available = CONFIG_VALUE( "AppleFirebaseMessagingPlugin", "Available", true );

        if( AppleFirebaseMessagingPlugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFirebaseMessagingPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( AppleFirebaseMessagingService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFirebaseMessagingPlugin::_finalizePlugin()
    {
        SERVICE_FINALIZE( AppleFirebaseMessagingService );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFirebaseMessagingPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( AppleFirebaseMessagingService );
    }
    //////////////////////////////////////////////////////////////////////////
}
