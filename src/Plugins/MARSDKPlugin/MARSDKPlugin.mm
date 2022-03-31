#include "MARSDKPlugin.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( MARSDK, Mengine::MARSDKPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MARSDKPlugin::MARSDKPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MARSDKPlugin::~MARSDKPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MARSDKPlugin::_unimportantPlugin() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MARSDKPlugin::_availablePlugin() const
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
    bool MARSDKPlugin::_initializePlugin()
    {
        m_marSDKNative = [[IOSMarSDKNative alloc] init];
        
        [m_marSDKNative didFinishLaunchingWithOptions:[UIApplication sharedApplication] didFinishLaunchingWithOptions:NULL];

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MARSDKPlugin::_finalizePlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MARSDKPlugin::_destroyPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
}
