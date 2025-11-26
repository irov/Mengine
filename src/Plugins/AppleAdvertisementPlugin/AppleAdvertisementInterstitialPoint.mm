#import "AppleAdvertisementInterstitialPoint.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/iOS/iOSApplication.h"

@implementation AppleAdvertisementInterstitialPoint

- (instancetype)initWithName:(NSString *)name withJson:(NSDictionary *)json {
    self = [super initWithName:name withJson:json];
    
    if (self != nil) {
        self.m_actionOffset = [self parseAdPointInteger:json key:@"trigger_action_offset" required:NO defaultValue:-1];
        self.m_actionCooldown = [self parseAdPointInteger:json key:@"trigger_action_cooldown" required:NO defaultValue:-1];
        self.m_timeOffset = [self parseAdPointTimeInterval:json key:@"trigger_time_offset" required:NO defaultValue:600];
        self.m_timeCooldown = [self parseAdPointTimeInterval:json key:@"trigger_time_cooldown" required:NO defaultValue:-1];
        self.m_installTimeOffset = [self parseAdPointTimeInterval:json key:@"trigger_install_time_offset" required:NO defaultValue:600];
        self.m_sessionOffset = [self parseAdPointInteger:json key:@"trigger_session_offset" required:NO defaultValue:-1];
    }

    return self;
}

- (BOOL)canYouShowAd {
    if ([self isEnabled] == NO) {
        return NO;
    }
    
    if (self.m_sessionOffset > 0) {
        NSInteger sessionIndex = [iOSApplication.sharedInstance getSessionIndex];
        
        if (sessionIndex < self.m_sessionOffset) {
            return NO;
        }
    }
    
    NSInteger attempt = [self.m_attempts incrementAttempts];
    
    if (self.m_actionOffset > 0) {
        if (attempt < self.m_actionOffset) {
            return NO;
        }
    }
    
    if (self.m_actionCooldown > 0) {
        if (self.m_actionOffset >= 0) {
            if ((attempt - self.m_actionOffset) % self.m_actionCooldown != 0) {
                return NO;
            }
        } else {
            if (attempt % self.m_actionCooldown != 0) {
                return NO;
            }
        }
    }
        
    NSInteger currentTime = [AppleDetail getTimestamp];
        
    if (self.m_installTimeOffset > 0) {
        NSInteger installTime = [iOSApplication.sharedInstance getInstallTimestamp];
        
        if (currentTime - installTime < self.m_installTimeOffset) {
            return NO;
        }
    }
    
    if (self.m_timeOffset > 0) {
        NSInteger sessionTime = [iOSApplication.sharedInstance getSessionTimestamp];
        
        if (currentTime - sessionTime < self.m_timeOffset) {
            return NO;
        }
    }
    
    if (self.m_timeCooldown > 0) {
        NSInteger cooldownTimestamp = [self.m_cooldown getLastShownTimestamp];
        
        if (currentTime - cooldownTimestamp < self.m_timeCooldown) {
            return NO;
        }
    }
    
    return YES;
}

@end
