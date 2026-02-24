#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"
#import "AppleHelpshiftInterface.h"
#import "AppleHelpshiftDelegate.h"

@interface AppleHelpshiftApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface, AppleHelpshiftInterface>

@property (nonatomic, strong) id<AppleHelpshiftProviderInterface> m_provider;
@property (nonatomic, strong) AppleHelpshiftDelegate * m_delegate;

@end
