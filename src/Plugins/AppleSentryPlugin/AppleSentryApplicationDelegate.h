#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"

@interface AppleSentryApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface>

@property (assign) BOOL m_sendAllow;

@end
