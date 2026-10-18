#include "Win32WebViewMCP.h"

#include "Interface/LoggerServiceInterface.h"
#include "Interface/WebViewBridgeServiceInterface.h"
#include "Interface/WebViewPlatformServiceInterface.h"

#include "Environment/Windows/Win32UnicodeHelper.h"
#include "Environment/Windows/Win32WebViewPlatformServiceExtensionInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/JSONHelper.h"
#include "Kernel/ProxyLogger.h"
#include "Kernel/StringHelper.h"
#include "Kernel/ThreadMutexHelper.h"
#include "Kernel/ThreadMutexScope.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32WebViewMCP::Win32WebViewMCP()
        : m_window( nullptr )
        , m_socket( INVALID_SOCKET )
        , m_outputOffset( 0 )
        , m_connecting( false )
        , m_stopping( false )
        , m_logSequence( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32WebViewMCP::~Win32WebViewMCP()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32WebViewMCP::initialize( HWND _window, int _argc, WChar ** _argv )
    {
        m_window = _window;
        String host;
        String port;
        String token;

        for( int index = 1; index != _argc; ++index )
        {
            String argument;
            if( Helper::Win32UnicodeToUtf8( _argv[index], &argument ) == false )
            {
                return false;
            }

            if( argument.compare( 0, 11, "--mcp-host:" ) == 0 )
            {
                host = argument.substr( 11 );
            }
            else if( argument.compare( 0, 11, "--mcp-port:" ) == 0 )
            {
                port = argument.substr( 11 );
            }
            else if( argument.compare( 0, 12, "--mcp-token:" ) == 0 )
            {
                token = argument.substr( 12 );
            }
        }

        if( token.empty() == true )
        {
            return true;
        }

        if( host.empty() == true )
        {
            return false;
        }

        if( port.empty() == true )
        {
            return false;
        }

        if( this->connect_( host, port ) == false )
        {
            return false;
        }

        m_logMutex = Helper::createThreadMutex( nullptr );

        if( m_logMutex == nullptr )
        {
            return false;
        }

        m_logs = jpp::make_array();
        ProxyLoggerPtr collector = Helper::makeFactorableUnique<ProxyLogger>( nullptr );
        collector->setLambda( [this]( const LoggerRecordInterfacePtr & _record )
        {
            LoggerMessage message;
            _record->getMessage( &message );

            const Char * category = message.category != nullptr ? message.category : "";
            String text( message.data, message.size );
            jpp::object record = jpp::make_object();
            record.set( "timestamp", message.timestamp );
            record.set( "level", static_cast<uint32_t>(message.level) );
            record.set( "category", category );
            record.set( "message", text );
            MENGINE_THREAD_MUTEX_SCOPE( m_logMutex );

            if( m_logs.size() >= 5000 )
            {
                m_logs.remove_front();
            }

            uint64_t sequence = ++m_logSequence;
            record.set( "sequence", sequence );
            m_logs.push_back( record );
        } );
        collector->setWriteHistory( true );

        if( LOGGER_SERVICE()
            ->registerLogger( collector ) == false )
        {
            return false;
        }

        m_logCollector = collector;

        jpp::array capabilities = jpp::make_array();
        capabilities.push_back( "script_call" );
        capabilities.push_back( "logs_read" );
        capabilities.push_back( "frame_capture" );
        capabilities.push_back( "app_stop" );
        jpp::object params = jpp::make_object();
        params.set( "token", token );
        params.set( "capabilities", capabilities );
        jpp::object request = jpp::make_object();
        request.set( "method", "handshake" );
        request.set( "params", params );
        this->sendJSON_( EMCPFrameType::Request, 0, request );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32WebViewMCP::finalize()
    {
        if( m_logCollector != nullptr )
        {
            LOGGER_SERVICE()
                ->unregisterLogger( m_logCollector );

            m_logCollector = nullptr;
        }

        m_logs = jpp::array::none();
        m_logMutex = nullptr;

        if( m_socket != INVALID_SOCKET )
        {
            ::closesocket( m_socket );
            m_socket = INVALID_SOCKET;
        }

        m_output.clear();
        m_decoder.reset();
    }
    //////////////////////////////////////////////////////////////////////////
    jpp::object Win32WebViewMCP::status_() const
    {
        bool documentReady = WEBVIEW_BRIDGE_SERVICE()
            ->isDocumentReady();
        bool appReady = WEBVIEW_BRIDGE_SERVICE()
            ->isAppReady();
        uint64_t generation = WEBVIEW_BRIDGE_SERVICE()
            ->getDocumentGeneration();
        String number = Helper::stringUInt64( generation );
        jpp::object status = jpp::make_object();
        status.set( "complete", appReady );
        status.set( "documentReady", documentReady );
        status.set( "appReady", appReady );
        status.set( "generation", number );

        return status;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32WebViewMCP::tick()
    {
        if( m_socket == INVALID_SOCKET )
        {
            return;
        }

        if( m_connecting == true )
        {
            WSAPOLLFD poll{m_socket, POLLWRNORM, 0};
            int result = ::WSAPoll( &poll, 1, 0 );

            if( result == 0 )
            {
                return;
            }

            if( result < 0 )
            {
                ::PostMessageW( m_window, WM_CLOSE, 0, 0 );

                return;
            }

            if( (poll.revents & (POLLERR | POLLHUP | POLLNVAL)) != 0 )
            {
                ::PostMessageW( m_window, WM_CLOSE, 0, 0 );

                return;
            }

            m_connecting = false;
        }

        if( m_outputOffset < m_output.size() )
        {
            const Char * data = reinterpret_cast<const Char *>(m_output.data() + m_outputOffset);
            int size = static_cast<int>(m_output.size() - m_outputOffset);
            int sent = ::send( m_socket, data, size, 0 );

            if( sent > 0 )
            {
                m_outputOffset += static_cast<size_t>(sent);

                if( m_outputOffset == m_output.size() )
                {
                    m_output.clear();
                    m_outputOffset = 0;
                }
            }
            else if( ::WSAGetLastError() != WSAEWOULDBLOCK )
            {
                ::PostMessageW( m_window, WM_CLOSE, 0, 0 );

                return;
            }
        }

        if( m_stopping == true )
        {
            if( m_output.empty() == false )
            {
                return;
            }

            ::PostMessageW( m_window, WM_CLOSE, 0, 0 );

            return;
        }

        Char buffer[16384];
        int receiveSize = static_cast<int>(sizeof( buffer ));
        int received = ::recv( m_socket, buffer, receiveSize, 0 );

        if( received == 0 )
        {
            ::PostMessageW( m_window, WM_CLOSE, 0, 0 );

            return;
        }

        if( received < 0 )
        {
            if( ::WSAGetLastError() != WSAEWOULDBLOCK )
            {
                ::PostMessageW( m_window, WM_CLOSE, 0, 0 );
            }

            return;
        }

        size_t receivedSize = static_cast<size_t>(received);
        VectorMCPFrames frames;
        const Char * error = nullptr;
        if( m_decoder.append( buffer, receivedSize, &frames, &error ) == false )
        {
            ::PostMessageW( m_window, WM_CLOSE, 0, 0 );

            return;
        }

        for( const MCPFrame & frame : frames )
        {
            if( frame.type == EMCPFrameType::Request )
            {
                this->onRequest_( frame );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32WebViewMCP::connect_( const String & _host, const String & _port )
    {
        addrinfo hints{};
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        addrinfo * address = nullptr;
        const Char * host = _host.c_str();
        const Char * port = _port.c_str();

        if( ::getaddrinfo( host, port, &hints, &address ) != 0 )
        {
            return false;
        }

        for( const addrinfo * entry = address; entry != nullptr; entry = entry->ai_next )
        {
            SOCKET socket = ::socket( entry->ai_family, entry->ai_socktype, entry->ai_protocol );

            if( socket == INVALID_SOCKET )
            {
                continue;
            }

            u_long nonblocking = 1;
            if( ::ioctlsocket( socket, FIONBIO, &nonblocking ) != 0 )
            {
                ::closesocket( socket );

                continue;
            }

            int addressLength = static_cast<int>(entry->ai_addrlen);
            int result = ::connect( socket, entry->ai_addr, addressLength );
            if( result != 0 )
            {
                if( ::WSAGetLastError() != WSAEWOULDBLOCK )
                {
                    ::closesocket( socket );

                    continue;
                }
            }

            m_socket = socket;
            m_connecting = result != 0;

            break;
        }

        ::freeaddrinfo( address );

        if( m_socket == INVALID_SOCKET )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32WebViewMCP::queueFrame_( const Data & _frame )
    {
        if( m_output.size() + _frame.size() > MCP_PROTOCOL_MAX_ATTACHMENT )
        {
            m_stopping = true;

            return;
        }

        m_output.insert( m_output.end(), _frame.begin(), _frame.end() );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32WebViewMCP::sendJSON_( EMCPFrameType _type, uint32_t _id, const jpp::object & _value )
    {
        Data json;
        Helper::writeJSONDataCompact( _value, &json );
        const void * data = json.data();
        size_t size = json.size();

        if( size > MCP_PROTOCOL_MAX_PAYLOAD )
        {
            this->error_( _id, "response exceeds payload limit" );

            return;
        }

        Data frame = Helper::makeMCPJSONFrame( _type, _id, data, size );
        this->queueFrame_( frame );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32WebViewMCP::reply_( uint32_t _id, const jpp::object & _value )
    {
        jpp::object reply = jpp::make_object();
        reply.set( "result", _value );
        this->sendJSON_( EMCPFrameType::Response, _id, reply );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32WebViewMCP::error_( uint32_t _id, const Char * _message )
    {
        jpp::object error = jpp::make_object();
        error.set( "code", "execution_error" );
        error.set( "message", _message );
        jpp::object reply = jpp::make_object();
        reply.set( "error", error );
        this->sendJSON_( EMCPFrameType::Response, _id, reply );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32WebViewMCP::onRequest_( const MCPFrame & _frame )
    {
        const void * data = _frame.payload.data();
        size_t size = _frame.payload.size();
        jpp::object request = Helper::loadJSONBuffer( data, size, nullptr );

        if( request.is_type_object() == false )
        {
            this->error_( _frame.requestId, "invalid MCP request" );

            return;
        }

        jpp::object methodField;
        if( request.exist( "method", &methodField ) == false )
        {
            this->error_( _frame.requestId, "method required" );

            return;
        }

        if( methodField.is_type_string() == false )
        {
            this->error_( _frame.requestId, "method must be a string" );

            return;
        }

        String method = methodField;
        jpp::object defaultParams = jpp::make_object();
        jpp::object params = request.get( "params", defaultParams );

        if( params.is_type_object() == false )
        {
            this->error_( _frame.requestId, "params must be an object" );

            return;
        }

        Win32WebViewPlatformServiceExtensionInterface * platform = WEBVIEW_PLATFORM_SERVICE()
            ->getUnknown();

        if( method == "script_call" )
        {
            jpp::object nameField;
            if( params.exist( "name", &nameField ) == false )
            {
                this->error_( _frame.requestId, "handler name required" );

                return;
            }

            if( nameField.is_type_string() == false )
            {
                this->error_( _frame.requestId, "handler name must be a string" );

                return;
            }

            String name = nameField;

            if( name == "webview.status" )
            {
                jpp::object status = this->status_();
                this->reply_( _frame.requestId, status );
            }
            else if( name == "webview.reload" || name == "webview.recreate" )
            {
                if( name == "webview.reload" )
                {
                    platform->reload();
                }
                else
                {
                    platform->retryBrowser();
                }

                jpp::object result = jpp::make_true();
                this->reply_( _frame.requestId, result );
            }
            else
            {
                this->error_( _frame.requestId, "unknown native WebView handler" );
            }
        }
        else if( method == "logs_read" )
        {
            int64_t afterValue = 0;
            jpp::object afterField;
            if( params.exist( "after", &afterField ) == true )
            {
                if( afterField.is_type_integer() == false )
                {
                    this->error_( _frame.requestId, "after must be an integer" );

                    return;
                }

                afterValue = afterField;
            }

            int64_t limit = 500;
            jpp::object limitField;
            if( params.exist( "limit", &limitField ) == true )
            {
                if( limitField.is_type_integer() == false )
                {
                    this->error_( _frame.requestId, "limit must be an integer" );

                    return;
                }

                limit = limitField;
            }

            if( limit <= 0 )
            {
                this->error_( _frame.requestId, "limit must be positive" );

                return;
            }

            if( limit > 5000 )
            {
                limit = 5000;
            }

            uint64_t after = afterValue > 0 ? static_cast<uint64_t>(afterValue) : 0;
            uint64_t next = after;
            jpp::array records = jpp::make_array();

            {
                MENGINE_THREAD_MUTEX_SCOPE( m_logMutex );

                for( size_t index = 0; index != m_logs.size(); ++index )
                {
                    jpp::object record = m_logs[index];
                    uint64_t sequence = record.get( "sequence", MENGINE_UINT64_C(0) );

                    if( sequence <= after )
                    {
                        continue;
                    }

                    records.push_back( record );
                    next = sequence;

                    if( records.size() >= static_cast<size_t>(limit) )
                    {
                        break;
                    }
                }
            }

            jpp::object result = jpp::make_object();
            result.set( "records", records );
            result.set( "next", next );
            this->reply_( _frame.requestId, result );
        }
        else if( method == "frame_capture" )
        {
            uint32_t id = _frame.requestId;
            platform->takeSnapshot( [this, id]( const Data & _data, uint32_t _width, uint32_t _height )
            {
                if( _data.empty() == true )
                {
                    this->error_( id, "snapshot cancelled or unavailable" );

                    return;
                }

                jpp::object attachment = jpp::make_object();
                size_t size = _data.size();
                attachment.set( "size", size );
                attachment.set( "mimeType", "image/png" );
                jpp::object result = jpp::make_object();
                result.set( "width", _width );
                result.set( "height", _height );
                result.set( "attachment", attachment );
                this->reply_( id, result );
                const void * data = _data.data();
                Data frame = Helper::makeMCPFrame( EMCPFrameType::Binary, MCP_FRAME_FLAG_FINAL, id, 0, 1, data, size );
                this->queueFrame_( frame );
            } );
        }
        else if( method == "app_stop" )
        {
            jpp::object result = jpp::make_true();
            this->reply_( _frame.requestId, result );
            m_stopping = true;
        }
        else
        {
            this->error_( _frame.requestId, "unsupported MCP method" );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
