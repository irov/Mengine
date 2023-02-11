#include "Win32SentryPlugin.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( Win32SentryService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Win32Sentry, Mengine::Win32SentryPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32SentryPlugin::Win32SentryPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32SentryPlugin::~Win32SentryPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32SentryPlugin::_unimportantPlugin() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32SentryPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "sentry" ) == true )
        {
            return true;
        }

        if( HAS_OPTION( "nosentry" ) == true )
        {
            return false;
        }
        
        bool SentryPlugin_Available = CONFIG_VALUE( "Win32SentryPlugin", "Available", true );

        if( SentryPlugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32SentryPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( Win32SentryService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32SentryPlugin::_finalizePlugin()
    {
        SERVICE_FINALIZE( Win32SentryService );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32SentryPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( Win32SentryService );
    }
    //////////////////////////////////////////////////////////////////////////
}
