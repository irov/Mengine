#pragma once

#include "Config/Char.h"
#include "Config/StdDef.h"

#import <Foundation/Foundation.h>

namespace Mengine
{
    namespace Helper
    {
        NSString * AppleGetUserDefaultsString( NSString * _key, NSString * _default );
        BOOL AppleSetUserDefaultsString( NSString * _key, NSString * _value );
        BOOL AppleGetUserDefaultsBoolean( NSString * _key, BOOL _default );
        BOOL AppleSetUserDefaultsBoolean( NSString * _key, BOOL _value );
        NSInteger AppleGetUserDefaultsInteger( NSString * _key, NSInteger _default );
        BOOL AppleSetUserDefaultsInteger( NSString * _key, NSInteger _value );

        BOOL AppleRemoveUserDefaults( NSString * _key );
    }
}
