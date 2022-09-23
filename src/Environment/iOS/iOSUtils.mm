#include "iOSUtils.h"

#include "Config/StdString.h"

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool iOSGetDeviceId( Char * const _deviceId )
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
        bool iOSGetDeviceName( Char * const _deviceName )
        {
            NSString * deviceName = [[UIDevice currentDevice] name];
    
            if( deviceName == nil )
            {
                return false;
            }
    
            const Char * deviceName_str = [deviceName UTF8String];
    
            MENGINE_STRCPY( _deviceName, deviceName_str );
    
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
