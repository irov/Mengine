#pragma once

#import "Environment/iOS/iOSPluginInterface.h"
#import "iOSHelpshiftInterface.h"
#import "iOSHelpshiftDelegate.h"

@interface iOSHelpshiftPlugin : NSObject<iOSPluginInterface, iOSHelpshiftInterface>

@property (nonatomic, strong) id<iOSHelpshiftProviderInterface> m_provider;
@property (nonatomic, strong) iOSHelpshiftDelegate * m_delegate;

@end
