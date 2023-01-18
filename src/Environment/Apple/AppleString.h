#pragma once

#include "Kernel/String.h"
#include "Kernel/ConstString.h"

#import <Foundation/Foundation.h>
 
namespace Mengine
{
    namespace Helper
    {
        size_t NSStringCopyToString( const NSString * _src, Char * const _dst, size_t _capacity );
    
        ConstString NSStringToConstString( const NSString * _value );
        String NSStringToString( const NSString * _value );
    
        NSString * unicodeToNSString( const WString & _value );
        NSString * stringToNSString( const String & _value );
        NSString * stringToNSString( const ConstString & _value );
    }
}
