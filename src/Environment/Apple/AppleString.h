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
        size_t NSStringCopyToString( const NSString * _src, Char * const _dst, size_t _capacity );
    
        ConstString NSStringToConstString( const NSString * _value );
        String NSStringToString( const NSString * _value );
    
        typedef StaticString<MENGINE_APPLE_MESSAGE_NSID> NSIDMessage;
        NSIDMessage NSIdToString( id _value );
    
        NSString * unicodeToNSString( const WString & _value );
        NSString * stringToNSString( const String & _value );
        NSString * stringToNSString( const ConstString & _value );
    }
}
