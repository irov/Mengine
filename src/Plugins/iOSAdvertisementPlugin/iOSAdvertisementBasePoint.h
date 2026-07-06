#pragma once

#import "Environment/Apple/AppleIncluder.h"

#import "iOSAdvertisementAttempts.h"
#import "iOSAdvertisementCooldown.h"

@interface iOSAdvertisementBasePoint : NSObject

- (instancetype)initWithName:(NSString *)name withJson:(NSDictionary *)json;

- (NSString *)getName;
- (NSInteger)getId;
- (BOOL)isEnabled;

- (void)setAttempts:(iOSAdvertisementAttempts *)attempts;
- (iOSAdvertisementAttempts *)getAttempts;

- (void)setCooldown:(iOSAdvertisementCooldown *)cooldown;
- (iOSAdvertisementCooldown *)getCooldown;

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

@property (nonatomic, strong) iOSAdvertisementAttempts * m_attempts;
@property (nonatomic, strong) iOSAdvertisementCooldown * m_cooldown;

@property (nonatomic) NSInteger m_lastShowTime;

@end
