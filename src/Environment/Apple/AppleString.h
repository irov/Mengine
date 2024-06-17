#pragma once

#include "Kernel/StaticString.h"
#include "Kernel/String.h"
#include "Kernel/ConstString.h"

#import <Foundation/Foundation.h>
 
#ifndef MENGINE_APPLE_MESSAGE_NSID
#define MENGINE_APPLE_MESSAGE_NSID 1024
#endif

namespace Mengine
{
    namespace Helper
    {
        size_t NSStringCopyToString( NSString * _src, Char * const _dst, size_t _capacity );
    
        ConstString NSStringToConstString( NSString * _value );
        String NSStringToString( NSString * _value );
    
        typedef StaticString<MENGINE_APPLE_MESSAGE_NSID> NSIDMessage;
        NSIDMessage NSIdToString( id _value );
    
        NSString * stringToNSString( const Char * _value, size_t _size );
        NSString * unicodeToNSString( const WString & _value );
        NSString * stringToNSString( const String & _value );
        NSString * stringToNSString( const ConstString & _value );
    }
}
