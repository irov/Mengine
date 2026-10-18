#include "WebViewBridgeService.h"

#include "Kernel/AssertionMainThreadGuard.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( WebViewBridgeService, Mengine::WebViewBridgeService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    const size_t WEBVIEW_BRIDGE_MAX_PENDING_REQUESTS = 128;
    const size_t WEBVIEW_BRIDGE_MAX_IDENTIFIER_LENGTH = 128;
    //////////////////////////////////////////////////////////////////////////
    WebViewBridgeService::WebViewBridgeService()
        : m_generation( 0 )
        , m_documentOpen( false )
        , m_documentReady( false )
        , m_appReady( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    WebViewBridgeService::~WebViewBridgeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool WebViewBridgeService::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void WebViewBridgeService::_finalizeService()
    {
        this->closeDocument();
        m_sender = nullptr;
        m_handlers.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void WebViewBridgeService::setSender( const WebViewBridgeSenderInterfacePtr & _sender )
    {
        m_sender = _sender;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WebViewBridgeService::addHandler( const String & _name, const LambdaHandler & _handler )
    {
        if( _name.empty() == true )
        {
            return false;
        }

        if( _name.size() > WEBVIEW_BRIDGE_MAX_IDENTIFIER_LENGTH )
        {
            return false;
        }

        if( _handler == nullptr )
        {
            return false;
        }

        if( m_handlers.find( _name ) != m_handlers.end() )
        {
            return false;
        }

        m_handlers.emplace( _name, _handler );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void WebViewBridgeService::removeHandler( const String & _name )
    {
        m_handlers.erase( _name );
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t WebViewBridgeService::beginDocument()
    {
        this->closeDocument();
        m_documentOpen = true;

        return m_generation;
    }
    //////////////////////////////////////////////////////////////////////////
    void WebViewBridgeService::closeDocument()
    {
        ++m_generation;
        m_documentOpen = false;
        m_documentReady = false;
        m_appReady = false;
        m_pending.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t WebViewBridgeService::getDocumentGeneration() const
    {
        return m_generation;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WebViewBridgeService::isDocumentReady() const
    {
        return m_documentReady;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WebViewBridgeService::isAppReady() const
    {
        return m_appReady;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WebViewBridgeService::receiveMessage( uint64_t _generation, const jpp::object & _message )
    {
        MENGINE_ASSERTION_MAIN_THREAD_GUARD();

        if( m_documentOpen == false )
        {
            return false;
        }

        if( _generation != m_generation )
        {
            return false;
        }

        if( _message.is_type_object() == false )
        {
            return false;
        }

        jpp::object kind;
        if( _message.exist( "kind", &kind ) == false )
        {
            return false;
        }

        if( kind.is_type_string() == false )
        {
            return false;
        }

        String kindValue = kind;

        if( kindValue == "ready" )
        {
            m_documentReady = true;

            return true;
        }

        if( m_documentReady == false )
        {
            return false;
        }

        if( kindValue == "appReady" )
        {
            m_appReady = true;

            return true;
        }

        if( kindValue != "request" )
        {
            return false;
        }

        jpp::object id;
        if( _message.exist( "id", &id ) == false )
        {
            return false;
        }

        if( id.is_type_string() == false )
        {
            return false;
        }

        jpp::object method;
        if( _message.exist( "method", &method ) == false )
        {
            return false;
        }

        if( method.is_type_string() == false )
        {
            return false;
        }

        jpp::object arguments;
        if( _message.exist( "arguments", &arguments ) == false )
        {
            return false;
        }

        if( arguments.is_type_object() == false )
        {
            return false;
        }

        String requestId = id;
        String methodName = method;

        if( requestId.empty() == true )
        {
            return false;
        }

        if( requestId.size() > WEBVIEW_BRIDGE_MAX_IDENTIFIER_LENGTH )
        {
            return false;
        }

        if( methodName.size() > WEBVIEW_BRIDGE_MAX_IDENTIFIER_LENGTH )
        {
            return false;
        }

        if( m_pending.size() >= WEBVIEW_BRIDGE_MAX_PENDING_REQUESTS )
        {
            return false;
        }

        if( m_pending.find( requestId ) != m_pending.end() )
        {
            return false;
        }

        m_pending.emplace( requestId );

        MapHandlers::const_iterator it = m_handlers.find( methodName );

        if( it == m_handlers.end() )
        {
            jpp::object result = jpp::make_null();
            if( this->completeRequest( _generation, requestId, result, "unknown_method" ) == false )
            {
                return false;
            }

            return true;
        }

        LambdaHandler handler = it->second;
        handler( _generation, requestId, arguments );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool WebViewBridgeService::completeRequest( uint64_t _generation, const String & _id, const jpp::object & _result, const String & _error )
    {
        MENGINE_ASSERTION_MAIN_THREAD_GUARD();

        if( m_documentOpen == false )
        {
            return false;
        }

        if( _generation != m_generation )
        {
            return false;
        }

        if( m_pending.erase( _id ) == 0 )
        {
            return false;
        }

        WebViewBridgeSenderInterfacePtr sender = m_sender;

        if( sender == nullptr )
        {
            return false;
        }

        jpp::object response = jpp::make_object();
        response.set( "kind", "response" );
        response.set( "id", _id );

        if( _error.empty() == false )
        {
            response.set( "error", _error );
        }
        else
        {
            response.set( "result", _result );
        }

        if( sender->onWebViewBridgeSenderSendJSON( response ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
