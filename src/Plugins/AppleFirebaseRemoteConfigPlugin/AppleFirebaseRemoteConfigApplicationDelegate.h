#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"

@interface AppleFirebaseRemoteConfigApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface>

+ (AppleFirebaseRemoteConfigApplicationDelegate *) sharedInstance;

- (BOOL)existRemoteConfigValue:(NSString *)key;
- (NSDictionary *)getRemoteConfigValue:(NSString *)key;

@property (nonatomic) NSDictionary * m_configs;

@end
