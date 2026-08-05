#include "MCPPlugin.h"

#include "MCPService.h"

#include "Interface/NotificationServiceInterface.h"
#include "Interface/SocketSystemInterface.h"
#include "Interface/ThreadSystemInterface.h"

#include "Kernel/BuildMode.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/ParamsHelper.h"
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
#if defined(MENGINE_PLATFORM_ANDROID)
            SERVICE_ID( NotificationServiceInterface ),
#endif
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

#if defined(MENGINE_PLATFORM_ANDROID)
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_APPLICATION_INTENT_START, &MCPPlugin::notifyApplicationIntent_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_APPLICATION_INTENT_NEW, &MCPPlugin::notifyApplicationIntent_, MENGINE_DOCUMENT_FACTORABLE );
#elif defined(MENGINE_PLATFORM_WINDOWS) || defined(MENGINE_PLATFORM_MACOS) || defined(MENGINE_PLATFORM_LINUX) || defined(MENGINE_PLATFORM_IOS)
        if( HAS_OPTION( "mcp" ) == true )
        {
            const String host = GET_OPTION_VALUE( "mcp-host", "127.0.0.1" );
            const String port = GET_OPTION_VALUE( "mcp-port", "" );
            const String token = GET_OPTION_VALUE( "mcp-token", "" );
            const String mode = GET_OPTION_VALUE( "mcp-mode", "visible" );

            if( MCP_SERVICE()->run( host, port, token, mode ) == false )
            {
                LOGGER_ERROR( "MCP activation failed" );

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
                Helper::removeScriptEmbedding<MCPScriptEmbedding>();
#endif

                SERVICE_FINALIZE( MCPService );
                SERVICE_DESTROY( MCPService );

                return false;
            }
        }
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPPlugin::_finalizePlugin()
    {
#if defined(MENGINE_PLATFORM_ANDROID)
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_APPLICATION_INTENT_START );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_APPLICATION_INTENT_NEW );
#endif

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
#if defined(MENGINE_PLATFORM_ANDROID)
    void MCPPlugin::notifyApplicationIntent_( const String & _action, const String & _data, const String & _type, uint32_t _flags, const Params & _extras )
    {
        MENGINE_UNUSED( _action );
        MENGINE_UNUSED( _data );
        MENGINE_UNUSED( _type );
        MENGINE_UNUSED( _flags );

        String host = Helper::getParam( _extras, STRINGIZE_STRING_LOCAL( "mengine.mcp.host" ), "" );
        String port = Helper::getParam( _extras, STRINGIZE_STRING_LOCAL( "mengine.mcp.port" ), "" );
        String token = Helper::getParam( _extras, STRINGIZE_STRING_LOCAL( "mengine.mcp.token" ), "" );
        String mode = Helper::getParam( _extras, STRINGIZE_STRING_LOCAL( "mengine.mcp.mode" ), "" );

        if( host.empty() == true || port.empty() == true || token.size() != 64 || mode.empty() == true )
        {
            return;
        }

        MCP_SERVICE()->stop();

        if( MCP_SERVICE()->run( host, port, token, mode ) == false )
        {
            LOGGER_ERROR( "MCP Android intent activation failed" );
        }
    }
#endif
    //////////////////////////////////////////////////////////////////////////
}
