#include "AppleSKAdNetworkPlugin.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/NotificationHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AppleSKAdNetworkService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AppleSKAdNetwork, Mengine::AppleSKAdNetworkPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleSKAdNetworkPlugin::AppleSKAdNetworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleSKAdNetworkPlugin::~AppleSKAdNetworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSKAdNetworkPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "appleskadnetwork" ) == true )
        {
            return true;
        }

        if( HAS_OPTION( "noappleskadnetwork" ) == true )
        {
            return false;
        }
        
        bool AppleSKAdNetworkPlugin_Available = CONFIG_VALUE( "AppleSKAdNetworkPlugin", "Available", true );

        if( AppleSKAdNetworkPlugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSKAdNetworkPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( AppleSKAdNetworkService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSKAdNetworkPlugin::_finalizePlugin()
    {
        SERVICE_FINALIZE( AppleSKAdNetworkService );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSKAdNetworkPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( AppleSKAdNetworkService );
    }
    //////////////////////////////////////////////////////////////////////////
}
