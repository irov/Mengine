#import "AppleWebViewMCP.h"

#import "AppleWebViewMCPTransport.h"

#include "Config/StdLib.h"

@implementation AppleWebViewMCP

#pragma mark - Lifecycle

- (instancetype)init
{
    self = [super init];

    if( self == nil )
    {
        return nil;
    }

    m_transport = [[AppleWebViewMCPTransport alloc] init];

    return self;
}

- (void)dealloc
{
    [m_transport stop];
}

- (void)startWithDelegate:(id<AppleWebViewMCPApplicationInterface>)_delegate
{
    m_application = _delegate;

    __weak AppleWebViewMCP * weakSelf = self;
    [m_transport startWithHandler:^(NSString * _method, NSDictionary * _params, uint32_t _requestId)
    {
        [weakSelf processRequest:_method params:_params requestId:_requestId];
    }];
}

- (void)stop
{
    [m_transport stop];
    m_application = nil;
}

#pragma mark - Requests

- (void)processRequest:(NSString *)_method params:(NSDictionary *)_params requestId:(uint32_t)_requestId
{
    id<AppleWebViewMCPApplicationInterface> application = m_application;

    if( application == nil )
    {
        [m_transport error:_requestId message:@"application closed"];

        return;
    }

    if( [_method isEqualToString:@"script_call"] == YES )
    {
        id name = _params[@"name"];

        if( [name isKindOfClass:NSString.class] == NO )
        {
            [m_transport error:_requestId message:@"unknown native WebView handler"];

            return;
        }

        if( [name isEqualToString:@"webview.status"] == YES )
        {
            NSDictionary * status = [application webViewStatus];
            [m_transport reply:_requestId result:status];

            return;
        }

        if( [name isEqualToString:@"webview.reload"] == YES )
        {
            [application reloadWebView];
            [m_transport reply:_requestId result:@{@"loading": @YES}];

            return;
        }

        if( [name isEqualToString:@"webview.evaluate"] == NO )
        {
            [m_transport error:_requestId message:@"unknown native WebView handler"];

            return;
        }

        id arguments = _params[@"arguments"];

        if( [arguments isKindOfClass:NSDictionary.class] == NO )
        {
            [m_transport error:_requestId message:@"script required"];

            return;
        }

        id script = arguments[@"script"];

        if( [script isKindOfClass:NSString.class] == NO )
        {
            [m_transport error:_requestId message:@"script required"];

            return;
        }

        __weak AppleWebViewMCP * weakSelf = self;
        [application evaluateWebView:script completion:^(id _result, NSError * _error)
        {
            AppleWebViewMCP * strongSelf = weakSelf;

            if( strongSelf == nil )
            {
                return;
            }

            if( _error != nil )
            {
                NSString * description = _error.localizedDescription;
                [strongSelf->m_transport error:_requestId message:description];

                return;
            }

            id value = _result ?: NSNull.null;
            [strongSelf->m_transport reply:_requestId result:value];
        }];

        return;
    }

    if( [_method isEqualToString:@"logs_read"] == YES )
    {
        NSNumber * afterValue = _params[@"after"] ?: @0;

        if( [afterValue isKindOfClass:NSNumber.class] == NO )
        {
            [m_transport error:_requestId message:@"after must be a number"];

            return;
        }

        NSNumber * limitValue = _params[@"limit"] ?: @500;

        if( [limitValue isKindOfClass:NSNumber.class] == NO )
        {
            [m_transport error:_requestId message:@"limit must be a number"];

            return;
        }

        int64_t afterNumber = afterValue.longLongValue;
        uint64_t after = afterNumber > 0 ? static_cast<uint64_t>(afterNumber) : 0;
        int64_t limit = limitValue.longLongValue;

        if( limit <= 0 )
        {
            [m_transport error:_requestId message:@"limit must be positive"];

            return;
        }

        if( limit > 5000 )
        {
            limit = 5000;
        }

        NSArray * history = [application readLogs];
        NSMutableArray * records = [[NSMutableArray alloc] init];
        uint64_t next = after;

        for( NSDictionary * record in history )
        {
            NSNumber * sequenceValue = record[@"sequence"];
            uint64_t sequence = sequenceValue.unsignedLongLongValue;

            if( sequence <= after )
            {
                continue;
            }

            [records addObject:record];
            next = sequence;

            if( records.count >= static_cast<NSUInteger>(limit) )
            {
                break;
            }
        }

        [m_transport reply:_requestId result:@{@"records": records, @"next": @(next)}];

        return;
    }

    if( [_method isEqualToString:@"frame_capture"] == YES )
    {
        __weak AppleWebViewMCP * weakSelf = self;
        [application captureWebView:^(NSData * _data, uint32_t _width, uint32_t _height)
        {
            AppleWebViewMCP * strongSelf = weakSelf;

            if( strongSelf == nil )
            {
                return;
            }

            NSUInteger size = _data.length;

            if( size == 0 )
            {
                [strongSelf->m_transport error:_requestId message:@"snapshot unavailable"];

                return;
            }

            if( size > Mengine::MCP_PROTOCOL_MAX_ATTACHMENT )
            {
                [strongSelf->m_transport error:_requestId message:@"snapshot exceeds attachment limit"];

                return;
            }

            NSDictionary * result = @{
                @"width": @(_width),
                @"height": @(_height),
                @"attachment": @{
                    @"size": @(size),
                    @"mimeType": @"image/png"
                }
            };

            [strongSelf->m_transport reply:_requestId result:result];
            [strongSelf->m_transport binary:_requestId data:_data];
        }];

        return;
    }

    if( [_method isEqualToString:@"app_stop"] == YES )
    {
        [m_transport reply:_requestId result:@{@"stopped": @YES}];

        int64_t stopDelay = 100 * NSEC_PER_MSEC;
        dispatch_time_t deadline = dispatch_time( DISPATCH_TIME_NOW, stopDelay );
        dispatch_queue_t queue = dispatch_get_main_queue();
        dispatch_after( deadline, queue, ^
        {
            [application stopApplication];

#if defined(MENGINE_PLATFORM_IOS)
            Mengine::StdLib::exit( EXIT_SUCCESS );
#endif
        } );

        return;
    }

    [m_transport error:_requestId message:@"unsupported method"];
}

@end
