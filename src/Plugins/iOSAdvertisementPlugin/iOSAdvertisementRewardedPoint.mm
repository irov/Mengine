#import "iOSAdvertisementRewardedPoint.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/Apple/AppleUserDefaults.h"

constexpr NSInteger DEFAULT_SHOW_LIMIT = -1;
constexpr NSInteger DEFAULT_SHOW_TIMEOUT = 86400;

@implementation iOSAdvertisementRewardedPoint

- (instancetype)initWithName:(NSString *)name withJson:(NSDictionary *)json {
    self = [super initWithName:name withJson:json];

    if (self != nil) {
        self.m_showLimit = [self parseAdPointInteger:json key:@"trigger_show_limit" required:NO defaultValue:DEFAULT_SHOW_LIMIT minValue:@(-1) maxValue:@(INT32_MAX)];
        self.m_showTimeout = [self parseAdPointTimeInterval:json key:@"trigger_show_timeout" required:NO defaultValue:DEFAULT_SHOW_TIMEOUT minValue:@1 maxValue:nil];
        NSString * frequencyKey = [@"ad.rewarded.frequency." stringByAppendingString:name];
        self.m_showStartKey = [frequencyKey stringByAppendingString:@".start"];
        self.m_showCountKey = [frequencyKey stringByAppendingString:@".count"];
    }

    return self;
}

- (NSInteger)getShowLimit {
    return self.m_showLimit;
}

- (NSInteger)getShowTimeout {
    return self.m_showTimeout;
}

- (BOOL)hasShowQuota {
    if (self.m_showLimit < 0) {
        return YES;
    }

    NSInteger start = [AppleUserDefaults getIntegerForKey:self.m_showStartKey defaultValue:0];
    NSInteger count = [AppleUserDefaults getIntegerForKey:self.m_showCountKey defaultValue:0];
    NSInteger now = [AppleDetail getTimestamp];

    if (count == 0 || now - start >= self.m_showTimeout) {
        count = 0;
    }

    return count < self.m_showLimit;
}

- (void)recordShown {
    NSInteger start = [AppleUserDefaults getIntegerForKey:self.m_showStartKey defaultValue:0];
    NSInteger count = [AppleUserDefaults getIntegerForKey:self.m_showCountKey defaultValue:0];
    NSInteger now = [AppleDetail getTimestamp];

    if (count == 0 || now - start >= self.m_showTimeout) {
        start = now;
        count = 0;
    }

    NSInteger showCount = count + 1;

    [AppleUserDefaults setIntegerForKey:self.m_showStartKey value:start];
    [AppleUserDefaults setIntegerForKey:self.m_showCountKey value:showCount];
}

- (BOOL)canOfferAd {
    if ([self isEnabled] == NO) {
        return NO;
    }

    BOOL hasQuota = [self hasShowQuota];

    return hasQuota;
}

- (BOOL)canYouShowAd {
    if ([self isEnabled] == NO) {
        return NO;
    }

    if ([self hasShowQuota] == NO) {
        return NO;
    }

    [self.m_attempts incrementAttempts];

    return YES;
}

@end
