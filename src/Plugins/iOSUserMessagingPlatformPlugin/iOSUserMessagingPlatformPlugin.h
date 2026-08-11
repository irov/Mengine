#pragma once

#import "Environment/iOS/iOSPluginInterface.h"

NS_ASSUME_NONNULL_BEGIN

@interface iOSUserMessagingPlatformPlugin : NSObject<iOSPluginInterface>

+ (instancetype)sharedInstance;

- (void)showConsentFlow;
- (BOOL)isConsentFlowUserGeographyGDPR;
- (void)waitForConsentCompletion:(void (^)(void))completion;

@property (atomic, assign) BOOL m_completed;
@property (nonatomic, strong) NSMutableArray<void (^)(void)> * m_completionHandlers;

@end


NS_ASSUME_NONNULL_END
