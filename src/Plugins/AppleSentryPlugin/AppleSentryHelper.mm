#import "AppleSentryHelper.h"

#import <Sentry/Sentry.h>

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void appleSentryErrorCapture( const Char * _msg, int32_t _code )
        {
            NSError *error = [NSError errorWithDomain:@(_msg) code:_code userInfo: nil];
            [SentrySDK captureError:error];
        }
        //////////////////////////////////////////////////////////////////////////
        void appleSentryMessageCapture( const Char * _msg )
        {
            [SentrySDK captureMessage:@(_msg)];
        }
        //////////////////////////////////////////////////////////////////////////
        void appleSentrySetExtraBoolean( const Char * _key, int32_t _value )
        {
            [SentrySDK configureScope:^(SentryScope *_Nonnull scope) {
                [scope setExtraValue:(_value == 0 ? @FALSE : @TRUE) forKey:@(_key)];
            }];
        }
        //////////////////////////////////////////////////////////////////////////
        void appleSentrySetExtraInteger( const Char * _key, int32_t _value )
        {
            [SentrySDK configureScope:^(SentryScope *_Nonnull scope) {
                [scope setExtraValue:@(_value) forKey:@(_key)];
            }];
        }
        //////////////////////////////////////////////////////////////////////////
        void appleSentrySetExtraString( const Char * _key, const Char * _value )
        {
            [SentrySDK configureScope:^(SentryScope *_Nonnull scope) {
                [scope setExtraValue:@(_value) forKey:@(_key)];
            }];
        }
        //////////////////////////////////////////////////////////////////////////
        void appleSentrySetExtraNSString( const Char * _key, NSString * _value )
        {
            [SentrySDK configureScope:^(SentryScope *_Nonnull scope) {
                [scope setExtraValue:_value forKey:@(_key)];
            }];
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
