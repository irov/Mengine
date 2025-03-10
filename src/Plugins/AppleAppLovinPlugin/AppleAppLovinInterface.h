#pragma once

#include "Config/Config.h"

#import <Foundation/Foundation.h>

@protocol AppleAppLovinConsentFlowProviderInterface <NSObject>

- (void)onAppleAppLovinConsentFlowShowSuccess;
- (void)onAppleAppLovinConsentFlowShowFailed;

@end

@protocol AppleAppLovinInterface <NSObject>

+ (instancetype)sharedInstance;

- (BOOL)hasSupportedCMP;
- (BOOL)isConsentFlowUserGeographyGDPR;
- (BOOL)loadAndShowCMPFlow:(id<AppleAppLovinConsentFlowProviderInterface>)provider;
- (void)showMediationDebugger;

@end


