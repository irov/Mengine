#include "Win32PilotPlugin.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( Win32PilotService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Win32Pilot, Mengine::Win32PilotPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32PilotPlugin::Win32PilotPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32PilotPlugin::~Win32PilotPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PilotPlugin::_unimportantPlugin() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PilotPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "pilot" ) == true )
        {
            return true;
        }

        if( HAS_OPTION( "nopilot" ) == true )
        {
            return false;
        }

        bool Win32PilotPlugin_Available = CONFIG_VALUE_BOOLEAN( "Win32PilotPlugin", "Available", false );

        if( Win32PilotPlugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PilotPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( Win32PilotService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32PilotPlugin::_finalizePlugin()
    {
        SERVICE_FINALIZE( Win32PilotService );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32PilotPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( Win32PilotService );
    }
    //////////////////////////////////////////////////////////////////////////
}
