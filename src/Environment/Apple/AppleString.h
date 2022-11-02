#pragma once

#include "Kernel/String.h"
#include "Kernel/ConstString.h"

#import <Foundation/Foundation.h>
 
namespace Mengine
{
    namespace Helper
    {
        NSString * unicodeToNSString( const WString & _value );
        NSString * stringToNSString( const String & _value );
        NSString * stringToNSString( const ConstString & _value );
    }
}
