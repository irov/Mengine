#import "AppleAdvertisementRewardedPoint.h"

@implementation AppleAdvertisementRewardedPoint

- (instancetype)initWithName:(NSString *)name withJson:(NSDictionary *)json {
    self = [super initWithName:name withJson:json];
    
    if (self != nil) {
        //ToDo
    }

    return self;
}

- (BOOL)canOfferAd {
    if ([self isEnabled] == NO) {
        return NO;
    }
    
    return YES;
}

- (BOOL)canYouShowAd {
    if ([self isEnabled] == NO) {
        return NO;
    }
    
    [self.m_attempts incrementAttempts];
    
    return YES;
}

@end
