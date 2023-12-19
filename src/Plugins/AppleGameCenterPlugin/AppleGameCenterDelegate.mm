#import "AppleGameCenterDelegate.h"

#include "Kernel/PlatformHelper.h"

@implementation AppleGameCenterDelegate

#pragma mark -

- (void) gameCenterViewControllerDidFinish:(GKGameCenterViewController *)gameCenterViewController {
    [gameCenterViewController dismissViewControllerAnimated:YES completion:^{}];
}

#pragma mark -

- (BOOL) login:(void(^)(NSError* _Nullable))handler {
    GKLocalPlayer * localPlayer = [GKLocalPlayer localPlayer];
    
    [localPlayer setAuthenticateHandler:^(UIViewController* v, NSError * error) {
        if( error != nil )
        {
            self.m_authenticateSuccess = false;
            
            Mengine::Helper::dispatchMainThreadEvent([handler, error]() {
                handler( error );
            });
            
            return;
        }
        
        self.m_authenticateSuccess = true;
        
        Mengine::Helper::dispatchMainThreadEvent([handler]() {
            handler( nil );
        });
    }];
    
    return YES;
}

- (BOOL) loadCompletedAchievements:(void(^)(NSError * _Nullable, NSArray * _Nullable))handler {
    if( self.m_authenticateSuccess == false )
    {
        return NO;
    }
    
    [GKAchievement loadAchievementsWithCompletionHandler:^(NSArray<GKAchievement *> * _Nullable achievements, NSError * _Nullable error) {
        if( error != nil )
        {
            Mengine::Helper::dispatchMainThreadEvent([handler, error]() {
                handler( error, nil );
            });
            
            return;
        }
        
        if( achievements != nil )
        {
            Mengine::Helper::dispatchMainThreadEvent([handler]() {
                handler( nil, nil );
            });
            
            return;
        }
        
        NSMutableArray * cmpAch = [[NSMutableArray alloc] init];
        
        for( GKAchievement * ach in achievements )
        {
            if( [ach isCompleted] == TRUE )
            {
                [cmpAch addObject:[ach identifier]];
            }
        }
        
        Mengine::Helper::dispatchMainThreadEvent([handler, cmpAch]() {
            handler( nil, cmpAch );
        });
    }];
    
    return YES;
}

- (BOOL) resetAchievements:(void(^ _Nonnull)(NSError * __nullable error))handler {
    if( self.m_authenticateSuccess == false )
    {
        return NO;
    }
    
    [GKAchievement resetAchievementsWithCompletionHandler:^(NSError * _Nullable error) {
        Mengine::Helper::dispatchMainThreadEvent([handler, error]() {
            handler( error );
        });
    }];
    
    return YES;
}

- (BOOL) reportScore:(NSString*)identifier score:(int64_t)score response:(void(^)(NSError * _Nullable))handler {
	if( self.m_authenticateSuccess == false )
    {
        return NO;
    }
    
	GKScore * scoreReporter = [[GKScore alloc] initWithLeaderboardIdentifier:identifier];
	scoreReporter.value = score;
    scoreReporter.context = 0;
    
    NSArray * scores = @[scoreReporter];
    [GKScore reportScores:scores withCompletionHandler:^(NSError *error) {
        Mengine::Helper::dispatchMainThreadEvent([handler, error]() {
            handler(error);
        });
	}];
    
    return YES;
}

- (BOOL) reportAchievementIdentifier:(NSString*)identifier percentComplete:(double)percent withBanner:(BOOL)banner response:(void(^)(NSError * _Nullable))handler {
    if( self.m_authenticateSuccess == false )
    {
        return NO;
    }
    
    GKAchievement * achievement = [[GKAchievement alloc] initWithIdentifier:identifier];
    [achievement setShowsCompletionBanner:banner];
    [achievement setPercentComplete: percent];
    
    [GKAchievement reportAchievements:@[achievement] withCompletionHandler:^(NSError * _Nullable error) {
        Mengine::Helper::dispatchMainThreadEvent([handler, error]() {
            handler(error);
        });
    }];
    
    return YES;
}

@end
