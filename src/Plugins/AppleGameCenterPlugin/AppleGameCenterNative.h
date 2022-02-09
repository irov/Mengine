#import <Foundation/Foundation.h>
#import <GameKit/GameKit.h>

@interface AppleGameCenterNative : NSObject <GKGameCenterControllerDelegate>
{
    BOOL gcAuthenticateSuccess;
}

@property (assign) BOOL gcAuthenticateSuccess;

- (BOOL) login:(void(^)(BOOL))handler;
- (BOOL) loadCompletedAchievements:(void(^)(NSError * _Nullable, NSArray* _Nullable))handler;

- (BOOL) reportScore:(NSString*)identifier score:(int64_t)score response:(void(^)(NSError * _Nullable))handler;
- (BOOL) reportAchievementIdentifier:(NSString*)identifier percentComplete:(float)percent withBanner:(BOOL)banner response:(void(^)(NSError * _Nullable))handler;

@end
