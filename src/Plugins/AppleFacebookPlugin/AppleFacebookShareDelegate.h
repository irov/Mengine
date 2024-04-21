#pragma once

#include "AppleFacebookInterface.h"

#import <AuthenticationServices/AuthenticationServices.h>
#import <SafariServices/SafariServices.h>

#import <FBSDKCoreKit/FBSDKCoreKit-Swift.h>
#import <FBSDKShareKit/FBSDKShareKit-Swift.h>

@interface AppleFacebookShareDelegate : NSObject<FBSDKSharingDelegate>

@property (assign) Mengine::AppleFacebookServiceInterface * _Nonnull m_service;

- (instancetype _Nonnull) initWithService:(Mengine::AppleFacebookServiceInterface* _Nonnull)service;

@end
