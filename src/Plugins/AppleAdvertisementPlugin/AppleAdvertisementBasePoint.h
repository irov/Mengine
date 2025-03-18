#pragma once

#include "Config/Config.h"

#import "AppleAdvertisementAttempts.h"
#import "AppleAdvertisementCooldown.h"

#import <Foundation/Foundation.h>

@interface AppleAdvertisementBasePoint : NSObject

- (instancetype)init:(NSString *)name withJson:(NSDictionary *)json;

- (NSString *)getName;
- (NSInteger)getId;
- (BOOL)isEnabled;

- (void)setAttempts:(AppleAdvertisementAttempts *)attempts;
- (AppleAdvertisementAttempts *)getAttempts;

- (void)setCooldown:(AppleAdvertisementCooldown *)cooldown;
- (AppleAdvertisementCooldown *)getCooldown;

- (BOOL)parseAdPointBoolean:(NSDictionary *)json key:(NSString *)key required:(BOOL)required defaultValue:(BOOL)defaultValue;
- (NSInteger)parseAdPointInteger:(NSDictionary *)json key:(NSString *)key required:(BOOL)required defaultValue:(NSInteger)defaultValue;
- (NSTimeInterval)parseAdPointTimeInterval:(NSDictionary *)json key:(NSString *)key required:(BOOL)required defaultValue:(NSTimeInterval)defaultValue;
- (NSString *)parseAdPointString:(NSDictionary *)json key:(NSString *)key required:(BOOL)required defaultValue:(NSString *)defaultValue;

- (void)showAd;

@property (nonatomic, strong) NSString * m_name;

@property (nonatomic) NSInteger m_id;
@property (nonatomic) BOOL m_enabled;

@property (nonatomic, strong) NSString * m_cooldownGroupName;

@property (nonatomic, strong) AppleAdvertisementAttempts * m_attempts;
@property (nonatomic, strong) AppleAdvertisementCooldown * m_cooldown;

@property (nonatomic) NSTimeInterval m_lastShowTime;

@end
