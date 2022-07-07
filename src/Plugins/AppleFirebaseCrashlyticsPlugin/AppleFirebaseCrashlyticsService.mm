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
        [FIRApp configure];
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
    void AppleFirebaseCrashlyticsService::sendValue(const ConstString& _val){
        LOGGER_INFO( "FirebaseCrashlytics", "sendValue %s", _val.c_str() );
        
        [[FIRCrashlytics crashlytics] log:[NSString stringWithUTF8String:_val.c_str()]];
    }
    //////////////////////////////////////////////////////////////////////
    void AppleFirebaseCrashlyticsService::sendKeyAndValue(const ConstString& _key, const ConstString& _val)
    {
        LOGGER_INFO( "FirebaseCrashlytics", "sendKeyAndValue %s : %s", _key.c_str() , _val.c_str() );
        
        [[FIRCrashlytics crashlytics] setCustomValue:[NSString stringWithUTF8String:_val.c_str()]
                                              forKey:[NSString stringWithUTF8String:_key.c_str()]];
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFirebaseCrashlyticsService::sendKeyAndValues(const FirebaseCrashlyticsParams& _params){
        
        LOGGER_INFO( "FirebaseCrashlytics", "sendKeyAndValues");
        NSMutableDictionary *keysAndValues = [[NSMutableDictionary alloc] init];
        for(FirebaseCrashlyticsParams::const_iterator it = _params.begin(),it_e = _params.end(); it != it_e; ++it)
        {
            [keysAndValues setObject:[NSString stringWithUTF8String:it->second.c_str()]  forKey:[NSString stringWithUTF8String:it->first.c_str()]];
            LOGGER_INFO("FirebaseCrashlytics", "%s : %s", it->first.c_str(),it->second.c_str());
        }
        [[FIRCrashlytics crashlytics] setCustomKeysAndValues: keysAndValues];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFirebaseCrashlyticsService::sendTestError()
    {
        NSDictionary *userInfo = @{
            NSLocalizedDescriptionKey: NSLocalizedString(@"The request failed.", nil),
            NSLocalizedFailureReasonErrorKey: NSLocalizedString(@"The response returned a 404.", nil),
            NSLocalizedRecoverySuggestionErrorKey: NSLocalizedString(@"Does this page exist?", nil),
            @"ProductID": @"123456",
            @"View": @"MainView",
        };
        
        NSError *error = [NSError errorWithDomain:NSCocoaErrorDomain
                                            code:-1001
                                        userInfo:userInfo];
        recordError(error);
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFirebaseCrashlyticsService::recordError(const ConstString& _name, long _code, const FirebaseCrashlyticsParams& _params)
    {
        
        NSMutableDictionary *userInfo = [[NSMutableDictionary alloc] init];
        for(FirebaseCrashlyticsParams::const_iterator it = _params.begin(),it_e = _params.end(); it != it_e; ++it)
        {
            [userInfo setObject:[NSString stringWithUTF8String:it->second.c_str()]  forKey:[NSString stringWithUTF8String:it->first.c_str()]];
            LOGGER_INFO("FirebaseCrashlytics", "%s : %s", it->first.c_str(), it->second.c_str());
        }
        
        NSError *error = [NSError errorWithDomain:[NSString stringWithUTF8String:_name.c_str()]
                                             code: _code
                                         userInfo:userInfo];
        recordError(error);
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFirebaseCrashlyticsService::recordError(NSError * _error){
        [[FIRCrashlytics crashlytics] recordError: _error];
    }
}
