#pragma once

#import "Environment/Apple/AppleIncluder.h"

#import "AppleAdvertisementAttempts.h"
#import "AppleAdvertisementCooldown.h"

@interface AppleAdvertisementBasePoint : NSObject

- (instancetype)initWithName:(NSString *)name withJson:(NSDictionary *)json;

- (NSString *)getName;
- (NSInteger)getId;
- (BOOL)isEnabled;

- (void)setAttempts:(AppleAdvertisementAttempts *)attempts;
- (AppleAdvertisementAttempts *)getAttempts;

- (void)setCooldown:(AppleAdvertisementCooldown *)cooldown;
- (AppleAdvertisementCooldown *)getCooldown;

- (NSString *)getCooldownGroupName;

- (BOOL)parseAdPointBoolean:(NSDictionary *)json key:(NSString *)key required:(BOOL)required defaultValue:(BOOL)defaultValue;
- (NSInteger)parseAdPointInteger:(NSDictionary *)json key:(NSString *)key required:(BOOL)required defaultValue:(NSInteger)defaultValue;
- (NSInteger)parseAdPointTimeInterval:(NSDictionary *)json key:(NSString *)key required:(BOOL)required defaultValue:(NSInteger)defaultValue;
- (NSString *)parseAdPointString:(NSDictionary *)json key:(NSString *)key required:(BOOL)required defaultValue:(NSString *)defaultValue;

- (void)showAd;

@property (nonatomic, strong) NSString * m_name;

@property (nonatomic) NSInteger m_id;
@property (nonatomic) BOOL m_enabled;

@property (nonatomic, strong) NSString * m_cooldownGroupName;

@property (nonatomic, strong) AppleAdvertisementAttempts * m_attempts;
@property (nonatomic, strong) AppleAdvertisementCooldown * m_cooldown;

@property (nonatomic) NSInteger m_lastShowTime;

@end
