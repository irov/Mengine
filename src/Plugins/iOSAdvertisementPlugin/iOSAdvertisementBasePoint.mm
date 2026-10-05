#import "iOSAdvertisementBasePoint.h"

#import "Environment/Apple/AppleDetail.h"

#import "Environment/iOS/iOSLog.h"

@implementation iOSAdvertisementBasePoint

- (instancetype)initWithName:(NSString *)name withJson:(NSDictionary *)json {
    self = [super init];

    if (self != nil) {
        self.m_name = name;

        self.m_id = [self parseAdPointInteger:json key:@"id" required:NO defaultValue:1 minValue:nil maxValue:nil];
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

- (void)setAttempts:(iOSAdvertisementAttempts *)attempts {
    self.m_attempts = attempts;
}

- (iOSAdvertisementAttempts *)getAttempts {
    return self.m_attempts;
}

- (void)setCooldown:(iOSAdvertisementCooldown *)cooldown {
    self.m_cooldown = cooldown;
}

- (iOSAdvertisementCooldown *)getCooldown {
    return self.m_cooldown;
}

- (NSString *)getCooldownGroupName {
    return self.m_cooldownGroupName;
}

- (BOOL)parseAdPointBoolean:(NSDictionary *)json key:(NSString *)key required:(BOOL)required defaultValue:(BOOL)defaultValue {
    id value = [json objectForKey:key];

    if (value == nil) {
        if (required) {
            IOS_LOGGER_ERROR(@"[iOSAdvertisementBasePoint] required key not found: %@", key);
        }

        return defaultValue;
    }

    if ([value isKindOfClass:[NSNumber class]] == NO) {
        IOS_LOGGER_ERROR(@"[iOSAdvertisementBasePoint] invalid key NSNumber type: %@, %@", key, NSStringFromClass([value class]));

        return defaultValue;
    }

    BOOL result = [value boolValue];

    return result;
}

- (NSInteger)parseAdPointInteger:(NSDictionary *)json key:(NSString *)key required:(BOOL)required defaultValue:(NSInteger)defaultValue minValue:(NSNumber *)minValue maxValue:(NSNumber *)maxValue {
    id value = [json objectForKey:key];

    if (value == nil) {
        if (required) {
            IOS_LOGGER_ERROR(@"[iOSAdvertisementBasePoint] required key not found: %@", key);
        }

        return defaultValue;
    }

    if ([value isKindOfClass:[NSNumber class]] == NO) {
        Class valueClass = [value class];
        NSString * valueClassName = NSStringFromClass(valueClass);

        IOS_LOGGER_ERROR(@"[iOSAdvertisementBasePoint] invalid key NSNumber type: %@, %@", key, valueClassName);

        return defaultValue;
    }

    CFTypeID valueTypeId = CFGetTypeID((__bridge CFTypeRef)value);
    CFTypeID booleanTypeId = CFBooleanGetTypeID();

    if (valueTypeId == booleanTypeId) {
        Class valueClass = [value class];
        NSString * valueClassName = NSStringFromClass(valueClass);

        IOS_LOGGER_ERROR(@"[iOSAdvertisementBasePoint] invalid key integer type: %@, %@", key, valueClassName);

        return defaultValue;
    }

    if (minValue != nil) {
        if ([value compare:minValue] == NSOrderedAscending) {
            IOS_LOGGER_ERROR(@"%@ attribute %@ must be >= %@, but got %@; using default %ld"
                , self.m_name
                , key
                , minValue
                , value
                , (long)defaultValue
            );

            return defaultValue;
        }
    }

    if (maxValue != nil) {
        if ([value compare:maxValue] == NSOrderedDescending) {
            IOS_LOGGER_ERROR(@"%@ attribute %@ must be <= %@, but got %@; using default %ld"
                , self.m_name
                , key
                , maxValue
                , value
                , (long)defaultValue
            );

            return defaultValue;
        }
    }

    NSInteger result = [value integerValue];

    return result;
}

- (NSInteger)parseAdPointTimeInterval:(NSDictionary *)json key:(NSString *)key required:(BOOL)required defaultValue:(NSInteger)defaultValue minValue:(NSNumber *)minValue maxValue:(NSNumber *)maxValue {
    NSInteger resultSec = [self parseAdPointInteger:json key:key required:required defaultValue:defaultValue minValue:minValue maxValue:maxValue];

    NSInteger resultMillisec = resultSec * 1000;

    return resultMillisec;
}

- (NSString *)parseAdPointString:(NSDictionary *)json key:(NSString *)key required:(BOOL)required defaultValue:(NSString *)defaultValue {
    id value = [json objectForKey:key];

    if (value == nil) {
        if (required) {
            IOS_LOGGER_ERROR(@"[iOSAdvertisementBasePoint] required key not found: %@", key);
        }

        return defaultValue;
    }

    if ([value isKindOfClass:[NSString class]] == NO) {
        IOS_LOGGER_ERROR(@"[iOSAdvertisementBasePoint] invalid key NSString type: %@, %@", key, NSStringFromClass([value class]));

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
