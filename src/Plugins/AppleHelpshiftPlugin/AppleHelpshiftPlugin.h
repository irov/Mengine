#pragma once

#import "Environment/iOS/iOSPluginInterface.h"
#import "AppleHelpshiftInterface.h"
#import "AppleHelpshiftDelegate.h"

@interface AppleHelpshiftPlugin : NSObject<iOSPluginInterface, AppleHelpshiftInterface>

@property (nonatomic, strong) id<AppleHelpshiftProviderInterface> m_provider;
@property (nonatomic, strong) AppleHelpshiftDelegate * m_delegate;

@end
