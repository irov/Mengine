#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"

#import "AppleFirebaseRemoteConfigInterface.h"

@interface AppleFirebaseRemoteConfigApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface, AppleFirebaseRemoteConfigInterface>

@property (nonatomic) NSDictionary * m_configs;
@property (nonatomic) NSDictionary * m_ids;

@end
