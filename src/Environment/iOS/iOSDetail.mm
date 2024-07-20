#include "iOSDetail.h"

#import "iOSPluginApplicationDelegateInterface.h"

#import <AdSupport/ASIdentifierManager.h>

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        UIWindow * iOSGetRootWindow()
        {
            UIApplication * application = [UIApplication sharedApplication];
            id<UIApplicationDelegate> delegate = application.delegate;
            UIWindow * window = delegate.window;
            
            return window;
        }
        //////////////////////////////////////////////////////////////////////////
        UIView * iOSGetRootView()
        {
            UIWindow * window = Helper::iOSGetRootWindow();
            
            UIView * view = [window.subviews objectAtIndex:0];
            
            return view;
        }
        //////////////////////////////////////////////////////////////////////////
        UIViewController * iOSGetRootViewController()
        {
            UIWindow * window = Helper::iOSGetRootWindow();

            UIViewController * viewController = window.rootViewController;
            
            return viewController;
        }
        //////////////////////////////////////////////////////////////////////////
        NSUUID * iOSGetAdIdentifier()
        {
            NSUUID * idfa_uuid = [[ASIdentifierManager sharedManager] advertisingIdentifier];
            
            return idfa_uuid;
        }
        //////////////////////////////////////////////////////////////////////////
        NSObject<iOSUIMainApplicationDelegateInterface> * iOSGetUIMainApplicationDelegate()
        {
            NSObject<iOSUIMainApplicationDelegateInterface> * delegate = (NSObject<iOSUIMainApplicationDelegateInterface> *)[[UIApplication sharedApplication] delegate];
            
            return delegate;
        }
        //////////////////////////////////////////////////////////////////////////
        id iOSGetUIProxyApplicationDelegate( Class delegateClass )
        {
            NSObject<iOSUIMainApplicationDelegateInterface> * delegate = Helper::iOSGetUIMainApplicationDelegate();

            NSArray<iOSPluginApplicationDelegateInterface> * pluginDelegates = [delegate getPluginApplicationDelegates];

            for( NSObject<iOSPluginApplicationDelegateInterface> * delegate : pluginDelegates ) {
                if( [delegate isMemberOfClass:delegateClass] == YES ) {
                    return delegate;
                }
            }

            return nil;
        }
        //////////////////////////////////////////////////////////////////////////
        void iOSEventNotify( AppleEvent * event, id firstArg, ... )
        {
            va_list args;
            va_start(args, firstArg);
            
            NSMutableArray * send_args = [[NSMutableArray alloc] init];
            
            for( NSString *arg = firstArg; arg != nil; arg = va_arg(args, NSString*) ) {
                [send_args addObject:firstArg];
            }
            
            va_end(args);
            
            __weak NSObject<iOSUIMainApplicationDelegateInterface> * delegate = Helper::iOSGetUIMainApplicationDelegate();
                
            [delegate addMainQueueOperation: ^{
                [delegate notify:event arrayArgs:send_args];
            }];
        }
        //////////////////////////////////////////////////////////////////////////
        void iOSEventNotifyArray( AppleEvent * event, NSArray<id> * args )
        {
            __weak NSObject<iOSUIMainApplicationDelegateInterface> * delegate = Helper::iOSGetUIMainApplicationDelegate();
                
            [delegate addMainQueueOperation: ^{
                [delegate notify:event arrayArgs:args];
            }];
        }
        //////////////////////////////////////////////////////////////////////////
        void iOSAdRevenue( iOSAdRevenueParam * revenue )
        {
            __weak NSObject<iOSUIMainApplicationDelegateInterface> * delegate = Helper::iOSGetUIMainApplicationDelegate();
    
            [delegate addMainQueueOperation: ^{
                [delegate eventAdRevenue:revenue];
            }];
        }
        //////////////////////////////////////////////////////////////////////////
        void iOSTransparencyConsent( iOSTransparencyConsentParam * consent )
        {
            __weak NSObject<iOSUIMainApplicationDelegateInterface> * delegate = Helper::iOSGetUIMainApplicationDelegate();
     
            [delegate addMainQueueOperation: ^{
                [delegate eventTransparencyConsent:consent];
            }];
        }
        //////////////////////////////////////////////////////////////////////////
        void iOSLog( iOSLogRecordParam * record )
        {
            __weak NSObject<iOSUIMainApplicationDelegateInterface> * delegate = Helper::iOSGetUIMainApplicationDelegate();
        
            [delegate addMainQueueOperation: ^{
                [delegate log:record];
            }];
        }
        //////////////////////////////////////////////////////////////////////////
        NSString * iOSPathForTemporaryFileWithPrefix( NSString * prefix, NSString * ext )
        {
            NSString * result;
            CFUUIDRef uuid;
            CFStringRef uuidStr;

            uuid = CFUUIDCreate(NULL);
            assert(uuid != NULL);

            uuidStr = CFUUIDCreateString(NULL, uuid);
            assert(uuidStr != NULL);

            result = [NSTemporaryDirectory() stringByAppendingPathComponent:[NSString stringWithFormat:@"%@%@.%@", prefix, uuidStr, ext]];
            assert(result != nil);

            CFRelease(uuidStr);
            CFRelease(uuid);

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        void iOSAlert( NSString * title, NSString * message, void (^_cb)(void) )
        {
            UIViewController * rootViewController = Helper::iOSGetRootViewController();
            
            Helper::iOSAlertWithViewController( rootViewController, title, message, _cb );
        }
        //////////////////////////////////////////////////////////////////////////
        void iOSAlertWithViewController( UIViewController * view, NSString * title, NSString * message, void (^_cb)(void) )
        {
            if( view == nil )
            {
                return;
            }
            
            UIAlertController * alertController = [UIAlertController alertControllerWithTitle:title
                                                                                     message:message
                                                                              preferredStyle:UIAlertControllerStyleAlert];

            UIAlertAction * okAction = [UIAlertAction actionWithTitle:@"OK"
                                                                style:UIAlertActionStyleDefault
                                                              handler:^(UIAlertAction *action) {
                _cb();
            }];

            [alertController addAction:okAction];
            
            dispatch_async(dispatch_get_main_queue(), ^{
                [view presentViewController:alertController animated:YES completion:^{
                    //ToDo
                }];
            });
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
