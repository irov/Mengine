#include "AppleUtils.h"

namespace Mengine
{
	NSErrorMessage AppleGetMessageFromNSError( NSError * _error )
	{
		NSString * message = [NSString stringWithFormat:@"[Error %u Description: %@ Failure reason: %@ Recovery suggestion: %@"
			, [error code]
			, [error localizedDescription]
			, [error localizedFailureReason]
			, [error localizedRecoverySuggestion]
			];

		const Char * message_str = [message UTF8String];

		return NSErrorMessage( message_str );
	}
}
