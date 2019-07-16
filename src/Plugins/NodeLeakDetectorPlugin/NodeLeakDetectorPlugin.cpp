#include "NodeLeakDetectorPlugin.h"

#include "Interface/OptionsServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"

#include "NodeLeakDetectorServiceInterface.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( NodeLeakDetectorService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( NodeLeakDetector, Mengine::NodeLeakDetectorPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    NodeLeakDetectorPlugin::NodeLeakDetectorPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    NodeLeakDetectorPlugin::~NodeLeakDetectorPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool NodeLeakDetectorPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "memleak" ) == true )
        {
            return true;
        }

        if( CONFIG_VALUE( "NodeLeakDetector", "Enable", false ) == true )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool NodeLeakDetectorPlugin::_systemPlugin() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool NodeLeakDetectorPlugin::_initializePlugin()
    {
        SERVICE_CREATE( NodeLeakDetectorService );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeLeakDetectorPlugin::_finalizePlugin()
    {
        SERVICE_FINALIZE( NodeLeakDetectorService );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeLeakDetectorPlugin::_destroy()
    {
        SERVICE_DESTROY( NodeLeakDetectorService );
    }
}