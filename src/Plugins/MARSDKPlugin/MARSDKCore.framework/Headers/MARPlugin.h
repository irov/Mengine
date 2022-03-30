//
//  MARPlugin.h
//  MARSDKCore
//
//  Created by js wu on 2021/6/25.
//

#import "MARSDK.h"

@class MARUserExtraData;

// MARPlugin 插件接口
@protocol MARPluginProtocol

-(instancetype) initWithParams:(NSDictionary*)params;

@optional

-(BOOL) isInitCompleted;
-(void) setupWithParams:(NSDictionary*)params;
-(void) submitExtraData:(MARUserExtraData*)data;

// UIApplicationDelegate事件
- (BOOL)application:(UIApplication*)application openURL:(NSURL*)url sourceApplication:(NSString*)sourceApplication annotation:(id)annotation;
- (BOOL)application:(UIApplication *)application openURL:(NSURL*)url options:(NSDictionary *)options;

- (BOOL)application:(UIApplication*)application handleOpenURL:(NSURL *)url;
- (void)application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions;

- (void)applicationWillResignActive:(UIApplication *)application;
- (void)applicationDidEnterBackground:(UIApplication *)application;
- (void)applicationWillEnterForeground:(UIApplication *)application;
- (void)applicationDidBecomeActive:(UIApplication *)application;
- (void)applicationWillTerminate:(UIApplication *)application;

- (void)application:(UIApplication*)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData*)deviceToken;
- (void)application:(UIApplication*)application didFailToRegisterForRemoteNotificationsWithError:(NSError*)error;

- (void)application:(UIApplication*)application didReceiveLocalNotification:(UILocalNotification*)notification;
- (void)application:(UIApplication*)application didReceiveRemoteNotification:(NSDictionary*)userInfo;
- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo fetchCompletionHandler:(void (^)(UIBackgroundFetchResult))completionHandler;

- (UIInterfaceOrientationMask)application:(UIApplication *)application supportedInterfaceOrientationsForWindow:(UIWindow *)window;

- (BOOL)application:(UIApplication *)application continueUserActivity:(NSUserActivity *)userActivity restorationHandler:(void(^)(NSArray<id<UIUserActivityRestoring>> * __nullable restorableObjects))restorationHandler;

@end

@interface MARPlugin : NSObject<MARPluginProtocol>

@property (nonatomic, copy) NSDictionary* params;

-(instancetype) initWithParams:(NSDictionary*)params;

-(BOOL) getBoolForParam:(NSString*)key default:(BOOL)defaultValue;

-(UIView*) view;
-(UIViewController*) viewController;

-(id) getInterface:(Protocol *)aProtocol;

-(void) eventPlatformInit:(NSDictionary*) params;
-(void) eventUserLogin:(NSDictionary*) params;
-(void) eventUserLogout:(NSDictionary*) params;
-(void) eventPayPaid:(NSDictionary*) params;
-(void) eventPaySuccess:(NSDictionary*) params;
-(void) eventRealName:(NSDictionary*) params;
-(void) eventWithCode:(int)code msg:(NSString*)msg;
-(void) eventCustom:(NSString*)name params:(NSDictionary*)params;

@end

