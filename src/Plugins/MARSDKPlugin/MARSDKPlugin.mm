#include "MARSDKPlugin.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( MARSDKService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( MARSDK, Mengine::MARSDKPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MARSDKPlugin::MARSDKPlugin()
    {
        //для примера вызова
        m_marSDKNative = [[IOSMarSDKNative alloc] init];
        [m_marSDKNative didFinishLaunchingWithOptions:[UIApplication sharedApplication] didFinishLaunchingWithOptions:NULL];
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
        if( SERVICE_CREATE( MARSDKService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MARSDKPlugin::_finalizePlugin()
    {
        SERVICE_FINALIZE( MARSDKService );
    }
    //////////////////////////////////////////////////////////////////////////
    void MARSDKPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( MARSDKService );
    }
    //////////////////////////////////////////////////////////////////////////
}
