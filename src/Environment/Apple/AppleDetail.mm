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
        void AppleGetParamsNSDictionary( NSDictionary * _dictionary, Params * const _params )
        {
            if( _dictionary == nil )
            {
                return;
            }
            
            CFTypeID boolenTypeId = CFBooleanGetTypeID();
            CFTypeID numberTypeId = CFNumberGetTypeID();
            
            [_dictionary enumerateKeysAndObjectsUsingBlock:^(id key, id value, BOOL * stop) {
                ConstString key_str = Helper::NSStringToConstString( key );
                
                if ([value isKindOfClass:[NSNumber class]] == YES) {
                    CFTypeID valueTypeId = CFGetTypeID((__bridge CFTypeRef)(value));
                    
                    if (valueTypeId == boolenTypeId) {
                        bool b = [value boolValue];
                        
                        _params->emplace( key_str, b );
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
                                
                                _params->emplace( key_str, n );
                            }break;
                                
                            case kCFNumberFloat32Type:
                            case kCFNumberFloat64Type:
                            case kCFNumberFloatType:
                            case kCFNumberDoubleType: {
                                double d = [value doubleValue];
                                
                                _params->emplace( key_str, d );
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
                    
                    _params->emplace( key_str, s );
                } else {
                    return;
                }
                
                const Char * value_str = [[NSString stringWithFormat:@"%@", value] UTF8String];
                
                _params->emplace(std::make_pair(key_str, String(value_str)));
            }];
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
