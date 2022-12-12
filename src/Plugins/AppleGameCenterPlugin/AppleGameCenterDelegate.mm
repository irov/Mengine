#import "AppleGameCenterDelegate.h"

@implementation AppleGameCenterDelegate

@synthesize m_authenticateSuccess;

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
            m_authenticateSuccess = false;
            
            handler( error );
            
            return;
        }
        
        m_authenticateSuccess = true;
        
        handler( nil );
    }];
    
    return true;
}

- (BOOL) loadCompletedAchievements:(void(^)(NSError * _Nullable, NSArray * _Nullable))handler {
    if( m_authenticateSuccess == false )
    {
        return false;
    }
    
    [GKAchievement loadAchievementsWithCompletionHandler:^(NSArray<GKAchievement *> * _Nullable achievements, NSError * _Nullable error) {
        if( error != nil )
        {
            handler( error, nil );
            
            return;
        }
        
        if( achievements != nil )
        {
            handler( nil, nil );
            
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
        
        handler( nil, cmpAch );
    }];
    
    return true;
}

- (BOOL) resetAchievements:(void(^ _Nonnull)(NSError * __nullable error))handler {
    if( m_authenticateSuccess == false )
    {
        return false;
    }
    
    [GKAchievement resetAchievementsWithCompletionHandler:^(NSError * _Nullable error) {
        handler( error );
    }];
    
    return true;
}

- (BOOL) reportScore:(NSString*)identifier score:(int64_t)score response:(void(^)(NSError * _Nullable))handler {
	if( m_authenticateSuccess == false )
    {
        return false;
    }
    
	GKScore * scoreReporter = [[GKScore alloc] initWithLeaderboardIdentifier:identifier];
	scoreReporter.value = score;
    scoreReporter.context = 0;
    
    NSArray * scores = @[scoreReporter];
    [GKScore reportScores:scores withCompletionHandler:^(NSError *error) {
        handler(error);
	}];
    
    return true;
}

- (BOOL) reportAchievementIdentifier:(NSString*)identifier percentComplete:(double)percent withBanner:(BOOL)banner response:(void(^)(NSError * _Nullable))handler {
    if( m_authenticateSuccess == false )
    {
        return false;
    }
    
    GKAchievement * achievement = [[GKAchievement alloc] initWithIdentifier:identifier];
    [achievement setShowsCompletionBanner:banner];
    [achievement setPercentComplete: percent];
    
    [GKAchievement reportAchievements:@[achievement] withCompletionHandler:^(NSError * _Nullable error) {
        handler(error);
    }];
    
    return true;
}

@end
