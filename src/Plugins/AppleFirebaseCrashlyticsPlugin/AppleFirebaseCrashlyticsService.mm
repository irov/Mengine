#include "AppleFirebaseCrashlyticsService.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/Apple/AppleString.h"

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

        NSString * ns_value = [AppleString NSStringFromConstString:_value];
        
        [[FIRCrashlytics crashlytics] log:ns_value];
    }
    //////////////////////////////////////////////////////////////////////
    void AppleFirebaseCrashlyticsService::sendKeyAndValue( const ConstString & _key, const ConstString & _value )
    {
        LOGGER_MESSAGE( "send key: %s value: %s"
            , _key.c_str()
            , _value.c_str()
        );

        NSString * ns_key = [AppleString NSStringFromConstString:_key];
        NSString * ns_value = [AppleString NSStringFromConstString:_value];
        
        [[FIRCrashlytics crashlytics] setCustomValue:ns_value
                                              forKey:ns_key];
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFirebaseCrashlyticsService::sendKeyAndValues( const Params & _params )
    {
        LOGGER_MESSAGE( "send key and values:" );

        NSMutableDictionary * keysAndValues = [[NSMutableDictionary alloc] init];

        for( auto && [key, value] : _params )
        {
            NSString * ns_key = [AppleString NSStringFromConstString:key];
            
            Helper::visit( value
                , [keysAndValues, ns_key]( const ParamBool & _element )
            {
                [keysAndValues setObject:@(_element) forKey:ns_key];
            }
                , [keysAndValues, ns_key]( const ParamInteger & _element )
            {
                [keysAndValues setObject:@(_element) forKey:ns_key];
            }
                , [keysAndValues, ns_key]( const ParamDouble & _element )
            {
                [keysAndValues setObject:@(_element) forKey:ns_key];
            }
                , [keysAndValues, ns_key]( const ParamString & _element )
            {
                NSString * ns_element = [AppleString NSStringFromString:_element];
                
                [keysAndValues setObject:ns_element forKey:ns_key];
            }
                , [keysAndValues, ns_key]( const ParamWString & _element )
            {
                NSString * ns_element = [AppleString NSStringFromUnicode:_element];
                
                [keysAndValues setObject:ns_element forKey:ns_key];
            }
                , [keysAndValues, ns_key]( const ParamConstString & _element )
            {
                NSString * ns_element = [AppleString NSStringFromConstString:_element];
                
                [keysAndValues setObject:ns_element forKey:ns_key];
            } );
        }

        [[FIRCrashlytics crashlytics] setCustomKeysAndValues: keysAndValues];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFirebaseCrashlyticsService::recordError( const String & _name, uint32_t _code )
    {
        LOGGER_MESSAGE( "record error name: %s code: %u"
            , _name.c_str()
            , _code
        );

        NSDictionary *userInfo = @{
          NSLocalizedDescriptionKey: @(_name.c_str())
        };
        
        NSError * error = [NSError errorWithDomain:@("com.mengine.firebase")
                                             code:_code
                                         userInfo:userInfo];

        [[FIRCrashlytics crashlytics] recordError:error];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFirebaseCrashlyticsService::recordError( NSError * _error )
    {
        LOGGER_MESSAGE( "record error: %s"
           , [AppleDetail getMessageFromNSError:_error].c_str()
        );

        [[FIRCrashlytics crashlytics] recordError:_error];
    }
    //////////////////////////////////////////////////////////////////////////
}
