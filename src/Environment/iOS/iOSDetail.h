#pragma once

#include "Config/Config.h"

#import "Environment/Apple/AppleEvent.h"
#import "Environment/Apple/AppleLogRecordParam.h"

#import "Environment/iOS/iOSAdRevenueParam.h"
#import "Environment/iOS/iOSTransparencyConsentParam.h"
#import "Environment/iOS/iOSUIMainApplicationDelegateInterface.h"

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

@interface iOSDetail : NSObject

+(NSString * _Nonnull)getDeviceId;
+(NSString * _Nonnull)getDeviceName;
+(BOOL)isAppTrackingTransparencyAllowed;
+(NSString * _Nonnull)getIDFA;
+(BOOL)isValidIDFA;

+(UIWindow * _Nonnull)getRootWindow;
+(UIView * _Nonnull)getRootView;
+(UIViewController * _Nonnull)getRootViewController;
+(NSUUID * _Nonnull)getAdIdentifier;

+(NSObject<iOSUIMainApplicationDelegateInterface> * _Nonnull)getUIMainApplicationDelegate;

+(id _Nullable)getPluginDelegateOfClass:(Class _Nonnull)delegateClass;
+(id _Nullable)getPluginDelegateOfProtocol:(Protocol * _Nonnull)delegateProtocol;

+(void)eventNotify:(AppleEvent * _Nonnull)event args:(NSArray<id> * _Nonnull)args;
+(void)setSessionId:(iOSSessionIdParam * _Nonnull)sessionId;
+(void)removeSessionData;
+(void)adRevenue:(iOSAdRevenueParam * _Nonnull)revenue;
+(void)transparencyConsent:(iOSTransparencyConsentParam * _Nonnull)consent;
+(void)log:(AppleLogRecordParam * _Nonnull)record;
+(void)config:(NSDictionary * _Nonnull)config;

+(NSString * _Nonnull)pathForTemporaryFileWithPrefix:(NSString * _Nonnull)prefix ext:(NSString * _Nonnull)ext;

+(void)showOkAlertWithTitle:(NSString * _Nonnull)title
                    message:(NSString * _Nonnull)message
                         ok:(void (^ _Nonnull)(void) _Nonnull)ok;

+(void)showOkAlertWithViewController:(UIViewController * _Nonnull)viewController
                               title:(NSString * _Nonnull)title
                             message:(NSString * _Nonnull)message
                                  ok:(void (^ _Nonnull)(void) _Nonnull)ok;

+(void)showAreYouSureAlertDialogWithTitle:(NSString * _Nonnull)title
                                  message:(NSString * _Nonnull)message
                                    delay:(NSTimeInterval)delayMillis
                                      yes:(void (^ _Nonnull)(void) _Nonnull)yes
                                   cancel:(void (^ _Nonnull)(void) _Nonnull)cancel;

+(void)showAreYouSureAlertDialogWithContext:(UIViewController * _Nonnull)viewController
                                      title:(NSString * _Nonnull)title
                                    message:(NSString * _Nonnull)message
                                      delay:(NSTimeInterval)delayMillis
                                        yes:(void (^ _Nonnull)(void) _Nonnull)yes
                                     cancel:(void (^ _Nonnull)(void) _Nonnull)cancel;

@end
