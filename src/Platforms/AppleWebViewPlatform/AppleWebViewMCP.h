#pragma once

#import "AppleWebViewMCPApplicationInterface.h"

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class AppleWebViewMCPTransport;

@interface AppleWebViewMCP : NSObject
{
@private
    AppleWebViewMCPTransport * m_transport;

    __weak id<AppleWebViewMCPApplicationInterface> m_application;
}

- (void)startWithDelegate:(id<AppleWebViewMCPApplicationInterface>)_delegate;
- (void)stop;

@end

NS_ASSUME_NONNULL_END
