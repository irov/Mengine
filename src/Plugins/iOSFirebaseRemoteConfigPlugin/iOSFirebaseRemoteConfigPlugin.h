#pragma once

#import "Environment/iOS/iOSPluginInterface.h"

#import "iOSFirebaseRemoteConfigInterface.h"

@interface iOSFirebaseRemoteConfigPlugin : NSObject<iOSPluginInterface, iOSFirebaseRemoteConfigInterface>

@property (nonatomic) NSDictionary * m_configs;
@property (nonatomic) NSDictionary * m_ids;
@property (nonatomic, copy) NSDictionary * m_signals;

@end
