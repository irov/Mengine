#pragma once

#import "Environment/iOS/iOSPluginInterface.h"
#import "Environment/iOS/iOSPluginAdRevenueDelegateInterface.h"

@interface AppleSKAdNetworkPlugin : NSObject<iOSPluginInterface, iOSPluginAdRevenueDelegateInterface>

+ (instancetype)sharedInstance;

@property (nonatomic) BOOL m_closed;

@property (nonatomic) NSInteger m_sendFine;
@property (nonatomic) NSInteger m_acceptFine;

@property (nonatomic) NSTimeInterval m_time;

@property (nonatomic, strong) NSDictionary * m_config;
@property (nonatomic) double m_adRevenue;

@end
