#include "iOSDetail.h"

#import "UIMainApplicationDelegateInterface.h"
#import "UIPluginApplicationDelegateInterface.h"

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
        id iOSGetUIProxyApplicationDelegate( Class delegateClass )
        {
            NSObject<UIMainApplicationDelegateInterface> * delegate = (NSObject<UIMainApplicationDelegateInterface> *)[[UIApplication sharedApplication] delegate];

            NSArray<UIPluginApplicationDelegateInterface> * pluginDelegates = [delegate getPluginDelegates];

            for( NSObject<UIPluginApplicationDelegateInterface> * delegate : pluginDelegates ) {
                if( [delegate isMemberOfClass:delegateClass] == YES ) {
                    return delegate;
                }
            }

            return nil;
        }
        //////////////////////////////////////////////////////////////////////////
        void iOSPluginApplicationDelegateEventNotify( NSString * name, id firstArg, ... )
        {
            NSObject<UIMainApplicationDelegateInterface> * delegate = (NSObject<UIMainApplicationDelegateInterface> *)[[UIApplication sharedApplication] delegate];
            
            va_list args;
            va_start(args, firstArg);
            
            NSMutableArray * send_args = [[NSMutableArray alloc] init];

            for( NSString *arg = firstArg; arg != nil; arg = va_arg(args, NSString*) ) {
                [send_args addObject:firstArg];
            }

            va_end(args);
            
            [delegate notify:name arrayArgs:send_args];
        }
        //////////////////////////////////////////////////////////////////////////
        NSString * iOSPathForTemporaryFileWithPrefix( NSString * prefix, NSString * ext )
        {
            NSString *  result;
            CFUUIDRef   uuid;
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
        void iOSAlert( UIViewController * view, NSString * title, NSString * message )
        {
            UIAlertController *alertController = [UIAlertController alertControllerWithTitle:title
                                                                                     message:message
                                                                              preferredStyle:UIAlertControllerStyleAlert];

            UIAlertAction *okAction = [UIAlertAction actionWithTitle:@"OK"
                                                               style:UIAlertActionStyleDefault
                                                             handler:nil];
            [alertController addAction:okAction];

            [view presentViewController:alertController animated:YES completion:nil];
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
