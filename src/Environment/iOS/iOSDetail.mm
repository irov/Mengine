#include "iOSDetail.h"

#import <AdSupport/ASIdentifierManager.h>

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        UIViewController * iOSGetRootViewController()
        {
            UIWindow * window = [UIApplication sharedApplication].delegate.window;

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
    }
}
