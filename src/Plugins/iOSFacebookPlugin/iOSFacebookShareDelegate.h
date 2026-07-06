#pragma once

#include "iOSFacebookInterface.h"

#import <AuthenticationServices/AuthenticationServices.h>
#import <SafariServices/SafariServices.h>

#import <FBSDKCoreKit/FBSDKCoreKit-Swift.h>
#import <FBSDKShareKit/FBSDKShareKit-Swift.h>

@interface iOSFacebookShareDelegate : NSObject<FBSDKSharingDelegate>

@property (nonatomic, weak) id<iOSFacebookInterface> m_facebook;

- (instancetype _Nonnull) initWithFacebook:(id<iOSFacebookInterface> _Nonnull)facebook;

@end
