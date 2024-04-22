#include "AppleString.h"

#include "Interface/AppleEnvironmentServiceInterface.h"

#include "Kernel/ConstStringHelper.h"

#include "Config/StdString.h"

namespace Mengine
{
    namespace Helper
    {
        /////////////////////////////////////////////////////////////////////////
        size_t NSStringCopyToString( NSString * _src, Char * const _dst, size_t _capacity )
        {
            const Char * src_str = [_src UTF8String];
            
            MENGINE_STRNCPY( _dst, src_str, _capacity );
            
            size_t len = MENGINE_STRLEN( _dst );
            
            return len;
        }
        /////////////////////////////////////////////////////////////////////////
        ConstString NSStringToConstString( NSString * _value )
        {
            ConstString value_cstr;
            APPLE_ENVIRONMENT_SERVICE()
                ->stringize( _value, &value_cstr );
            
            return value_cstr;
        }
        /////////////////////////////////////////////////////////////////////////
        String NSStringToString( NSString * _value )
        {
            const Char * value_str = [_value UTF8String];
            
            String value_string = String( value_str );
            
            return value_string;
        }
        /////////////////////////////////////////////////////////////////////////
        NSIDMessage NSIdToString( id _value )
        {
            const Char * message_str = [[NSString stringWithFormat:@"%@", _value] UTF8String];

            return NSIDMessage( message_str );
        }
        /////////////////////////////////////////////////////////////////////////
        NSString * unicodeToNSString( const WString & _value )
        {
            const WChar * value_wstr = _value.c_str();
            WString::size_type value_size = _value.size();
            
            NSData * data_wstr = [NSData dataWithBytes:value_wstr length:(value_size * sizeof(WChar))];
            NSString * ns = [[NSString alloc] initWithData:data_wstr encoding:NSUTF32LittleEndianStringEncoding];
            
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
