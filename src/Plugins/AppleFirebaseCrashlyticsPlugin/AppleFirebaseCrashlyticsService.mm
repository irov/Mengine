#include "AppleFirebaseCrashlyticsService.h"

#include "Kernel/Logger.h"

#include "Config/Algorithm.h"

#import <Firebase/Firebase.h>

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
        @try {
            [FIRApp configure];
        }
        @catch (NSException *exception) {
            LOGGER_ERROR( "%s"
                , [exception.reason UTF8String]
            );
        }
        
#ifdef MENGINE_DEBUG
        [[FIRCrashlytics crashlytics] setCrashlyticsCollectionEnabled:false];
#else
        [[FIRCrashlytics crashlytics] setCrashlyticsCollectionEnabled:true];
#endif

        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleFirebaseCrashlyticsService::_finalizeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFirebaseCrashlyticsService::sendValue( const ConstString & _value )
    {
        LOGGER_INFO( "firebasecrashlytics", "send value: '%s'"
            , _value.c_str() 
        );

        const Char * value_str = _value.c_str();
        
        [[FIRCrashlytics crashlytics] log:[NSString stringWithUTF8String:value_str]];
    }
    //////////////////////////////////////////////////////////////////////
    void AppleFirebaseCrashlyticsService::sendKeyAndValue( const ConstString & _key, const ConstString & _value )
    {
        LOGGER_INFO( "firebasecrashlytics", "send key and value: '%s' = '%s'"
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
        LOGGER_INFO( "firebasecrashlytics", "send key and values" );

        NSMutableDictionary *keysAndValues = [[NSMutableDictionary alloc] init];

        for( auto && [key, value] : _params )
        {
            LOGGER_INFO("firebasecrashlytics", "param [%s : %s]"
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
    void AppleFirebaseCrashlyticsService::recordError(const ConstString& _name, uint32_t _code, const FirebaseCrashlyticsParams& _params)
    {
        LOGGER_INFO( "firebasecrashlytics", "record error '%s' code [%u]"
            , _name.c_str()
            , _code
        );

        NSMutableDictionary *userInfo = [[NSMutableDictionary alloc] init];

        for( auto && [key, value] : _params )
        {
            LOGGER_INFO("firebasecrashlytics", "param [%s : %s]"
                , key.c_str()
                , value.c_str()
            );

            const Char * key_str = key.c_str();
            const Char * value_str = value.c_str();

            [userInfo setObject:[NSString stringWithUTF8String:value_str]  forKey:[NSString stringWithUTF8String:key_str]];
        }

        const Char * name_str = _name.c_str();
        
        NSError *error = [NSError errorWithDomain:[NSString stringWithUTF8String:name_str]
            code: _code
            userInfo:userInfo];

        [[FIRCrashlytics crashlytics] recordError: error];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFirebaseCrashlyticsService::recordError( NSError * _error )
    {
        LOGGER_INFO( "firebasecrashlytics", "record error" );

        [[FIRCrashlytics crashlytics] recordError: _error];
    }
    //////////////////////////////////////////////////////////////////////////
}
