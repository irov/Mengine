#import "AppleStoreReviewPlugin.h"

#import "Environment/iOS/iOSNetwork.h"
#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"

#include "Kernel/Logger.h"
#include "Kernel/ScriptEmbeddingHelper.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "AppleStoreReviewScriptEmbedding.h"
#endif

#import <StoreKit/StoreKit.h>

@implementation AppleStoreReviewPlugin

+ (instancetype) sharedInstance {
    static AppleStoreReviewPlugin * sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[AppleStoreReviewPlugin class]];
    });
    return sharedInstance;
}

- (void)launchTheInAppReview {
    IOS_LOGGER_MESSAGE( @"launch the InAppReview" );
    
    UIWindowScene * foregroundScene = nil;
    
    for (UIWindowScene * scene in UIApplication.sharedApplication.connectedScenes) {
        if (scene.activationState == UISceneActivationStateForegroundActive) {
            foregroundScene = scene;
        }
    }
    
    if (foregroundScene != nil) {
        [SKStoreReviewController requestReviewInScene:foregroundScene];
    }
}

#pragma mark - iOSPluginInterface

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
- (void)onRunBegin {
    Mengine::Helper::addScriptEmbedding<Mengine::AppleStoreReviewScriptEmbedding>( MENGINE_DOCUMENT_FUNCTION );
}

- (void)onStopEnd {
    Mengine::Helper::removeScriptEmbedding<Mengine::AppleStoreReviewScriptEmbedding>();
}
#endif

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    (void)application;
    (void)launchOptions;

    //ToDo
    
    return YES;
}

@end
