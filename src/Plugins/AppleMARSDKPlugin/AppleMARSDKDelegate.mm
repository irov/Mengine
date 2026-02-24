#import "AppleMARSDKDelegate.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"

@implementation AppleMARSDKDelegate

#pragma mark -

- (instancetype _Nonnull)initWithMARSDK:(id<AppleMARSDKInterface>)marsdk {
    self = [super init];
    
    self.m_marsdk = marsdk;
    
    return self;
}

- (UIView *)GetView {
    UIView * view = [self GetViewController].view;
    
    return view;
}

- (UIViewController *)GetViewController {
    UIViewController * rootViewController = Mengine::Helper::iOSGetRootViewController();
    
    return rootViewController;
}

- (void) OnPlatformInit: (NSDictionary *)params {
    IOS_LOGGER_MESSAGE( @"OnPlatformInit params: %s"
        , [[NSString stringWithFormat:@"%@", params] UTF8String]
    );

    id<AppleMARSDKProviderInterface> provider = [self.m_marsdk getProvider];

    if( provider == nil )
    {
        return;
    }

    [AppleDetail addMainQueueOperation:^{
        [provider onPlatformInit:params];
    }];
}

- (void) OnRealName:(NSDictionary*)params {
    IOS_LOGGER_MESSAGE( @"OnRealName params: %s"
        , [[NSString stringWithFormat:@"%@", params] UTF8String]
    );

    id<AppleMARSDKProviderInterface> provider = [self.m_marsdk getProvider];

    if( provider == nil )
    {
        return;
    }

    [AppleDetail addMainQueueOperation:^{
        [provider onRealName:params];
    }];
}

- (void) OnEventWithCode:(int)code msg: (NSString*)msg {
    IOS_LOGGER_MESSAGE( @"OnEventWithCode code:%d msg:%s"
        , code
        , [msg UTF8String]
    );

    id<AppleMARSDKProviderInterface> provider = [self.m_marsdk getProvider];

    if( provider == nil )
    {
        return;
    }

    [AppleDetail addMainQueueOperation:^{
        [provider onEventWithCode:code msg:msg];
    }];
}

- (void) OnEventCustom:(NSString*)eventName params:(NSDictionary*)params {
    IOS_LOGGER_MESSAGE( @"OnEventCustom event:%s params:%s"
        , [eventName UTF8String]
        , [[NSString stringWithFormat:@"%@", params] UTF8String]
    );

    id<AppleMARSDKProviderInterface> provider = [self.m_marsdk getProvider];

    if( provider == nil )
    {
        return;
    }

    [AppleDetail addMainQueueOperation:^{
        [provider onEventCustom:eventName params:params];
    }];
}

- (void) OnUserLogin: (NSDictionary *)params {
    IOS_LOGGER_MESSAGE( @"OnUserLogin params:%s"
        , [[NSString stringWithFormat:@"%@", params] UTF8String]
    );

    id<AppleMARSDKProviderInterface> provider = [self.m_marsdk getProvider];

    if( provider == nil )
    {
        return;
    }

    [AppleDetail addMainQueueOperation:^{
        [provider onUserLogin:params];
    }];
}

- (void) OnUserLogout: (NSDictionary *)params {
    IOS_LOGGER_MESSAGE( @"OnUserLogout params:%s"
        , [[NSString stringWithFormat:@"%@", params] UTF8String]
    );

    id<AppleMARSDKProviderInterface> provider = [self.m_marsdk getProvider];

    if( provider == nil )
    {
        return;
    }

    [AppleDetail addMainQueueOperation:^{
        [provider onUserLogout:params];
    }];
}

- (void) OnPayPaid: (NSDictionary *)params {
    IOS_LOGGER_MESSAGE( @"OnPayPaid params:%s"
        , [[NSString stringWithFormat:@"%@", params] UTF8String]
    );

    id<AppleMARSDKProviderInterface> provider = [self.m_marsdk getProvider];

    if( provider == nil )
    {
        return;
    }

    [AppleDetail addMainQueueOperation:^{
        [provider onPayPaid:params];
    }];
}

@end
