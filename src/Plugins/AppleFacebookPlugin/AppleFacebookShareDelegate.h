#pragma once

#include "AppleFacebookInterface.h"

#import <FBSDKCoreKit/FBSDKCoreKit.h>
#import <FBSDKLoginKit/FBSDKLoginKit.h>
#import <FBSDKShareKit/FBSDKShareKit.h>
#import <FBSDKShareKit/FBSDKShareKit-Swift.h>
#import <Foundation/Foundation.h>

@interface AppleFacebookShareDelegate : NSObject<FBSDKSharingDelegate>

@property (assign) Mengine::AppleFacebookServiceInterface * _Nonnull m_service;

- (instancetype _Nonnull) initWithService:(Mengine::AppleFacebookServiceInterface* _Nonnull)service;

@end
