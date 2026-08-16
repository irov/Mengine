#include "MCPPlugin.h"

#include "MCPService.h"

#include "Interface/SocketSystemInterface.h"
#include "Interface/ThreadSystemInterface.h"

#include "Kernel/BuildMode.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/PluginHelper.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "MCPScriptEmbedding.h"
#   include "Kernel/ScriptEmbeddingHelper.h"
#endif

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( MCPService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( MCP, Mengine::MCPPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPPlugin::MCPPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MCPPlugin::~MCPPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPPlugin::_unimportantPlugin() const
    {
#if defined(MENGINE_PLATFORM_WINDOWS) || defined(MENGINE_PLATFORM_MACOS) || defined(MENGINE_PLATFORM_LINUX) || defined(MENGINE_PLATFORM_IOS)
        if( HAS_OPTION( "mcp" ) == false )
        {
            return false;
        }
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const ServiceRequiredList & MCPPlugin::requiredServices() const
    {
        static ServiceRequiredList required = {
            SERVICE_ID( SocketSystemInterface ),
            SERVICE_ID( ThreadSystemInterface )
        };

        return required;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPPlugin::_availablePlugin() const
    {
        bool available = false;

        if( Helper::isBuildPublish() == true || Helper::isDebugMode() == false )
        {
            return available;
        }

        if( HAS_OPTION( "nomcp" ) == true )
        {
            return available;
        }

#if defined(MENGINE_PLATFORM_ANDROID)
        available = true;
#elif defined(MENGINE_PLATFORM_WINDOWS) || defined(MENGINE_PLATFORM_MACOS) || defined(MENGINE_PLATFORM_LINUX) || defined(MENGINE_PLATFORM_IOS)
        available = true;
#endif

        return available;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( MCPService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        if( Helper::addScriptEmbedding<MCPScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            SERVICE_FINALIZE( MCPService );
            SERVICE_DESTROY( MCPService );

            return false;
        }
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPPlugin::_finalizePlugin()
    {
        if( SERVICE_IS_INITIALIZE( MCPServiceInterface ) == true )
        {
            MCP_SERVICE()->stop();
        }

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        Helper::removeScriptEmbedding<MCPScriptEmbedding>();
#endif

        SERVICE_FINALIZE( MCPService );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( MCPService );
    }
    //////////////////////////////////////////////////////////////////////////
}
