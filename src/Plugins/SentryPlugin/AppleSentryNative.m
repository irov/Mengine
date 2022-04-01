#import "AppleSentryNative.h"

@import Sentry;

int appleSentryInitialize( const char * _dsn, int _debug, const char * _releaseName )
{
    [SentrySDK startWithConfigureOptions:^(SentryOptions *options) {
        options.dsn = @(_dsn);
        options.debug = _debug; // Enabled debug when first installing is always helpful
        options.releaseName = @(_releaseName);
        options.attachStacktrace = true;
    }];

    return 0;
}

int appleSentryFinalize()
{
    [SentrySDK close];
}

int appleSentryCapture( const char * _msg, int _code )
{
    NSError *error = [NSError errorWithDomain:@(_msg) code:_code userInfo: nil];
    [SentrySDK captureError:error];
}

int appleSentrySetExtraBoolean( const char * _key, int _value )
{
    [SentrySDK configureScope:^(SentryScope *_Nonnull scope) {
        [scope setExtraValue:(_value == 0 ? @FALSE : @TRUE) forKey:@(_key)];
    }];
    
    return 0;
}

int appleSentrySetExtraInteger( const char * _key, int _value )
{
    [SentrySDK configureScope:^(SentryScope *_Nonnull scope) {
        [scope setExtraValue:@(_value) forKey:@(_key)];
    }];
    
    return 0;
}

int appleSentrySetExtraString( const char * _key, const char * _value )
{
    [SentrySDK configureScope:^(SentryScope *_Nonnull scope) {
        [scope setExtraValue:@(_value) forKey:@(_key)];
    }];
    
    return 0;
}
