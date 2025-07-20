#pragma once

#import "Environment/Apple/AppleIncluder.h"

@interface iOSAdRevenueParam : NSObject

@property (nonatomic, strong) NSString * _Nonnull REVENUE_PLATFORM;
@property (nonatomic, strong) NSString * _Nonnull REVENUE_COUNTRY_CODE;
@property (nonatomic, strong) NSString * _Nullable REVENUE_PLACEMENT;
@property (nonatomic, strong) NSString * _Nonnull REVENUE_NETWORK_PLACEMENT;
@property (nonatomic, strong) NSString * _Nonnull REVENUE_SOURCE;
@property (nonatomic, strong) NSString * _Nonnull REVENUE_FORMAT;
@property (nonatomic, strong) NSString * _Nonnull REVENUE_UNIT;
@property (nonatomic, strong) NSString * _Nonnull REVENUE_CURRENCY;
@property (nonatomic, strong) NSNumber * _Nonnull REVENUE_VALUE;

@end
