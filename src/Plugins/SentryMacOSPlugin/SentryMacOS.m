#import "SentryMacOS.h"

@import Sentry;

int SentryMacOSInitialize( const char * _dsn )
{
    [SentrySDK startWithConfigureOptions:^(SentryOptions *options) {
        options.dsn = @(_dsn);
        options.debug = YES; // Enabled debug when first installing is always helpful
    }];

    return 0;
}
