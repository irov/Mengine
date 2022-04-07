#include "AppleMARSDKPlugin.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AppleMARSDKService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AppleMARSDK, Mengine::AppleMARSDKPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleMARSDKPlugin::AppleMARSDKPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleMARSDKPlugin::~AppleMARSDKPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleMARSDKPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "nomarsdk" ) == true )
        {
            return false;
        }

        if( HAS_OPTION( "marsdk" ) == false )
        {
            if( CONFIG_VALUE( "MARSDK", "Enable", true ) == false )
            {
                return false;
            }
        }
      
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleMARSDKPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( AppleMARSDKService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleMARSDKPlugin::_finalizePlugin()
    {
        SERVICE_FINALIZE( AppleMARSDKService );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleMARSDKPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( AppleMARSDKService );
    }
    //////////////////////////////////////////////////////////////////////////
}
