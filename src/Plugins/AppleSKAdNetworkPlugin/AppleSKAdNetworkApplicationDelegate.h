#pragma once

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"
#import "Environment/iOS/iOSPluginAdRevenueDelegateInterface.h"

@interface AppleSKAdNetworkApplicationDelegate : NSObject<iOSPluginApplicationDelegateInterface, iOSPluginAdRevenueDelegateInterface>

+ (instancetype)sharedInstance;

@property (nonatomic) BOOL m_closed;

@property (nonatomic) NSInteger m_sendFine;
@property (nonatomic) NSInteger m_acceptFine;

@property (nonatomic) NSTimeInterval m_time;

@property (nonatomic, strong) NSDictionary * m_config;
@property (nonatomic) double m_adRevenue;

@end
