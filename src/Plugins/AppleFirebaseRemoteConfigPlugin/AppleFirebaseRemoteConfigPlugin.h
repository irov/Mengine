#pragma once

#import "Environment/iOS/iOSPluginInterface.h"

#import "AppleFirebaseRemoteConfigInterface.h"

@interface AppleFirebaseRemoteConfigPlugin : NSObject<iOSPluginInterface, AppleFirebaseRemoteConfigInterface>

@property (nonatomic) NSDictionary * m_configs;
@property (nonatomic) NSDictionary * m_ids;

@end
