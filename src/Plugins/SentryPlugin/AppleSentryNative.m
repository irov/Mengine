#import "AppleSentry.h"

@import Sentry;

int AppleSentryInitialize( const char * _dsn, int _debug )
{
    [SentrySDK startWithConfigureOptions:^(SentryOptions *options) {
        options.dsn = @(_dsn);
        options.debug = _debug; // Enabled debug when first installing is always helpful
    }];

    return 0;
}
