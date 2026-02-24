#import "AppleMARSDKAdRewardedDelegate.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/iOS/iOSLog.h"

@implementation AppleMARSDKAdRewardedDelegate

#pragma mark -

- (instancetype _Nonnull)initWithMARSDK:(id<AppleMARSDKInterface>)marsdk {
    self = [super init];
    
    self.m_marsdk = marsdk;
    
    return self;
}

#pragma mark - MARAdRewardedDelegate

- (void) MARAdRewardedDidFailed:(MARAdErrorCode)code withMessage: (NSString *)message adDict:(NSDictionary *)adDict {
    IOS_LOGGER_MESSAGE( @"MARAdRewardedDidFailed code: %d message: %s adDict: %s"
        , (int32_t)code
        , [message UTF8String]
        , [[NSString stringWithFormat:@"%@", adDict] UTF8String]
    );

    id<AppleMARSDKProviderInterface> provider = [self.m_marsdk getProvider];

    if( provider == nil )
    {
        return;
    }

    [AppleDetail addMainQueueOperation:^{
        [provider onAdRewardedDidFailed];
    }];
}

- (void) MARAdRewardedDidLoaded:(NSDictionary *)adDict {
    IOS_LOGGER_MESSAGE( @"MARAdRewardedDidLoaded adDict: %s"
        , [[NSString stringWithFormat:@"%@", adDict] UTF8String]
    );

    id<AppleMARSDKProviderInterface> provider = [self.m_marsdk getProvider];

    if( provider == nil )
    {
        return;
    }

    [AppleDetail addMainQueueOperation:^{
        [provider onAdRewardedDidLoaded];
    }];
}

- (void) MARAdRewardedDidShow:(NSDictionary *)adDict {
    IOS_LOGGER_MESSAGE( @"MARAdRewardedDidShow adDict: %s"
        , [[NSString stringWithFormat:@"%@", adDict] UTF8String]
    );

    id<AppleMARSDKProviderInterface> provider = [self.m_marsdk getProvider];

    if( provider == nil )
    {
        return;
    }

    [AppleDetail addMainQueueOperation:^{
        [provider onAdRewardedDidShow];
    }];
}

- (void) MARAdRewardedDidShowFailed:(NSDictionary *)adDict {
    IOS_LOGGER_MESSAGE( @"MARAdRewardedDidShowFailed adDict: %s"
        , [[NSString stringWithFormat:@"%@", adDict] UTF8String]
    );

    id<AppleMARSDKProviderInterface> provider = [self.m_marsdk getProvider];

    if( provider == nil )
    {
        return;
    }

    [AppleDetail addMainQueueOperation:^{
        [provider onAdRewardedDidShowFailed];
    }];
}

- (void) MARAdRewardedDidClicked:(NSDictionary *)adDict {
    IOS_LOGGER_MESSAGE( @"MARAdRewardedDidClicked adDict: %s"
        , [[NSString stringWithFormat:@"%@", adDict] UTF8String]
    );

    id<AppleMARSDKProviderInterface> provider = [self.m_marsdk getProvider];

    if( provider == nil )
    {
        return;
    }

    [AppleDetail addMainQueueOperation:^{
        [provider onAdRewardedDidClicked];
    }];
}

- (void) MARAdRewardedDidClosed:(NSDictionary *)adDict {
    IOS_LOGGER_MESSAGE( @"MARAdRewardedDidClosed adDict: %s"
        , [[NSString stringWithFormat:@"%@", adDict] UTF8String]
    );

    id<AppleMARSDKProviderInterface> provider = [self.m_marsdk getProvider];

    if( provider == nil )
    {
        return;
    }

    [AppleDetail addMainQueueOperation:^{
        [provider onAdRewardedDidClosed];
    }];
}

- (void) MARAdRewardedDidSkipped:(NSDictionary *)adDict {
    IOS_LOGGER_MESSAGE( @"MARAdRewardedDidSkipped adDict: %s"
        , [[NSString stringWithFormat:@"%@", adDict] UTF8String]
    );

    id<AppleMARSDKProviderInterface> provider = [self.m_marsdk getProvider];

    if( provider == nil )
    {
        return;
    }

    [AppleDetail addMainQueueOperation:^{
        [provider onAdRewardedDidSkipped];
    }];
}

- (void) MARAdRewardedDidFinished:(NSString *)itemName itemNum:(int)itemNum adDict:(NSDictionary *)adDict {
    IOS_LOGGER_MESSAGE( @"MARAdRewardedDidFinished adDict: %s"
        , [[NSString stringWithFormat:@"%@", adDict] UTF8String]
    );

    id<AppleMARSDKProviderInterface> provider = [self.m_marsdk getProvider];

    if( provider == nil )
    {
        return;
    }

    [AppleDetail addMainQueueOperation:^{
        [provider onAdRewardedDidFinished:itemName itemNum:itemNum];
    }];
}

@end
