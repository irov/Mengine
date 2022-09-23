#include "iOSUtils.h"

#import <Foundation/Foundation.h>

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool iOSGetDeviceID( Char * const _deviceId )
        {
            NSString * deviceId = [[[UIDevice currentDevice] identifierForVendor] UUIDString];
    
            if( deviceId == nil )
            {
                return false;
            }
    
            const Char * deviceId_str = [deviceId UTF8String];
    
            MENGINE_STRCPY( _deviceId, deviceId_str );
    
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
