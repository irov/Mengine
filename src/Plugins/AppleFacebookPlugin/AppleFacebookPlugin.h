#pragma once

#import "Environment/iOS/iOSPluginInterface.h"
#import "AppleFacebookInterface.h"

#import "AppleFacebookShareDelegate.h"

#import <FBSDKLoginKit/FBSDKLoginKit-Swift.h>

@interface AppleFacebookPlugin : NSObject<iOSPluginInterface, AppleFacebookInterface>

@property (nonatomic, strong) id<AppleFacebookProviderInterface> m_provider;
@property (nonatomic, strong) FBSDKLoginManager * m_loginManager;
@property (nonatomic, strong) AppleFacebookShareDelegate * m_shareDelegate;

@end
