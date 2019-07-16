#include "PythonFrameworkPlugin.h"

#include "Interface/FrameworkInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/ScriptProviderServiceInterface.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( ScriptProviderService );
SERVICE_EXTERN( ScriptService );
SERVICE_EXTERN( Framework );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( PythonFramework, Mengine::PythonFrameworkPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{    
    //////////////////////////////////////////////////////////////////////////
    PythonFrameworkPlugin::PythonFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonFrameworkPlugin::~PythonFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonFrameworkPlugin::_initializePlugin()
    {
        SERVICE_CREATE( ScriptProviderService );
        SERVICE_CREATE( ScriptService );
        SERVICE_CREATE( Framework );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonFrameworkPlugin::_finalizePlugin()
    {
        SERVICE_FINALIZE( Framework );
        SERVICE_FINALIZE( ScriptService );
        SERVICE_FINALIZE( ScriptProviderService );
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonFrameworkPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( Framework );
        SERVICE_DESTROY( ScriptService );
        SERVICE_DESTROY( ScriptProviderService );
    }
}
