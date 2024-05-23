#include "AppleFirebaseCrashlyticsService.h"

#include "Environment/Apple/AppleErrorHelper.h"

#include "Kernel/Logger.h"

#include "Config/Algorithm.h"

#import <FirebaseCrashlytics/FirebaseCrashlytics.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AppleFirebaseCrashlyticsService, Mengine::AppleFirebaseCrashlyticsService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleFirebaseCrashlyticsService::AppleFirebaseCrashlyticsService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleFirebaseCrashlyticsService::~AppleFirebaseCrashlyticsService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFirebaseCrashlyticsService::_initializeService()
    {
        //Empty

        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleFirebaseCrashlyticsService::_finalizeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFirebaseCrashlyticsService::sendValue( const ConstString & _value )
    {
        LOGGER_MESSAGE( "send value: %s"
            , _value.c_str() 
        );

        const Char * value_str = _value.c_str();
        
        [[FIRCrashlytics crashlytics] log:[NSString stringWithUTF8String:value_str]];
    }
    //////////////////////////////////////////////////////////////////////
    void AppleFirebaseCrashlyticsService::sendKeyAndValue( const ConstString & _key, const ConstString & _value )
    {
        LOGGER_MESSAGE( "send key: %s value: %s"
            , _key.c_str()
            , _value.c_str() 
        );

        const Char * key_str = _key.c_str();
        const Char * value_str = _value.c_str();
        
        [[FIRCrashlytics crashlytics] setCustomValue:[NSString stringWithUTF8String:value_str]
                                              forKey:[NSString stringWithUTF8String:key_str]];
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFirebaseCrashlyticsService::sendKeyAndValues( const FirebaseCrashlyticsParams & _params )
    {
        LOGGER_MESSAGE( "send key and values:" );

        NSMutableDictionary * keysAndValues = [[NSMutableDictionary alloc] init];

        for( auto && [key, value] : _params )
        {
            LOGGER_MESSAGE("param key: %s value: %s"
                , key.c_str()
                , value.c_str()
            );

            const Char * key_str = key.c_str();
            const Char * value_str = value.c_str();

            [keysAndValues setObject:[NSString stringWithUTF8String:value_str]  forKey:[NSString stringWithUTF8String:key_str]];
        }

        [[FIRCrashlytics crashlytics] setCustomKeysAndValues: keysAndValues];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFirebaseCrashlyticsService::recordError( const ConstString & _name, uint32_t _code, const FirebaseCrashlyticsParams & _params )
    {
        LOGGER_MESSAGE( "record error name: %s code: %u"
            , _name.c_str()
            , _code
        );

        NSMutableDictionary *userInfo = [[NSMutableDictionary alloc] init];

        for( auto && [key, value] : _params )
        {
            LOGGER_MESSAGE("param key: %s value: %s"
                , key.c_str()
                , value.c_str()
            );

            const Char * key_str = key.c_str();
            const Char * value_str = value.c_str();

            [userInfo setObject:[NSString stringWithUTF8String:value_str]  forKey:[NSString stringWithUTF8String:key_str]];
        }

        const Char * name_str = _name.c_str();
        
        NSError *error = [NSError errorWithDomain:[NSString stringWithUTF8String:name_str]
            code:_code
            userInfo:userInfo];

        [[FIRCrashlytics crashlytics] recordError:error];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFirebaseCrashlyticsService::recordError( NSError * _error )
    {
        LOGGER_MESSAGE( "record error: %s"
           , Helper::AppleGetMessageFromNSError( _error ).c_str()
        );

        [[FIRCrashlytics crashlytics] recordError:_error];
    }
    //////////////////////////////////////////////////////////////////////////
}
