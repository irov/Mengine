#import "iOSAdvertisementRewardedPoint.h"

#import "iOSAdvertisementFrequency.h"

constexpr NSInteger DEFAULT_SHOW_LIMIT = -1;
constexpr NSInteger DEFAULT_SHOW_TIMEOUT = 86400;

@implementation iOSAdvertisementRewardedPoint {
    iOSAdvertisementFrequency * m_frequency;
}

- (instancetype)initWithName:(NSString *)name withJson:(NSDictionary *)json {
    self = [super initWithName:name withJson:json];

    if (self == nil) {
        return nil;
    }

    self.m_showLimit = [self parseAdPointInteger:json key:@"trigger_show_limit" required:NO defaultValue:DEFAULT_SHOW_LIMIT minValue:@(-1) maxValue:@(INT32_MAX)];
    self.m_showTimeout = [self parseAdPointTimeInterval:json key:@"trigger_show_timeout" required:NO defaultValue:DEFAULT_SHOW_TIMEOUT minValue:@1 maxValue:nil];

    return self;
}

- (void)setFrequency:(iOSAdvertisementFrequency *)frequency {
    m_frequency = frequency;
}

- (NSInteger)getShowLimit {
    return self.m_showLimit;
}

- (NSInteger)getShowTimeout {
    return self.m_showTimeout;
}

- (BOOL)hasShowQuota {
    if ([m_frequency hasShowQuota:self.m_showLimit withTimeout:self.m_showTimeout] == NO) {
        return NO;
    }

    return YES;
}

- (void)recordShown {
    [m_frequency recordShownWithTimeout:self.m_showTimeout];
}

- (BOOL)canOfferAd {
    if ([self isEnabled] == NO) {
        return NO;
    }

    if ([self hasShowQuota] == NO) {
        return NO;
    }

    return YES;
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
