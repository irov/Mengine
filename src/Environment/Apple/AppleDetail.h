#pragma once

#include "Kernel/Params.h"

#import <Foundation/Foundation.h>

namespace Mengine
{
    namespace Helper
    {
        NSInteger AppleCurrentTimeMillis();
        NSInteger AppleGetSecureRandomInteger();
        NSString * AppleGetRandomHexString( NSInteger length );
        void AppleGetParamsNSDictionary( NSDictionary * _dictionary, MapParams * const _map );
    }
}
