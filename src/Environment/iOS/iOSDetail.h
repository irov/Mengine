#pragma once

#include "Config/Config.h"

#import "Environment/Apple/AppleEvent.h"
#import "Environment/iOS/iOSAdRevenueParam.h"
#import "Environment/iOS/iOSTransparencyConsentParam.h"
#import "Environment/iOS/iOSLogRecordParam.h"
#import "Environment/iOS/iOSUIMainApplicationDelegateInterface.h"

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

@interface iOSDetail : NSObject

+ (NSString *)getDeviceId;
+ (NSString *)getDeviceName;
+ (NSString *)getIDFA;
+ (BOOL)isValidIDFA;

+ (UIWindow *)getRootWindow;
+ (UIView *)getRootView;
+ (UIViewController *)getRootViewController;
+ (NSUUID *)getAdIdentifier;

+ (void)addMainQueueOperation:(dispatch_block_t)block;

+ (NSObject<iOSUIMainApplicationDelegateInterface> *)getUIMainApplicationDelegate;
+ (id)getUIProxyApplicationDelegate:(Class)delegateClass;

+ (void)eventNotify:(AppleEvent *)event args:(NSArray<id> *)args;
+ (void)adRevenue:(iOSAdRevenueParam *)revenue;
+ (void)transparencyConsent:(iOSTransparencyConsentParam *)consent;
+ (void)log:(iOSLogRecordParam *)record;
+ (NSString *)pathForTemporaryFileWithPrefix:(NSString *)prefix ext:(NSString *)ext;

+ (void)alertWithTitle:(NSString *)title message:(NSString *)message callback:(void (^)(void))callback;
+ (void)alertWithViewController:(UIViewController *)viewController title:(NSString *)title message:(NSString *)message callback:(void (^)(void))callback;

@end
