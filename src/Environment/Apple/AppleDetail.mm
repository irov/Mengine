#include "AppleDetail.h"
#include "AppleString.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        NSInteger AppleCurrentTimeMillis()
        {
            NSTimeInterval time = [[NSDate date] timeIntervalSince1970];
            
            NSInteger millis = (NSInteger)(time * 1000.0);
            
            return millis;
        }
        //////////////////////////////////////////////////////////////////////////
        NSInteger AppleGetSecureRandomInteger()
        {
            NSInteger randomNumber;
            size_t size = sizeof(randomNumber);
            
            if( SecRandomCopyBytes(kSecRandomDefault, size, (uint8_t *)&randomNumber) != 0 )
            {
                return 0;
            }
            
            return randomNumber;
        }
        //////////////////////////////////////////////////////////////////////////
        NSString * AppleGetRandomHexString( NSInteger length )
        {
            NSInteger lengthBytes = (length + 1) / 2;
            uint8_t randomBytes[lengthBytes];
            if( SecRandomCopyBytes(kSecRandomDefault, lengthBytes, randomBytes) != 0 )
            {
                return @"";
            }
            
            NSMutableString * randomHexString = [NSMutableString stringWithCapacity:(length + 1) / 2 * 2];
            
            for( NSInteger i = 0; i != lengthBytes; ++i )
            {
                [randomHexString appendFormat:@"%02X", randomBytes[i]];
            }
            
            NSString * randomHexStringTrim = [randomHexString substringToIndex:length];
            
            return randomHexStringTrim;
        }
        //////////////////////////////////////////////////////////////////////////
        BOOL AppleIsValidJSONString( NSString * _value )
        {
            if( _value == nil )
            {
                return NO;
            }
            
            NSData *data = [_value dataUsingEncoding:NSUTF8StringEncoding];
            NSError *error = nil;
            
            [NSJSONSerialization JSONObjectWithData:data options:0 error:&error];
            
            if( error != nil )
            {
                return NO;
            }
            
            return YES;
        }
        //////////////////////////////////////////////////////////////////////////
        void AppleGetParamsFromNSDictionary( NSDictionary * _in, Params * const _out )
        {
            if( _in == nil )
            {
                return;
            }
            
            CFTypeID boolenTypeId = CFBooleanGetTypeID();
            CFTypeID numberTypeId = CFNumberGetTypeID();
            
            [_in enumerateKeysAndObjectsUsingBlock:^(id key, id value, BOOL * stop) {
                ConstString key_str = Helper::NSStringToConstString( key );
                
                if ([value isKindOfClass:[NSNumber class]] == YES) {
                    CFTypeID valueTypeId = CFGetTypeID((__bridge CFTypeRef)(value));
                    
                    if (valueTypeId == boolenTypeId) {
                        bool b = [value boolValue];
                        
                        _out->emplace( key_str, b );
                    } else if (valueTypeId == numberTypeId) {
                        CFNumberType numberType = CFNumberGetType((__bridge CFNumberRef)value);
                        
                        switch (numberType) {
                            case kCFNumberSInt8Type:
                            case kCFNumberSInt16Type:
                            case kCFNumberSInt32Type:
                            case kCFNumberSInt64Type:
                            case kCFNumberCharType:
                            case kCFNumberShortType:
                            case kCFNumberIntType:
                            case kCFNumberLongType:
                            case kCFNumberLongLongType: {
                                int64_t n = [value longLongValue];
                                
                                _out->emplace( key_str, n );
                            }break;
                                
                            case kCFNumberFloat32Type:
                            case kCFNumberFloat64Type:
                            case kCFNumberFloatType:
                            case kCFNumberDoubleType: {
                                double d = [value doubleValue];
                                
                                _out->emplace( key_str, d );
                            }break;
                            case kCFNumberCFIndexType:
                            case kCFNumberNSIntegerType:
                            case kCFNumberCGFloatType: {
                                return;
                            }break;
                        }
                    } else {
                        return;
                    }
                } else if ([value isKindOfClass:[NSString class]] == YES) {
                    Mengine::ConstString s = Mengine::Helper::NSStringToConstString(value);
                    
                    _out->emplace( key_str, s );
                } else {
                    return;
                }
                
                const Char * value_str = [[NSString stringWithFormat:@"%@", value] UTF8String];
                
                _out->emplace(std::make_pair(key_str, String(value_str)));
            }];
        }
        //////////////////////////////////////////////////////////////////////////
        NSDictionary * AppleGetNSDictionaryFromParams( const Params & _params )
        {
            NSMutableDictionary * dictionary = [NSMutableDictionary dictionary];
            
            for( auto && [key, value] : _params )
            {
                const Char * key_str = key.c_str();

                Helper::visit( value
                    , [dictionary, key_str]( const ParamBool & _element )
                {
                    [dictionary setObject:[NSNumber numberWithBool:_element]
                                   forKey:@(key_str)];
                }
                    , [dictionary, key_str]( const ParamInteger & _element )
                {
                    [dictionary setObject:[NSNumber numberWithLongLong:_element]
                                   forKey:@(key_str)];
                }
                    , [dictionary, key_str]( const ParamDouble & _element )
                {
                    [dictionary setObject:[NSNumber numberWithDouble:_element]
                                   forKey:@(key_str)];
                }
                    , [dictionary, key_str]( const ParamString & _element )
                {
                    NSString * value_ns = Helper::stringToNSString( _element );

                    [dictionary setObject:value_ns
                                   forKey:@(key_str)];
                }
                    , [dictionary, key_str]( const ParamWString & _element )
                {
                    NSString * value_ns = Helper::unicodeToNSString( _element );

                    [dictionary setObject:value_ns
                                   forKey:@(key_str)];
                }
                    , [dictionary, key_str]( const ParamConstString & _element )
                {
                    NSString * value_ns = Helper::stringToNSString( _element );

                    [dictionary setObject:value_ns
                                   forKey:@(key_str)];
                } );
            }
            
            return dictionary;
        }
        //////////////////////////////////////////////////////////////////////////
        void AppleGetVectorStringFromNSArray( NSArray<NSString *> * _in, VectorString * const _out )
        {
            for( NSString * string in _in )
            {
                String cstr = Helper::NSStringToString( string );
                
                _out->emplace_back( cstr );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        NSArray<NSString *> * AppleGetNSArrayFromVectorString( const VectorString & _strings )
        {
            NSMutableArray<NSString *> * array = [NSMutableArray array];
            
            for( const String & str : _strings )
            {
                NSString * string = Helper::stringToNSString( str );
                
                [array addObject:string];
            }
            
            return array;
        }
        //////////////////////////////////////////////////////////////////////////
        void AppleGetVectorConstStringFromNSArray( NSArray<NSString *> * _in, VectorConstString * const _out )
        {
            for( NSString * string in _in )
            {
                ConstString cstr = Helper::NSStringToConstString( string );
                
                _out->emplace_back( cstr );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        NSArray<NSString *> * AppleGetNSArrayFromVectorConstString( const VectorConstString & _strings )
        {
            NSMutableArray<NSString *> * array = [NSMutableArray array];
            
            for( const ConstString & cstr : _strings )
            {
                NSString * string = Helper::stringToNSString( cstr );
                
                [array addObject:string];
            }
            
            return array;
        }
        //////////////////////////////////////////////////////////////////////////
        void AppleGetDataFromNSData( NSData * _in, Data * const _out )
        {
            const uint8_t * bytes = (const uint8_t *)[_in bytes];
            NSUInteger length = [_in length];

            _out->insert( _out->end(), bytes, bytes + length );
        }
        //////////////////////////////////////////////////////////////////////////
        NSData * AppleGetNSDataFromData( const Data & _data )
        {
            const Data::value_type * bytes = _data.data();
            Data::size_type size = _data.size();
            
            NSData * data = [NSData dataWithBytes:bytes length:size];
            
            return data;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
