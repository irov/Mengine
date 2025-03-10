#import "iOSDetail.h"

#import "Environment/Apple/AppleLog.h"
#import "Environment/Apple/AppleDetail.h"

#import <AdSupport/ASIdentifierManager.h>
#import <AppTrackingTransparency/AppTrackingTransparency.h>

@implementation iOSDetail

+ (NSString *) getDeviceId {
    NSString * deviceId = [[[UIDevice currentDevice] identifierForVendor] UUIDString];

    return deviceId;
}

+ (NSString *) getDeviceName {
    NSString * deviceName = [[UIDevice currentDevice] name];

    return deviceName;
}

+ (BOOL) isAppTrackingTransparencyAllowed {
    if (@available(iOS 14.5, *)) {
        if (ATTrackingManager.trackingAuthorizationStatus == ATTrackingManagerAuthorizationStatusAuthorized) {
            return YES;
        }
    } else {
        if ([iOSDetail isValidIDFA] == YES) {
            return YES;
        }
    }
    
    return NO;
}

+ (NSString *) getIDFA {
    NSUUID * idfa_uuid = [iOSDetail getAdIdentifier];

    if (idfa_uuid == nil) {
        return nil;
    }

    NSString * idfa = [idfa_uuid UUIDString];

    return idfa;
}
//////////////////////////////////////////////////////////////////////////
+ (BOOL) isValidIDFA {
    NSUUID * idfa_uuid = [iOSDetail getAdIdentifier];

    if (idfa_uuid == nil) {
        return NO;
    }

    NSString * idfa = [idfa_uuid UUIDString];

    if ([idfa isEqualToString:@"00000000-0000-0000-0000-000000000000"] == YES) {
        return NO;
    }

    return YES;
}

+ (UIWindow *) getRootWindow {
    UIApplication * application = [UIApplication sharedApplication];
    id<UIApplicationDelegate> delegate = application.delegate;
    UIWindow * window = delegate.window;
    
    return window;
}

+ (UIView *) getRootView {
    UIWindow * window = [iOSDetail getRootWindow];
    UIView * view = [window.subviews objectAtIndex:0];
    
    return view;
}

+ (UIViewController *) getRootViewController {
    UIWindow * window = [iOSDetail getRootWindow];
    UIViewController *viewController = window.rootViewController;
    
    return viewController;
}

+ (NSUUID *) getAdIdentifier {
    NSUUID * idfa_uuid = [[ASIdentifierManager sharedManager] advertisingIdentifier];
    
    return idfa_uuid;
}

+ (NSObject<iOSUIMainApplicationDelegateInterface> *) getUIMainApplicationDelegate {
    NSObject<iOSUIMainApplicationDelegateInterface> *delegate = (NSObject<iOSUIMainApplicationDelegateInterface> *)[[UIApplication sharedApplication] delegate];
    
    return delegate;
}

+ (id _Nullable) getPluginDelegateOfClass:(Class)delegateClass {
    NSObject<iOSUIMainApplicationDelegateInterface> * mainDelegate = [iOSDetail getUIMainApplicationDelegate];
    id pluginDelegate = [mainDelegate getPluginDelegateOfClass:delegateClass];

    return pluginDelegate;
}

+ (id _Nullable) getPluginDelegateOfProtocol:(Protocol *)protocol {
    NSObject<iOSUIMainApplicationDelegateInterface> * mainDelegate = [iOSDetail getUIMainApplicationDelegate];
    id pluginDelegate = [mainDelegate getPluginDelegateOfProtocol:protocol];
    
    return pluginDelegate;
}

+ (void) eventNotify:(AppleEvent *)event args:(NSArray<id> *)args {
    [AppleDetail addMainQueueOperation:^{
        NSObject<iOSUIMainApplicationDelegateInterface> *delegate = [iOSDetail getUIMainApplicationDelegate];
        
        [delegate notify:event arrayArgs:args];
    }];
}

+ (void) setSessionId:(iOSSessionIdParam *)sessionId {
    [AppleDetail addMainQueueOperation:^{
        NSObject<iOSUIMainApplicationDelegateInterface> *delegate = [iOSDetail getUIMainApplicationDelegate];
    
        [delegate setSessionId:sessionId];
    }];
}

+ (void) removeSessionData {
    [AppleDetail addMainQueueOperation:^{
        NSObject<iOSUIMainApplicationDelegateInterface> *delegate = [iOSDetail getUIMainApplicationDelegate];
    
        [delegate removeSessionData];
    }];
}

+ (void) adRevenue:(iOSAdRevenueParam *)revenue {
    [AppleDetail addMainQueueOperation:^{
        NSObject<iOSUIMainApplicationDelegateInterface> *delegate = [iOSDetail getUIMainApplicationDelegate];
    
        [delegate eventAdRevenue:revenue];
    }];
}

