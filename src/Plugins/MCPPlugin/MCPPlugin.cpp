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

#include "Config/StdLib.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( MCPService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( MCP, Mengine::MCPPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPPlugin::MCPPlugin()
        : m_serviceCreated( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MCPPlugin::~MCPPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPPlugin::_unimportantPlugin() const
    {
        bool unimportant = true;

        return unimportant;
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
#else
        available = this->hasLaunchConfig_();
#endif

        return available;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPPlugin::_initializePlugin()
    {
        bool successful = true;

#if defined(MENGINE_PLATFORM_ANDROID)
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_APPLICATION_INTENT_START, &MCPPlugin::notifyApplicationIntent_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_APPLICATION_INTENT_NEW, &MCPPlugin::notifyApplicationIntent_, MENGINE_DOCUMENT_FACTORABLE );
#else
        if( this->hasLaunchConfig_() == true )
        {
            successful = this->initializeMCPService_();
        }
#endif

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPPlugin::_finalizePlugin()
    {
#if defined(MENGINE_PLATFORM_ANDROID)
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_APPLICATION_INTENT_START );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_APPLICATION_INTENT_NEW );
#endif

        if( m_serviceCreated == false )
        {
            return;
        }

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        Helper::removeScriptEmbedding<MCPScriptEmbedding>();
#endif

        SERVICE_FINALIZE( MCPService );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPPlugin::_destroyPlugin()
    {
        if( m_serviceCreated == true )
        {
            SERVICE_DESTROY( MCPService );
            m_serviceCreated = false;
        }

        MCPService::clearLaunchConfig();
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPPlugin::hasLaunchConfig_() const
    {
        const Char * host = StdLib::getenv( "MENGINE_MCP_HOST" );
        const Char * port = StdLib::getenv( "MENGINE_MCP_PORT" );
        const Char * token = StdLib::getenv( "MENGINE_MCP_TOKEN" );

        bool hasEnvironment = host != nullptr && host[0] != '\0'
            && port != nullptr && port[0] != '\0'
            && token != nullptr && token[0] != '\0';

        bool hasOptions = HAS_OPTION( "mcp-host" ) == true
            && HAS_OPTION( "mcp-port" ) == true
            && HAS_OPTION( "mcp-token" ) == true;

        bool hasLaunchConfig = hasEnvironment == true || hasOptions == true;

        return hasLaunchConfig;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPPlugin::initializeMCPService_()
    {
        bool successful = false;

        if( m_serviceCreated == true )
        {
            return successful;
        }

        if( SERVICE_CREATE( MCPService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return successful;
        }

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        if( Helper::addScriptEmbedding<MCPScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            SERVICE_FINALIZE( MCPService );
            SERVICE_DESTROY( MCPService );

            return successful;
        }
#endif

        m_serviceCreated = true;
        successful = true;

        return successful;
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

        if( host.empty() == true || port.empty() == true || token.empty() == true || mode.empty() == true )
        {
            return;
        }

        if( m_serviceCreated == true )
        {
            LOGGER_WARNING( "MCP ignored repeated Android intent activation" );

            return;
        }

        const Char * hostValue = host.c_str();
        const Char * portValue = port.c_str();
        const Char * tokenValue = token.c_str();
        const Char * modeValue = mode.c_str();
        MCPService::setLaunchConfig( hostValue, portValue, tokenValue, modeValue );

        if( this->initializeMCPService_() == false )
        {
            MCPService::clearLaunchConfig();

            LOGGER_ERROR( "MCP Android intent activation failed" );
        }
    }
#endif
    //////////////////////////////////////////////////////////////////////////
}
