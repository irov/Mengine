#include "AppleNativePythonPlugin.h"

#include "Interface/ServiceInterface.h"

#include "Kernel/PluginHelper.h"
#include "Kernel/DocumentHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AppleNativePythonService );
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AppleNativePython, Mengine::AppleNativePythonPlugin );
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleNativePythonPlugin::AppleNativePythonPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleNativePythonPlugin::~AppleNativePythonPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleNativePythonPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( AppleNativePythonService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleNativePythonPlugin::_finalizePlugin()
    {
        SERVICE_FINALIZE( AppleNativePythonService );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleNativePythonPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( AppleNativePythonService );
    }
    ///////////////////////////////////////////////////////////////////////
}
