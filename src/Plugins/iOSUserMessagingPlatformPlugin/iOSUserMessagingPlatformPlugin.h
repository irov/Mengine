#pragma once

#import "Environment/iOS/iOSPluginInterface.h"
#import "iOSUserMessagingPlatformInterface.h"

NS_ASSUME_NONNULL_BEGIN

@interface iOSUserMessagingPlatformPlugin : NSObject<iOSPluginInterface, iOSUserMessagingPlatformInterface>

+ (instancetype)sharedInstance;

- (void)showConsentFlow;
- (BOOL)isConsentFlowUserGeographyGDPR;
- (void)waitForConsentCompletion:(void (^)(void))completion;

@property (atomic, assign) BOOL m_completed;
@property (atomic, assign) iOSUserMessagingPlatformConsentState m_consentState;
@property (nonatomic, strong) NSMutableArray<void (^)(void)> * m_completionHandlers;

@end


NS_ASSUME_NONNULL_END
