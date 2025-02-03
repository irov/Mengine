#include "RenderDocPlugin.h"

#include "Kernel/Logger.h"
#include "Kernel/AllocatorHelper.h"
#include "Kernel/AssertionAllocator.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( RenderDocService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( RenderDoc, Mengine::RenderDocPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    RenderDocPlugin::RenderDocPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    RenderDocPlugin::~RenderDocPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderDocPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "renderdoc" ) == false )
        {
            return false;
        }

        if( HAS_OPTION( "norenderdoc" ) == true )
        {
            return false;
        }

        bool RenderDocPlugin_Available = CONFIG_VALUE_BOOLEAN( "RenderDocPlugin", "Available", true );

        if( RenderDocPlugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderDocPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( RenderDocService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderDocPlugin::_finalizePlugin()
    {
        SERVICE_FINALIZE( RenderDocService );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderDocPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( RenderDocService );
    }
    //////////////////////////////////////////////////////////////////////////
}