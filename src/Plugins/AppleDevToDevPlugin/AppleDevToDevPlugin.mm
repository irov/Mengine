#include "AppleDevToDevPlugin.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/NotificationHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AppleDevToDevService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AppleDevToDev, Mengine::AppleDevToDevPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleDevToDevPlugin::AppleDevToDevPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleDevToDevPlugin::~AppleDevToDevPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleDevToDevPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "appledevtodev" ) == true )
        {
            return true;
        }
            
        if( HAS_OPTION( "noappledevtodev" ) == true )
        {
            return false;
        }
        
        bool AppleDevToDevPlugin_Available = CONFIG_VALUE( "AppleDevToDevPlugin", "Available", true );
        
        if( AppleDevToDevPlugin_Available == false )
        {
            return false;
        }
  
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleDevToDevPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( AppleDevToDevService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleDevToDevPlugin::_finalizePlugin()
    {
        SERVICE_FINALIZE( AppleDevToDevService );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleDevToDevPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( AppleDevToDevService );
    }
    //////////////////////////////////////////////////////////////////////////
}
