#pragma once

#include "AppleFacebookInterface.h"

#import <AuthenticationServices/AuthenticationServices.h>
#import <SafariServices/SafariServices.h>

#import <FBSDKCoreKit/FBSDKCoreKit-Swift.h>
#import <FBSDKShareKit/FBSDKShareKit-Swift.h>

@interface AppleFacebookShareDelegate : NSObject<FBSDKSharingDelegate>

@property (nonatomic, weak) id<AppleFacebookInterface> m_facebook;

- (instancetype _Nonnull) initWithFacebook:(id<AppleFacebookInterface> _Nonnull)facebook;

@end
