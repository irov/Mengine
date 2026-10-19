#import "AppleWebViewMCPTransport.h"

#include "Config/StdLib.h"

@implementation AppleWebViewMCPTransport

#pragma mark - Options

+ (NSString *)getMCPOption:(NSString *)_name
{
    NSString * prefix = [NSString stringWithFormat:@"--mcp-%@:", _name];
    NSArray<NSString *> * arguments = NSProcessInfo.processInfo.arguments;

    for( NSString * argument in arguments )
    {
        if( [argument hasPrefix:prefix] == NO )
        {
            continue;
        }

        NSUInteger prefixLength = prefix.length;
        NSString * value = [argument substringFromIndex:prefixLength];

        return value;
    }

    return nil;
}

#pragma mark - Lifecycle

- (void)dealloc
{
    [self stop];
}

- (void)startWithHandler:(void (^)(NSString *, NSDictionary *, uint32_t))_handler
{
    [self stop];

    NSString * host = [AppleWebViewMCPTransport getMCPOption:@"host"];

    if( host == nil )
    {
        return;
    }

    NSString * port = [AppleWebViewMCPTransport getMCPOption:@"port"];

    if( port == nil )
    {
        return;
    }

    NSString * token = [AppleWebViewMCPTransport getMCPOption:@"token"];

    if( token == nil )
    {
        return;
    }

    if( [port canBeConvertedToEncoding:NSASCIIStringEncoding] == NO )
    {
        return;
    }

    NSScanner * scanner = [NSScanner scannerWithString:port];

    NSInteger portValue;
    if( [scanner scanInteger:&portValue] == NO )
    {
        return;
    }

    if( scanner.scanLocation != port.length )
    {
        return;
    }

    if( portValue < 0 )
    {
        return;
    }

    NSInteger maxPort = 65535;

    if( portValue > maxPort )
    {
        return;
    }

    nw_endpoint_t endpoint = nw_endpoint_create_host( host.UTF8String, port.UTF8String );

    if( endpoint == nil )
    {
        return;
    }

    nw_parameters_t parameters = nw_parameters_create_secure_tcp( NW_PARAMETERS_DISABLE_PROTOCOL, NW_PARAMETERS_DEFAULT_CONFIGURATION );

    if( parameters == nil )
    {
        return;
    }

    nw_connection_t connection = nw_connection_create( endpoint, parameters );

    if( connection == nil )
    {
        return;
    }

    dispatch_queue_t queue = dispatch_get_main_queue();
    nw_connection_set_queue( connection, queue );

    NSDictionary * handshake = @{
        @"method": @"handshake",
        @"params": @{
            @"token": token,
            @"capabilities": @[@"script_call", @"logs_read", @"frame_capture", @"app_stop"]
        }
    };

    __weak AppleWebViewMCPTransport * weakSelf = self;
    __weak nw_connection_t weakConnection = connection;
    nw_connection_set_state_changed_handler( connection, ^(nw_connection_state_t _state, nw_error_t _error)
    {
        MENGINE_UNUSED( _error );

        AppleWebViewMCPTransport * strongSelf = weakSelf;

        if( strongSelf == nil )
        {
            return;
        }

        nw_connection_t activeConnection = weakConnection;

        if( activeConnection == nil )
        {
            return;
        }

        if( strongSelf->m_connection != activeConnection )
        {
            return;
        }

        switch( _state )
        {
        case nw_connection_state_ready:
            {
                [strongSelf sendJSON:handshake requestId:0 type:Mengine::EMCPFrameType::Request];
                [strongSelf receive];
            }break;
        case nw_connection_state_failed:
        case nw_connection_state_cancelled:
            {
                Mengine::StdLib::exit( EXIT_SUCCESS );
            }break;
        default:
            break;
        }
    } );

    m_handler = [_handler copy];
    m_connection = connection;

    nw_connection_start( connection );
}

- (void)stop
{
    m_handler = nil;

    if( m_connection != nil )
    {
        nw_connection_set_state_changed_handler( m_connection, nil );
        nw_connection_cancel( m_connection );
        m_connection = nil;
    }

    m_decoder.reset();
}

#pragma mark - Replies

- (void)reply:(uint32_t)_requestId result:(id)_result
{
    NSDictionary * response = @{@"result": _result};

    if( [NSJSONSerialization isValidJSONObject:response] == NO )
    {
        [self error:_requestId message:@"result cannot be serialized as JSON"];

        return;
    }

    [self sendJSON:response requestId:_requestId type:Mengine::EMCPFrameType::Response];
}

- (void)error:(uint32_t)_requestId message:(NSString *)_message
{
    NSDictionary * response = @{
        @"error": @{
            @"code": @"execution_error",
            @"message": _message
        }
    };

    [self sendJSON:response requestId:_requestId type:Mengine::EMCPFrameType::Response];
}

