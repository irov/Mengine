#pragma once

#import "Environment/iOS/iOSPluginInterface.h"

#import "iOSFirebaseRemoteConfigInterface.h"

@interface iOSFirebaseRemoteConfigPlugin : NSObject<iOSPluginInterface, iOSFirebaseRemoteConfigInterface>

@property (nonatomic) NSDictionary * m_configs;
@property (nonatomic) NSDictionary * m_ids;

@end