+ (void) transparencyConsent:(iOSTransparencyConsentParam *)consent {
    [AppleDetail addMainQueueOperation:^{
        NSObject<iOSUIMainApplicationDelegateInterface> * delegate = [iOSDetail getUIMainApplicationDelegate];
    
        [delegate eventTransparencyConsent:consent];
    }];
}

+ (void) log:(AppleLogRecordParam *)record {
    [AppleLog withRecord:record];
    
    [AppleDetail addMainQueueOperation:^{
        NSObject<iOSUIMainApplicationDelegateInterface> *delegate = [iOSDetail getUIMainApplicationDelegate];
        
        [delegate log:record];
    }];
}

+ (void) config:(NSDictionary * _Nonnull)config {
    [AppleDetail addMainQueueOperation:^{
        NSObject<iOSUIMainApplicationDelegateInterface> *delegate = [iOSDetail getUIMainApplicationDelegate];
        
        [delegate config:config];
    }];
}

+ (NSString *) pathForTemporaryFileWithPrefix:(NSString *)prefix ext:(NSString *)ext {
    NSString * result;
    CFUUIDRef uuid = CFUUIDCreate(NULL);
    CFStringRef uuidStr = CFUUIDCreateString(NULL, uuid);

    result = [NSTemporaryDirectory() stringByAppendingPathComponent:[NSString stringWithFormat:@"%@%@.%@", prefix, uuidStr, ext]];

    CFRelease(uuidStr);
    CFRelease(uuid);

    return result;
}

+ (void) showOkAlertWithTitle:(NSString *)title message:(NSString *)message ok:(void (^)(void) _Nonnull)ok {
    UIViewController * viewController = [iOSDetail getRootViewController];
    
    [iOSDetail showOkAlertWithViewController:viewController
                                 title:title
                               message:message
                                    ok:ok];
}

+ (void) showOkAlertWithViewController:(UIViewController *)viewController
                          title:(NSString *)title
                        message:(NSString *)message
                             ok:(void (^)(void) _Nonnull)ok {
    if (viewController == nil) {
        return;
    }

    UIAlertController * alertController = [UIAlertController alertControllerWithTitle:title message:message preferredStyle:UIAlertControllerStyleAlert];
    UIAlertAction * okAction = [UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
        [AppleDetail dispatchMainQueue:^{
            ok();
        }];
    }];

    [alertController addAction:okAction];

    dispatch_async(dispatch_get_main_queue(), ^{
        [viewController presentViewController:alertController animated:YES completion:nil];
    });
}

+ (void) showAreYouSureAlertDialogWithTitle:(NSString *)title
                                   message:(NSString *)message
                                     delay:(NSTimeInterval)delayMillis
                                       yes:(void (^)(void) _Nonnull)yes
                                    cancel:(void (^)(void) _Nonnull)cancel {
    UIViewController * viewController = [iOSDetail getRootViewController];
    
    [iOSDetail showAreYouSureAlertDialogWithContext:viewController
                                              title:title
                                            message:message
                                              delay:delayMillis
                                                yes:yes
                                             cancel:cancel];
}

+ (void) showAreYouSureAlertDialogWithContext:(UIViewController *)viewController
                                       title:(NSString *)title
                                     message:(NSString *)message
                                        delay:(NSTimeInterval)delayMillis
                                          yes:(void (^)(void) _Nonnull)yes
                                      cancel:(void (^)(void) _Nonnull)cancel {
    NSString * areYouSureText = @"\n\nAre you sure?";
    NSString * fullMessage = [message stringByAppendingString:areYouSureText];
    
    UIAlertController * alert = [UIAlertController alertControllerWithTitle:title
                                                                   message:fullMessage
                                                            preferredStyle:UIAlertControllerStyleAlert];
    
    UIAlertAction * yesAction = [UIAlertAction actionWithTitle:@"YES"
                                                        style:UIAlertActionStyleDefault
                                                      handler:^(UIAlertAction * _Nonnull action) {
        [AppleDetail dispatchMainQueue:^{
            yes();
        }];
    }];
    
    UIAlertAction * cancelAction = [UIAlertAction actionWithTitle:@"CANCEL"
                                                           style:UIAlertActionStyleCancel
                                                         handler:^(UIAlertAction * _Nonnull action) {
        [AppleDetail dispatchMainQueue:^{
            cancel();
        }];
    }];
    
    [alert addAction:cancelAction];
    [alert addAction:yesAction];
    
    if (delayMillis > 0) {
        yesAction.enabled = NO;
        yesAction.accessibilityHint = @"Disabled temporarily";

        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(delayMillis * NSEC_PER_MSEC)), dispatch_get_main_queue(), ^{
            yesAction.enabled = YES;
            yesAction.accessibilityHint = nil;
        });
    }
    
    dispatch_async(dispatch_get_main_queue(), ^{
        [viewController presentViewController:alert animated:YES completion:nil];
    });
}

@end
