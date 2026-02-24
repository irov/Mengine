#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"
#import "AppleFacebookInterface.h"

#import "AppleFacebookShareDelegate.h"

#import <FBSDKLoginKit/FBSDKLoginKit-Swift.h>

@interface AppleFacebookApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface, AppleFacebookInterface>

@property (nonatomic, strong) id<AppleFacebookProviderInterface> m_provider;
@property (nonatomic, strong) FBSDKLoginManager * m_loginManager;
@property (nonatomic, strong) AppleFacebookShareDelegate * m_shareDelegate;

@end
