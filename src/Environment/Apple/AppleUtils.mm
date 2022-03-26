#include "AppleUtils.h"

namespace Mengine
{
    namespace Helper
    {
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
    }
}
