#include "AppleString.h"
 
namespace Mengine
{
    namespace Helper
    {
        /////////////////////////////////////////////////////////////////////////
        NSString * unicodeToNSString( const WString & _value )
        {
            const WChar * value_str = _value.c_str();
            WString::size_type value_size = _value.size();
            
            NSString * ns = [[NSString alloc]
                             initWithBytes:value_str
                             length:value_size
                             encoding:CFStringConvertEncodingToNSStringEncoding (kCFStringEncodingUTF32LE)
            ];
            
            return ns;
        }
        /////////////////////////////////////////////////////////////////////////
        NSString * stringToNSString( const String & _value )
        {
            const Char * value_str = _value.c_str();
            
            NSString * ns = [NSString stringWithUTF8String:value_str];
            
            return ns;
        }
        /////////////////////////////////////////////////////////////////////////
        NSString * stringToNSString( const ConstString & _value )
        {
            const Char * value_str = _value.c_str();
            
            NSString * ns = [NSString stringWithUTF8String:value_str];
            
            return ns;
        }
        /////////////////////////////////////////////////////////////////////////
    }
}
