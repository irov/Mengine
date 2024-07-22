#import "AppleString.h"

#include "Interface/AppleKernelServiceInterface.h"

#include "Kernel/ConstStringHelper.h"

#include "Config/StdString.h"

@implementation AppleString

+ (size_t)NSStringCopyToString:(NSString *) _src outString:(Mengine::Char *const _Nonnull) _dst withCapacity:(size_t) _capacity {
    const Mengine::Char * src_str = [_src UTF8String];
            
    MENGINE_STRNCPY( _dst, src_str, _capacity );
            
    size_t len = MENGINE_STRLEN( _dst );
            
    return len;
}
        
+ (Mengine::ConstString)NSStringToConstString:(NSString *) _value {
    Mengine::ConstString value_cstr;
    APPLE_KERNEL_SERVICE()
        ->stringize( _value, &value_cstr );
            
    return value_cstr;
}

+ (Mengine::String)NSStringToString:(NSString *) _value {
    const Mengine::Char * value_str = [_value UTF8String];
            
    Mengine::String value_string = Mengine::String( value_str );
            
    return value_string;
}

+ (NSString * _Nonnull)NSStringFromUnicode:(const Mengine::WString &) _value {
    const Mengine::WChar * value_wstr = _value.c_str();
    Mengine::WString::size_type value_size = _value.size();
            
    NSData * data_wstr = [NSData dataWithBytes:value_wstr length:(value_size * sizeof(Mengine::WChar))];
    NSString * ns = [[NSString alloc] initWithData:data_wstr encoding:NSUTF32LittleEndianStringEncoding];
            
    return ns;
}
        
+ (NSString *)NSStringFromString:(const Mengine::Char *) _value withSize:(size_t) _size {
    NSData * data_wstr = [NSData dataWithBytes:_value length:(_size * sizeof(Mengine::Char))];
    NSString * ns = [[NSString alloc] initWithData:data_wstr encoding:NSUTF8StringEncoding];
            
    return ns;
}

+ (NSString *)NSStringFromString:(const Mengine::String &) _value {
    const Mengine::Char * value_str = _value.c_str();
    Mengine::String::size_type value_size = _value.size();
            
    NSString * ns = [AppleString NSStringFromString:value_str withSize:value_size];
            
    return ns;
}
        
+ (NSString *)NSStringFromConstString:(const Mengine::ConstString &) _value {
    const Mengine::Char * value_str = _value.c_str();
    Mengine::String::size_type value_size = _value.size();
            
    NSString * ns = [AppleString NSStringFromString:value_str withSize:value_size];
            
    return ns;
}

@end
