#pragma once

#import "Environment/Apple/AppleIncluder.h"

@protocol iOSAppLovinConsentFlowProviderInterface <NSObject>

- (void)oniOSAppLovinConsentFlowShowSuccess;
- (void)oniOSAppLovinConsentFlowShowFailed;

@end

@protocol iOSAppLovinInterface <NSObject>

+ (instancetype)sharedInstance;

- (BOOL)hasSupportedCMP;
- (BOOL)isConsentFlowUserGeographyGDPR;
- (BOOL)loadAndShowCMPFlow:(id<iOSAppLovinConsentFlowProviderInterface>)provider;
- (void)showMediationDebugger;

@end
