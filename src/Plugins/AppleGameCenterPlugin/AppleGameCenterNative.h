#import <Foundation/Foundation.h>
#import <GameKit/GameKit.h>

@interface GameCenterManager: NSObject <GKGameCenterControllerDelegate>
{
    BOOL gcSuccess;
}

@property (assign) BOOL gcSuccess;

- (void)login:(void(^)(BOOL))handler;
- (void)loadCompletedAchievements:(void(^)(NSArray*))handler;

- (void)showLeaderboards:(NSString*)leaderboard fromViewController:(UIViewController *)viewController;
- (void)showAchievements:(UIViewController *)viewController;

- (void)reportScore:(int64_t)score forCategory:(NSString*)category;
- (void)reportAchievementIdentifier:(NSString*)identifier percentComplete:(float)percent withBanner:(BOOL)banner;

@end
