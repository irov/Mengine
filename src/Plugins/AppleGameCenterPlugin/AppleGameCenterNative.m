#import "AppleGameCenterNative.h"

@implementation AppleGameCenterNative

@synthesize gcAuthenticateSuccess;

#pragma mark -

- (void) gameCenterViewControllerDidFinish:(GKGameCenterViewController *)gameCenterViewController{
    [gameCenterViewController dismissViewControllerAnimated:YES completion:^{}];
}

- (BOOL) authenticateHandler:(NSError*)error
{
    if (!error) {
        return YES;
    }

    // Your application can process the error parameter to report the error to the player.
#ifdef DEBUG
    NSString* errMsg =[NSString stringWithFormat:@"Could not connect with Game Center servers: %@", [error localizedDescription]];
    showAlert(@"Error", errMsg, @"Try Later");
#endif
    
    return NO;
}

#pragma mark -

- (BOOL) login:(void(^)(BOOL))handler;
{
    GKLocalPlayer *localPlayer = [GKLocalPlayer localPlayer];
    
    [localPlayer setAuthenticateHandler:^(UIViewController* v, NSError* error) {
        BOOL result = [self authenticateHandler:error];
        
        if(gcAuthenticateSuccess == result) {
            return;
        }
        
        gcAuthenticateSuccess = result;
        
        handler(gcAuthenticateSuccess);
    }];
    
    return true;
}


- (BOOL) loadCompletedAchievements:(void(^)(NSError * _Nullable, NSArray * _Nullable))handler{
    if (!gcAuthenticateSuccess) {
        return false;
    }
    
    [GKAchievement loadAchievementsWithCompletionHandler:^(NSArray<GKAchievement *> * _Nullable achievements, NSError * _Nullable error) {
        if (error) {
            handler(error, nil);
            return;
        }
        
        if (!achievements){
            handler(nil, nil);
            return;
        }
            
        NSMutableArray *cmpAch = [[NSMutableArray alloc] init];
            
        for (GKAchievement* ach in achievements){
            if ([ach isCompleted]){
                [cmpAch addObject:[ach identifier]];
            }
        }
            
        handler(nil, cmpAch);
    }];
    
    return true;
}

- (BOOL) reportScore:(NSString*)identifier score:(int64_t)score response:(void(^)(NSError * _Nullable))handler{
	if (!gcAuthenticateSuccess) {
        return false;
    }
    	
	GKScore *scoreReporter = [[GKScore alloc] initWithLeaderboardIdentifier:identifier];
	scoreReporter.value = score;
    scoreReporter.context = 0;
    
    NSArray *scores = @[scoreReporter];
    [GKScore reportScores:scores withCompletionHandler:^(NSError *error) {
        handler(error);
	}];
    
    return true;
}

- (BOOL) reportAchievementIdentifier:(NSString*)identifier percentComplete:(double)percent withBanner:(BOOL)banner response:(void(^)(NSError * _Nullable))handler{
    if (!gcAuthenticateSuccess) {
        return false;
    }
        
    GKAchievement *achievement = [[GKAchievement alloc] initWithIdentifier:identifier];
    [achievement setShowsCompletionBanner:banner];
    [achievement setPercentComplete: percent];
    
    [GKAchievement reportAchievements:@[achievement] withCompletionHandler:^(NSError * _Nullable error) {
        handler(error);
    }];
    
    return true;
}

@end
