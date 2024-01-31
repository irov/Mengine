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
        void AppleGetParamsNSDictionary( NSDictionary * _dictionary, MapParams * const _map )
        {
            if( _dictionary == nil )
            {
                return;
            }
            
            [_dictionary enumerateKeysAndObjectsUsingBlock:^(id key, id value, BOOL * stop) {
                ConstString key_str = Helper::NSStringToConstString( key );
                const Char * value_str = [[NSString stringWithFormat:@"%@", value] UTF8String];
                
                _map->emplace(std::make_pair(key_str, String(value_str)));
            }];
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
