#include "ResourcePrefetcherPlugin.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( ResourcePrefetcherService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( ResourcePrefetcher, Mengine::ResourcePrefetcherPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourcePrefetcherPlugin::ResourcePrefetcherPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourcePrefetcherPlugin::~ResourcePrefetcherPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourcePrefetcherPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "resourceprefetcher" ) == true )
        {
            return true;
        }

        if( HAS_OPTION( "noresourceprefetcher" ) == true )
        {
            return false;
        }

        bool ResourcePrefetcherPlugin_Available = CONFIG_VALUE( "ResourcePrefetcherPlugin", "Available", true );

        if( ResourcePrefetcherPlugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourcePrefetcherPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( ResourcePrefetcherService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourcePrefetcherPlugin::_finalizePlugin()
    {
        SERVICE_FINALIZE( ResourcePrefetcherService );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourcePrefetcherPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( ResourcePrefetcherService );
    }
    //////////////////////////////////////////////////////////////////////////
}
