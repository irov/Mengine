#pragma once

#import "Environment/iOS/iOSPluginInterface.h"
#import "iOSFacebookInterface.h"

#import "iOSFacebookShareDelegate.h"

#import <FBSDKLoginKit/FBSDKLoginKit-Swift.h>

@interface iOSFacebookPlugin : NSObject<iOSPluginInterface, iOSFacebookInterface>

@property (nonatomic, strong) id<iOSFacebookProviderInterface> m_provider;
@property (nonatomic, strong) FBSDKLoginManager * m_loginManager;
@property (nonatomic, strong) iOSFacebookShareDelegate * m_shareDelegate;
@property (nonatomic, strong) NSString * m_userId;

@end
