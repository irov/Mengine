#include "AppleSentryPlugin.h"

#include "Environment/Apple/AppleDetail.h"

#include "Kernel/OptionHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AppleSentryService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AppleSentry, Mengine::AppleSentryPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleSentryPlugin::AppleSentryPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleSentryPlugin::~AppleSentryPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSentryPlugin::_unimportantPlugin() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSentryPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "sentry" ) == true )
        {
            return true;
        }

        if( HAS_OPTION( "nosentry" ) == true )
        {
            return false;
        }
        
        BOOL AppleSentryPlugin_Available = Helper::AppleGetBundlePluginConfigBoolean( @("MengineAppleSentryPlugin"), @("Available"), YES );

        if( AppleSentryPlugin_Available == NO )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleSentryPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( AppleSentryService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSentryPlugin::_finalizePlugin()
    {        
        SERVICE_FINALIZE( AppleSentryService );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleSentryPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( AppleSentryService );
    }
    //////////////////////////////////////////////////////////////////////////
}