- (void)binary:(uint32_t)_requestId data:(NSData *)_data
{
    const uint8_t * bytes = static_cast<const uint8_t *>(_data.bytes);
    NSUInteger size = _data.length;
    size_t chunkSize = 1024 * 1024;
    uint32_t chunkCount = static_cast<uint32_t>((size + chunkSize - 1) / chunkSize);

    if( chunkCount == 0 )
    {
        chunkCount = 1;
    }

    for( uint32_t chunkIndex = 0; chunkIndex != chunkCount; ++chunkIndex )
    {
        size_t offset = static_cast<size_t>(chunkIndex) * chunkSize;
        size_t count = size - offset;

        if( count > chunkSize )
        {
            count = chunkSize;
        }

        uint8_t flags = chunkIndex + 1 == chunkCount ? Mengine::MCP_FRAME_FLAG_FINAL : Mengine::MCP_FRAME_FLAG_NONE;
        const void * chunk = size == 0 ? nullptr : bytes + offset;
        Mengine::Data frame = Mengine::Helper::makeMCPFrame( Mengine::EMCPFrameType::Binary, flags, _requestId, chunkIndex, chunkCount, chunk, count );

        [self sendFrame:frame];
    }
}

#pragma mark - Sending

- (void)sendJSON:(id)_value requestId:(uint32_t)_requestId type:(Mengine::EMCPFrameType)_type
{
    NSData * data = [NSJSONSerialization dataWithJSONObject:_value options:NSJSONWritingSortedKeys | NSJSONWritingFragmentsAllowed error:nil];

    if( data == nil )
    {
        return;
    }

    const void * bytes = data.bytes;
    NSUInteger size = data.length;

    if( size > Mengine::MCP_PROTOCOL_MAX_PAYLOAD )
    {
        [self error:_requestId message:@"response exceeds payload limit"];

        return;
    }

    Mengine::Data frame = Mengine::Helper::makeMCPJSONFrame( _type, _requestId, bytes, size );

    [self sendFrame:frame];
}

- (void)sendFrame:(const Mengine::Data &)_frame
{
    if( m_connection == nil )
    {
        return;
    }

    const void * bytes = _frame.data();
    size_t size = _frame.size();

    dispatch_queue_t queue = dispatch_get_main_queue();
    dispatch_data_t content = dispatch_data_create( bytes, size, queue, DISPATCH_DATA_DESTRUCTOR_DEFAULT );

    nw_connection_send( m_connection, content, NW_CONNECTION_DEFAULT_MESSAGE_CONTEXT, true, ^(nw_error_t _error)
    {
        MENGINE_UNUSED( _error );

        //Empty
    } );
}

#pragma mark - Receiving

- (void)receive
{
    if( m_connection == nil )
    {
        return;
    }

    uint32_t receiveSize = 65536;
    __weak AppleWebViewMCPTransport * weakSelf = self;
    __weak nw_connection_t weakConnection = m_connection;
    nw_connection_receive( m_connection, 1, receiveSize, ^(dispatch_data_t _content, nw_content_context_t _context, bool _complete, nw_error_t _error)
    {
        MENGINE_UNUSED( _context );

        AppleWebViewMCPTransport * strongSelf = weakSelf;

        if( strongSelf == nil )
        {
            return;
        }

        nw_connection_t connection = weakConnection;

        if( connection == nil )
        {
            return;
        }

        if( strongSelf->m_connection != connection )
        {
            return;
        }

        if( _content != nil )
        {
            const void * bytes = nullptr;
            size_t size = 0;
            dispatch_data_t mapped __attribute__((objc_precise_lifetime)) = dispatch_data_create_map( _content, &bytes, &size );

            if( mapped == nil )
            {
                Mengine::StdLib::exit( EXIT_FAILURE );
            }

            if( size != 0 )
            {
                Mengine::VectorMCPFrames frames;
                const Mengine::Char * error = nullptr;
                if( strongSelf->m_decoder.append( bytes, size, &frames, &error ) == false )
                {
                    Mengine::StdLib::exit( EXIT_FAILURE );
                }

                for( const Mengine::MCPFrame & frame : frames )
                {
                    [strongSelf processFrame:frame];
                }
            }
        }

        if( _complete == true )
        {
            Mengine::StdLib::exit( EXIT_SUCCESS );
        }

        if( _error != nil )
        {
            Mengine::StdLib::exit( EXIT_SUCCESS );
        }

        [strongSelf receive];
    } );
}

- (void)processFrame:(const Mengine::MCPFrame &)_frame
{
    if( _frame.type != Mengine::EMCPFrameType::Request )
    {
        return;
    }

    const void * bytes = _frame.payload.data();
    size_t size = _frame.payload.size();
    NSData * payload = [NSData dataWithBytes:bytes length:size];
    id request = [NSJSONSerialization JSONObjectWithData:payload options:0 error:nil];

    if( [request isKindOfClass:NSDictionary.class] == NO )
    {
        return;
    }

    id method = request[@"method"];

    if( [method isKindOfClass:NSString.class] == NO )
    {
        return;
    }

    id params = request[@"params"];

    if( [params isKindOfClass:NSDictionary.class] == NO )
    {
        params = @{};
    }

    void (^handler)(NSString *, NSDictionary *, uint32_t) = m_handler;

    if( handler == nil )
    {
        return;
    }

    handler( method, params, _frame.requestId );
}

@end
