#pragma once

#include "Kernel/Params.h"
#include "Kernel/VectorConstString.h"

#import <Foundation/Foundation.h>

namespace Mengine
{
    namespace Helper
    {
        NSInteger AppleCurrentTimeMillis();
        NSInteger AppleGetSecureRandomInteger();
        NSString * AppleGetRandomHexString( NSInteger length );

        void AppleGetParamsFromNSDictionary( NSDictionary * _dictionary, Params * const _params );
        NSDictionary * AppleGetNSDictionaryFromParams( const Params & _params );
        void AppleGetVectorConstStringFromNSArray( NSArray<NSString *> * _array, VectorConstString * const _strings );
        NSArray<NSString *> * AppleGetNSArrayFromVectorConstString( const VectorConstString & _strings );
    }
}
