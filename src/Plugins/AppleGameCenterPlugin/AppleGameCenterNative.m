#import "AppleGameCenterNative.h"

@implementation AppleGameCenterNative

@synthesize gcSuccess;

#pragma mark -

- (void)gameCenterViewControllerDidFinish:(GKGameCenterViewController *)gameCenterViewController{
    [gameCenterViewController dismissViewControllerAnimated:YES completion:^{}];
}

- (void) authenticateHandler:(NSError*)error
{
    if (error == nil) {
        // Insert code here to handle a successful authentication.
        gcSuccess = YES;
    }
    else
    {
        // Your application can process the error parameter to report the error to the player.
#ifdef DEBUG
        NSString* errMsg =[NSString stringWithFormat:@"Could not connect with Game Center servers: %@", [error localizedDescription]];
        showAlert(@"Error", errMsg, @"Try Later");
//        NSLog(errMsg);
#endif
        gcSuccess = NO;
    }
}

#pragma mark -

- (void)login:(void(^)(BOOL))handler;
{
    gcSuccess = NO;
    GKLocalPlayer *localPlayer = [GKLocalPlayer localPlayer];
    
    [localPlayer setAuthenticateHandler:^(UIViewController* v, NSError* error) {
        [self authenticateHandler:error];
        
        handler(error == nil);
    }];
}


- (void) loadCompletedAchievements:(void(^)(NSArray * _Nullable))handler{
    if (!gcSuccess) {
        handler(nil);
        return;
    }
    
    [GKAchievement loadAchievementsWithCompletionHandler:^(NSArray<GKAchievement *> * _Nullable achievements, NSError * _Nullable error) {
        
        if (achievements){
            NSMutableArray *cmpAch = [[NSMutableArray alloc] init];
            
            for (GKAchievement* ach in achievements){
                if ([ach isCompleted]){
                    [cmpAch addObject:[ach identifier]];
                }
            }
            handler(cmpAch);
        }
        
    }];
}

- (void)reportScore:(int64_t)score forCategory:(NSString*)category {
	if (!gcSuccess) return;
	
	GKScore *scoreReporter = [[GKScore alloc] initWithLeaderboardIdentifier:category];
	scoreReporter.value = score;
    
	[scoreReporter reportScoreWithCompletionHandler:^(NSError *error) {
		if (error != nil) {
            showAlert(@"Error",@"Could not submit score with Game Center.",@"Try Later");
		}
	}];
}

void showAlert(NSString* title, NSString* message, NSString*cancelButtonTitle){
    
    UIAlertController *alert = [UIAlertController alertControllerWithTitle:title message:message preferredStyle:UIAlertControllerStyleAlert];
    
    UIAlertAction* defaultAction = [UIAlertAction actionWithTitle:cancelButtonTitle style:UIAlertActionStyleDefault
                                   handler:^(UIAlertAction * action) {}];

    [alert addAction:defaultAction];
    
    [[AppDelegate viewController] presentViewController:alert animated:YES completion:nil];
}
    
- (void)reportAchievementIdentifier:(NSString*)identifier percentComplete:(float)percent withBanner:(BOOL)banner {
    if (!gcSuccess) return;
    
    GKAchievement *achievement = [[GKAchievement alloc] initWithIdentifier: identifier] ;
    if (achievement) {
#ifdef DEBUG
        NSLog(@"report achievement %@ - %2.2f",identifier,percent);
#endif
        [achievement setShowsCompletionBanner:banner];
        achievement.percentComplete = percent;
        
        [GKAchievement reportAchievements:@[achievement]
                    withCompletionHandler:^(NSError * _Nullable error) {
            if (error != nil) {
                showAlert(@"Error",@"Could not submit achievement with Game Center.",@"Try Later");
                
            }
        }];
    }
}

@end
