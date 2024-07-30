#include "AppleFirebaseRemoteConfigService.h"

#import "Environment/Apple/AppleString.h"
#import "Environment/Apple/AppleDetail.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#   include "Interface/ScriptServiceInterface.h"

#   include "AppleFirebaseRemoteConfigScriptEmbedding.h"
#endif

#include "Kernel/NotificationHelper.h"

#import <FirebaseRemoteConfig/FirebaseRemoteConfig.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AppleFirebaseRemoteConfigService, Mengine::AppleFirebaseRemoteConfigService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleFirebaseRemoteConfigService::AppleFirebaseRemoteConfigService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleFirebaseRemoteConfigService::~AppleFirebaseRemoteConfigService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFirebaseRemoteConfigService::_initializeService()
    {
#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleFirebaseRemoteConfigScriptEmbedding" ), Helper::makeFactorableUnique<AppleFirebaseRemoteConfigScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleFirebaseRemoteConfigScriptEmbedding" ) );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif
        
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleFirebaseRemoteConfigService::_finalizeService()
    {
#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFirebaseRemoteConfigService::getValueBoolean( const ConstString & _key ) const
    {
        FIRRemoteConfigValue * firValue = [[FIRRemoteConfig remoteConfig] configValueForKey:@(_key.c_str())];
        
        BOOL value = [firValue boolValue];
        
        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t AppleFirebaseRemoteConfigService::getValueInteger( const ConstString & _key ) const
    {
        FIRRemoteConfigValue * firValue = [[FIRRemoteConfig remoteConfig] configValueForKey:@(_key.c_str())];
        
        NSInteger value = [[firValue numberValue] integerValue];
        
        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    double AppleFirebaseRemoteConfigService::getValueDouble( const ConstString & _key ) const
    {
        FIRRemoteConfigValue * firValue = [[FIRRemoteConfig remoteConfig] configValueForKey:@(_key.c_str())];
        
        NSInteger value = [[firValue numberValue] doubleValue];
        
        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    ConstString AppleFirebaseRemoteConfigService::getValueConstString( const ConstString & _key ) const
    {
        FIRRemoteConfigValue * firValue = [[FIRRemoteConfig remoteConfig] configValueForKey:@(_key.c_str())];
        
        NSString * value = [firValue stringValue];
        
        ConstString value_cs = [AppleString NSStringToConstString:value];
        
        return value_cs;
    }
    //////////////////////////////////////////////////////////////////////////
    Params AppleFirebaseRemoteConfigService::getValueJSON( const ConstString & _key ) const
    {
        FIRRemoteConfigValue * firValue = [[FIRRemoteConfig remoteConfig] configValueForKey:@(_key.c_str())];
        
        NSString * value = [firValue stringValue];
        
        Params params;
        [AppleDetail getParamsFromJSON:value outParams:&params];
        
        return params;
    }
    //////////////////////////////////////////////////////////////////////////
    Params AppleFirebaseRemoteConfigService::getValues() const
    {
        FIRRemoteConfig * remoteConfig = [FIRRemoteConfig remoteConfig];
        
        NSArray<NSString *> * remoteKeys = [remoteConfig allKeysFromSource:FIRRemoteConfigSourceRemote];
        
        Params params;
        
        for( NSString * key in remoteKeys )
        {
            Mengine::ConstString key_cstr = [AppleString NSStringToConstString:key];
            
            FIRRemoteConfigValue * value = [remoteConfig configValueForKey:key];
            
            NSString * value_ns = [value stringValue];
            
            String value_str = [AppleString NSStringToString:value_ns];
            
            params.emplace( key_cstr, value_str );
        }
        
        return params;
    }
    //////////////////////////////////////////////////////////////////////////
}
