#import <Foundation/Foundation.h>
#import <GameKit/GameKit.h>

@interface AppleGameCenterNative : NSObject <GKGameCenterControllerDelegate>
{
    BOOL gcSuccess;
}

@property (assign) BOOL gcSuccess;

- (void)login:(void(^)(BOOL))handler;
- (void)loadCompletedAchievements:(void(^)(NSArray*))handler;

- (void)reportScore:(int64_t)score forCategory:(NSString*)category;
- (void)reportAchievementIdentifier:(NSString*)identifier percentComplete:(float)percent withBanner:(BOOL)banner;

@end
