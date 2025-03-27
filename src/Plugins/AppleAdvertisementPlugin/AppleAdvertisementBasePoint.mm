#import "AppleAdvertisementBasePoint.h"

#import "Environment/Apple/AppleDetail.h"

#import "Environment/iOS/iOSLog.h"

@implementation AppleAdvertisementBasePoint

- (instancetype)initWithName:(NSString *)name withJson:(NSDictionary *)json {
    self = [super init];
    
    if (self != nil) {
        self.m_name = name;
        
        self.m_id = [self parseAdPointInteger:json key:@"id" required:NO defaultValue:1];
        self.m_enabled = [self parseAdPointBoolean:json key:@"enable" required:YES defaultValue:NO];
        
        self.m_cooldownGroupName = [self parseAdPointString:json key:@"trigger_cooldown_group" required:NO defaultValue:nil];
        
        self.m_lastShowTime = 0;
    }
    
    return self;
}

- (NSString *)getName {
    return self.m_name;
}

- (NSInteger)getId {
    return self.m_id;
}

- (BOOL)isEnabled {
    return self.m_enabled;
}

- (void)setAttempts:(AppleAdvertisementAttempts *)attempts {
    self.m_attempts = attempts;
}

- (AppleAdvertisementAttempts *)getAttempts {
    return self.m_attempts;
}

- (void)setCooldown:(AppleAdvertisementCooldown *)cooldown {
    self.m_cooldown = cooldown;
}

- (AppleAdvertisementCooldown *)getCooldown {
    return self.m_cooldown;
}

- (BOOL)parseAdPointBoolean:(NSDictionary *)json key:(NSString *)key required:(BOOL)required defaultValue:(BOOL)defaultValue {
    id value = [json objectForKey:key];
    
    if (value == nil) {
        if (required) {
            IOS_LOGGER_ERROR(@"[AppleAdvertisementBasePoint] required key not found: %@", key);
        }
        
        return defaultValue;
    }
    
    if ([value isKindOfClass:[NSNumber class]] == NO) {
        IOS_LOGGER_ERROR(@"[AppleAdvertisementBasePoint] invalid key NSNumber type: %@, %@", key, NSStringFromClass([value class]));
        
        return defaultValue;
    }
    
    BOOL result = [value boolValue];
    
    return result;
}

- (NSInteger)parseAdPointInteger:(NSDictionary *)json key:(NSString *)key required:(BOOL)required defaultValue:(NSInteger)defaultValue {
    id value = [json objectForKey:key];
    
    if (value == nil) {
        if (required) {
            IOS_LOGGER_ERROR(@"[AppleAdvertisementBasePoint] required key not found: %@", key);
        }
        
        return defaultValue;
    }
    
    if ([value isKindOfClass:[NSNumber class]] == NO) {
        IOS_LOGGER_ERROR(@"[AppleAdvertisementBasePoint] invalid key NSNumber type: %@, %@", key, NSStringFromClass([value class]));
        
        return defaultValue;
    }
    
    NSInteger result = [value integerValue];
    
    return result;
}

- (NSInteger)parseAdPointTimeInterval:(NSDictionary *)json key:(NSString *)key required:(BOOL)required defaultValue:(NSInteger)defaultValue {
    id value = [json objectForKey:key];
    
    if (value == nil) {
        if (required) {
            IOS_LOGGER_ERROR(@"[AppleAdvertisementBasePoint] required key not found: %@", key);
        }
        
        return defaultValue;
    }
    
    if ([value isKindOfClass:[NSNumber class]] == NO) {
        IOS_LOGGER_ERROR(@"[AppleAdvertisementBasePoint] invalid key NSNumber type: %@, %@", key, NSStringFromClass([value class]));
        
        return defaultValue;
    }

    NSInteger resultSec = [value integerValue];
    
    NSInteger resultMillisec = resultSec * 1000;
    
    return resultMillisec;
}

- (NSString *)parseAdPointString:(NSDictionary *)json key:(NSString *)key required:(BOOL)required defaultValue:(NSString *)defaultValue {
    id value = [json objectForKey:key];
    
    if (value == nil) {
        if (required) {
            IOS_LOGGER_ERROR(@"[AppleAdvertisementBasePoint] required key not found: %@", key);
        }
        
        return defaultValue;
    }
    
    if ([value isKindOfClass:[NSString class]] == NO) {
        IOS_LOGGER_ERROR(@"[AppleAdvertisementBasePoint] invalid key NSString type: %@, %@", key, NSStringFromClass([value class]));
        
        return defaultValue;
    }
    
    NSString * result = value;
    
    return result;
}

- (void)showAd {
    self.m_lastShowTime = [AppleDetail getTimestamp];
    
    [self.m_cooldown resetShownTimestamp];
}

@end
