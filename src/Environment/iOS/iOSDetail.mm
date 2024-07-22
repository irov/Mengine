#import "iOSDetail.h"

#import <AdSupport/ASIdentifierManager.h>

@implementation iOSDetail

+ (NSString *)getDeviceId {
    NSString * deviceId = [[[UIDevice currentDevice] identifierForVendor] UUIDString];

    return deviceId;
}

+ (NSString *)getDeviceName {
    NSString * deviceName = [[UIDevice currentDevice] name];

    return deviceName;
}

+ (NSString *)getIDFA {
    NSUUID * idfa_uuid = [iOSDetail getAdIdentifier];

    if( idfa_uuid == nil )
    {
        return nil;
    }

    NSString * idfa = [idfa_uuid UUIDString];

    return idfa;
}
//////////////////////////////////////////////////////////////////////////
+ (BOOL)isValidIDFA {
    NSUUID * idfa_uuid = [iOSDetail getAdIdentifier];

    if( idfa_uuid == nil )
    {
        return NO;
    }

    NSString * idfa = [idfa_uuid UUIDString];

    if ([idfa isEqualToString:@"00000000-0000-0000-0000-000000000000"] == YES)
    {
        return NO;
    }

    return YES;
}

+ (UIWindow *)getRootWindow {
    UIApplication *application = [UIApplication sharedApplication];
    id<UIApplicationDelegate> delegate = application.delegate;
    UIWindow *window = delegate.window;
    
    return window;
}

+ (UIView *)getRootView {
    UIWindow *window = [self getRootWindow];
    UIView *view = [window.subviews objectAtIndex:0];
    
    return view;
}

+ (UIViewController *)getRootViewController {
    UIWindow *window = [self getRootWindow];
    UIViewController *viewController = window.rootViewController;
    
    return viewController;
}

+ (NSUUID *)getAdIdentifier {
    NSUUID *idfa_uuid = [[ASIdentifierManager sharedManager] advertisingIdentifier];
    
    return idfa_uuid;
}

+ (NSObject<iOSUIMainApplicationDelegateInterface> *)getUIMainApplicationDelegate {
    NSObject<iOSUIMainApplicationDelegateInterface> *delegate = (NSObject<iOSUIMainApplicationDelegateInterface> *)[[UIApplication sharedApplication] delegate];
    
    return delegate;
}

+ (id)getUIProxyApplicationDelegate:(Class)delegateClass {
    NSObject<iOSUIMainApplicationDelegateInterface> *delegate = [iOSDetail getUIMainApplicationDelegate];
    NSArray<id<iOSPluginApplicationDelegateInterface>> *pluginDelegates = [delegate getPluginApplicationDelegates];

    for (NSObject<iOSPluginApplicationDelegateInterface> *pluginDelegate in pluginDelegates) {
        if ([pluginDelegate isMemberOfClass:delegateClass]) {
            return pluginDelegate;
        }
    }

    return nil;
}

+ (void)eventNotify:(AppleEvent *)event args:(NSArray<id> *)args {
    __weak NSObject<iOSUIMainApplicationDelegateInterface> *delegate = [self getUIMainApplicationDelegate];
    
    [delegate addMainQueueOperation:^{
        [delegate notify:event arrayArgs:args];
    }];
}

+ (void)adRevenue:(iOSAdRevenueParam *)revenue {
    __weak NSObject<iOSUIMainApplicationDelegateInterface> *delegate = [self getUIMainApplicationDelegate];
    
    [delegate addMainQueueOperation:^{
        [delegate eventAdRevenue:revenue];
    }];
}

+ (void)transparencyConsent:(iOSTransparencyConsentParam *)consent {
    __weak NSObject<iOSUIMainApplicationDelegateInterface> *delegate = [self getUIMainApplicationDelegate];
    
    [delegate addMainQueueOperation:^{
        [delegate eventTransparencyConsent:consent];
    }];
}

+ (void)log:(iOSLogRecordParam *)record {
    __weak NSObject<iOSUIMainApplicationDelegateInterface> *delegate = [self getUIMainApplicationDelegate];
    
    [delegate addMainQueueOperation:^{
        [delegate log:record];
    }];
}

+ (NSString *)pathForTemporaryFileWithPrefix:(NSString *)prefix ext:(NSString *)ext {
    NSString *result;
    CFUUIDRef uuid = CFUUIDCreate(NULL);
    CFStringRef uuidStr = CFUUIDCreateString(NULL, uuid);

    result = [NSTemporaryDirectory() stringByAppendingPathComponent:[NSString stringWithFormat:@"%@%@.%@", prefix, uuidStr, ext]];

    CFRelease(uuidStr);
    CFRelease(uuid);

    return result;
}

+ (void)alertWithTitle:(NSString *)title message:(NSString *)message callback:(void (^)(void))callback {
    UIViewController *rootViewController = [self getRootViewController];
    [self alertWithViewController:rootViewController title:title message:message callback:callback];
}

+ (void)alertWithViewController:(UIViewController *)viewController title:(NSString *)title message:(NSString *)message callback:(void (^)(void))callback {
    if (viewController == nil) {
        return;
    }

    UIAlertController *alertController = [UIAlertController alertControllerWithTitle:title message:message preferredStyle:UIAlertControllerStyleAlert];
    UIAlertAction *okAction = [UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
        if (callback) {
            callback();
        }
    }];

    [alertController addAction:okAction];

    dispatch_async(dispatch_get_main_queue(), ^{
        [viewController presentViewController:alertController animated:YES completion:nil];
    });
}

@end
