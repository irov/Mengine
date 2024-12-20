#include "PathfinderPlugin.h"

#include "Interface/ServiceInterface.h"

#include "Kernel/PluginHelper.h"
#include "Kernel/DocumentHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( PathfinderService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Pathfinder, Mengine::PathfinderPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PathfinderPlugin::PathfinderPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PathfinderPlugin::~PathfinderPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PathfinderPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( PathfinderService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PathfinderPlugin::_finalizePlugin()
    {
        SERVICE_FINALIZE( PathfinderService );
    }
    //////////////////////////////////////////////////////////////////////////
    void PathfinderPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( PathfinderService );
    }
    //////////////////////////////////////////////////////////////////////////
}
