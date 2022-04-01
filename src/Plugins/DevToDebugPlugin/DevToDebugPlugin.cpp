#include "DevToDebugPlugin.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( DevToDebugService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( DevToDebug, Mengine::DevToDebugPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevToDebugPlugin::DevToDebugPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugPlugin::~DevToDebugPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugPlugin::_unimportantPlugin() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "nodevtodebug" ) == true )
        {
            return false;
        }

        if( HAS_OPTION( "devtodebug" ) == false )
        {
            if( CONFIG_VALUE( "DevToDebug", "Enable", true ) == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( DevToDebugService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugPlugin::_finalizePlugin()
    {
        SERVICE_FINALIZE( DevToDebugService );
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( DevToDebugService );
    }
    //////////////////////////////////////////////////////////////////////////
}
