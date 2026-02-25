#pragma once

#import "Environment/iOS/iOSPluginInterface.h"

@interface AppleSentryPlugin : NSObject<iOSPluginInterface>

@property (assign) BOOL m_sendAllow;

@end
