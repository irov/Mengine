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
//    create script ->   https://firebase.google.com/docs/crashlytics/get-started?platform=ios#set-up-dsym-uploading
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
    //////////////////////////////////////////////////////////////////////
    bool AppleFirebaseCrashlyticsService::sendEvent()
    {
        LOGGER_INFO( "FirebaseCrashlytics", "sendEvent" );
        
        [[FIRCrashlytics crashlytics] setCustomValue:@(50) forKey:@"int_key"];
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFirebaseCrashlyticsService::sendKeyAndValues(){
        NSDictionary *keysAndValues =
            @{@"string key" : @"string value",
            @"string key 2" : @"string value 2",
            @"boolean key" : @(YES),
            @"boolean key 2" : @(NO),
            @"float key" : @(1.01),
            @"float key 2" : @(2.02)};
        
        [[FIRCrashlytics crashlytics] setCustomKeysAndValues: keysAndValues];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFirebaseCrashlyticsService::recordError(NSError * error){
        [[FIRCrashlytics crashlytics] recordError:error];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFirebaseCrashlyticsService::logEvent(){
        [[FIRCrashlytics crashlytics] log:@"Simple string message"];

        [[FIRCrashlytics crashlytics] logWithFormat:@"Higgs-Boson detected! Bailing out... %@", attributesDict];

    }
}
