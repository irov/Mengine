#pragma once

#include "Plugins/MCPPlugin/MCPProtocol.h"

#import <Foundation/Foundation.h>
#import <Network/Network.h>

NS_ASSUME_NONNULL_BEGIN

@interface AppleWebViewMCPTransport : NSObject
{
@private
    nw_connection_t m_connection;

    Mengine::MCPFrameDecoder m_decoder;

    void (^m_handler)(NSString *, NSDictionary *, uint32_t);
}

- (void)startWithHandler:(void (^)(NSString *, NSDictionary *, uint32_t))_handler;
- (void)stop;

- (void)reply:(uint32_t)_requestId result:(id)_result;
- (void)error:(uint32_t)_requestId message:(NSString *)_message;
- (void)binary:(uint32_t)_requestId data:(NSData *)_data;

@end

NS_ASSUME_NONNULL_END
