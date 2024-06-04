#pragma once

#include "Kernel/Params.h"
#include "Kernel/Data.h"
#include "Kernel/VectorString.h"
#include "Kernel/VectorConstString.h"

#import <Foundation/Foundation.h>

namespace Mengine
{
    namespace Helper
    {
        NSInteger AppleCurrentTimeMillis();
        NSInteger AppleGetSecureRandomInteger();
        NSString * AppleGetRandomHexString( NSInteger length );

        void AppleGetParamsFromNSDictionary( NSDictionary * _in, Params * const _out );
        NSDictionary * AppleGetNSDictionaryFromParams( const Params & _params );
        void AppleGetVectorStringFromNSArray( NSArray<NSString *> * _in, VectorString * const _out );
        NSArray<NSString *> * AppleGetNSArrayFromVectorString( const VectorString & _strings );
        void AppleGetVectorConstStringFromNSArray( NSArray<NSString *> * _in, VectorConstString * const _out );
        NSArray<NSString *> * AppleGetNSArrayFromVectorConstString( const VectorConstString & _strings );    
        void AppleGetDataFromNSData( NSData * _in, Data * const _out );
        NSData * AppleGetNSDataFromData( const Data & _data );
    }
}
