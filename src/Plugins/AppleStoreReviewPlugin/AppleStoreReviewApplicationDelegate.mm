#import "AppleStoreReviewApplicationDelegate.h"

#import "Environment/iOS/iOSNetwork.h"
#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"

#include "Kernel/Logger.h"

#import <StoreKit/StoreKit.h>

@implementation AppleStoreReviewApplicationDelegate

+ (instancetype) sharedInstance {
    static AppleStoreReviewApplicationDelegate * sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[AppleStoreReviewApplicationDelegate class]];
    });
    return sharedInstance;
}

- (void)launchTheInAppReview {
    IOS_LOGGER_MESSAGE( @"launch the InAppReview" );
    
    if (@available(iOS 14.0, *)) {
        UIWindowScene * foregroundScene = nil;
        
        for (UIWindowScene * scene in UIApplication.sharedApplication.connectedScenes) {
            if (scene.activationState == UISceneActivationStateForegroundActive) {
                foregroundScene = scene;
            }
        }
        
        if (foregroundScene != nil) {
            [SKStoreReviewController requestReviewInScene:foregroundScene];
        }
    } else if (@available(iOS 10.3, *)) {
        [SKStoreReviewController requestReview];
    }
}

#pragma mark - iOSPluginApplicationDelegateInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    //ToDo
    
    return YES;
}

@end
