#import "AppleSentry.h"

@import Sentry;

int AppleSentryInitialize( const char * _dsn )
{
    [SentrySDK startWithConfigureOptions:^(SentryOptions *options) {
        options.dsn = @(_dsn);
        options.debug = YES; // Enabled debug when first installing is always helpful
    }];

    return 0;
}
