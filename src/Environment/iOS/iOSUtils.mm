#include "iOSUtils.h"

#include "Config/StdString.h"

#include "iOSDetail.h"

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
        bool iOSGetIDFA( Char * const _idfa )
        {
            NSUUID * idfa_uuid = Helper::iOSGetAdIdentifier();

            if( idfa_uuid == nil )
            {
                return false;
            }

            NSString * idfa = [idfa_uuid UUIDString];

            const Char * idfa_str = [idfa UTF8String];

            MENGINE_STRCPY( _idfa, idfa_str );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool iOSIsValidIDFA()
        {
            NSUUID * idfa_uuid = Helper::iOSGetAdIdentifier();

            if( idfa_uuid == nil )
            {
                return false;
            }

            NSString * idfa = [idfa_uuid UUIDString];

            if( [idfa isEqualToString:@"00000000-0000-0000-0000-000000000000"] == true)
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
