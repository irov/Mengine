#import <Foundation/Foundation.h>
#import <GameKit/GameKit.h>

@interface AppleGameCenterNative : NSObject <GKGameCenterControllerDelegate>
{
    BOOL gcAuthenticateSuccess;
}

@property (assign) BOOL gcAuthenticateSuccess;

- (BOOL) login:(void(^ _Nonnull)(NSError* _Nullable))handler;
- (BOOL) loadCompletedAchievements:(void(^ _Nonnull)(NSError * _Nullable, NSArray* _Nullable))handler;
- (BOOL) resetAchievements:(void(^ _Nonnull)(NSError * __nullable error))handler;

- (BOOL) reportScore:(NSString * _Nonnull)identifier score:(int64_t)score response:(void(^ _Nonnull)(NSError * _Nullable))handler;
- (BOOL) reportAchievementIdentifier:(NSString * _Nonnull)identifier percentComplete:(double)percent withBanner:(BOOL)banner response:(void(^ _Nonnull)(NSError * _Nullable))handler;

@end
