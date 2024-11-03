#include "AreaOfInterestPlugin.h"

#include "Interface/ServiceInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/PluginHelper.h"
#include "Kernel/DocumentHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AreaOfInterestService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AreaOfInterest, Mengine::AreaOfInterestPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AreaOfInterestPlugin::AreaOfInterestPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AreaOfInterestPlugin::~AreaOfInterestPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AreaOfInterestPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( AreaOfInterestService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AreaOfInterestPlugin::_finalizePlugin()
    {
        SERVICE_FINALIZE( AreaOfInterestService );
    }
    //////////////////////////////////////////////////////////////////////////
    void AreaOfInterestPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( AreaOfInterestService );
    }
    //////////////////////////////////////////////////////////////////////////
}
