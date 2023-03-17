#include "AppleOneSignalPlugin.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/NotificationHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AppleOneSignalService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AppleOneSignal, Mengine::AppleOneSignalPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleOneSignalPlugin::AppleOneSignalPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleOneSignalPlugin::~AppleOneSignalPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleOneSignalPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "appleonesignal" ) == true )
        {
            return true;
        }

        if( HAS_OPTION( "noappleonesignal" ) == true )
        {
            return false;
        }
        
        bool AppleOneSignalPlugin_Available = CONFIG_VALUE( "AppleOneSignalPlugin", "Available", true );

        if( AppleOneSignalPlugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleOneSignalPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( AppleOneSignalService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleOneSignalPlugin::_finalizePlugin()
    {
        SERVICE_FINALIZE( AppleOneSignalService );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleOneSignalPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( AppleOneSignalService );
    }
    //////////////////////////////////////////////////////////////////////////
}
