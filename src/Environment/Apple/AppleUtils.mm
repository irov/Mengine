#include "AppleUtils.h"

namespace Mengine
{
    namespace Helper
    {
        /////////////////////////////////////////////////////////////////////////
        bool AppleOpenUrlInDefaultBrowser( const Char * _url )
        {
            NSString * url = [NSString stringWithUTF8String: _url];
    
        #if defined(MENGINE_PLATFORM_OSX)
            [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:url]];
        #else
            if( [[UIApplication sharedApplication] canOpenURL: [NSURL URLWithString:url]] == TRUE )
            {
                [[UIApplication sharedApplication] openURL: [NSURL URLWithString:url] options:@{} completionHandler:^(BOOL success)
                {
                    //ToDo callback
                }];
            }
            else
            {
                return false;
            }
        #endif
            return true;
        }
        /////////////////////////////////////////////////////////////////////////
        NSErrorMessage AppleGetMessageFromNSError( NSError * _error )
        {
            NSString * message = [NSString stringWithFormat:@"[Error %ld Description: %@ Failure reason: %@ Recovery suggestion: %@"
                , [_error code]
                , [_error localizedDescription]
                , [_error localizedFailureReason]
                , [_error localizedRecoverySuggestion]
                ];

            const Char * message_str = [message UTF8String];

            return NSErrorMessage( message_str );
        }
        /////////////////////////////////////////////////////////////////////////
        void AppleGetMapNSDictionary( NSDictionary * _dictionary, Map<String, String> * const _map )
        {
            [_dictionary enumerateKeysAndObjectsUsingBlock:^(id key, id value, BOOL *stop) {
                const Char * key_str = [[NSString stringWithFormat:@"%@", key] UTF8String];
                const Char * value_str = [[NSString stringWithFormat:@"%@", value] UTF8String];
                
                _map->emplace(std::make_pair(String(key_str), String(value_str)));
            }];
        }
    }
}
