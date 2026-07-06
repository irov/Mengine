#pragma once

#import "Environment/iOS/iOSPluginInterface.h"

@interface iOSSentryPlugin : NSObject<iOSPluginInterface>

@property (assign) BOOL m_sendAllow;

@end
