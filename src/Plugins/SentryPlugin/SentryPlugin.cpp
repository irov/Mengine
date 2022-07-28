#include "SentryPlugin.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( SentryService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Sentry, Mengine::SentryPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SentryPlugin::SentryPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SentryPlugin::~SentryPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SentryPlugin::_unimportantPlugin() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SentryPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "sentry" ) == true )
        {
            return true;
        }

        if( HAS_OPTION( "nosentry" ) == true )
        {
            return false;
        }
        
        bool SentryPlugin_Available = CONFIG_VALUE( "SentryPlugin", "Available", true );

        if( SentryPlugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SentryPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( SentryService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SentryPlugin::_finalizePlugin()
    {
        SERVICE_FINALIZE( SentryService );
    }
    //////////////////////////////////////////////////////////////////////////
    void SentryPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( SentryService );
    }
    //////////////////////////////////////////////////////////////////////////
}
