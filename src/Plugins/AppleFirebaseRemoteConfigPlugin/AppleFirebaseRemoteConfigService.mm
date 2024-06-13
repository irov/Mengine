#include "AppleFirebaseRemoteConfigService.h"

#include "Environment/Apple/AppleString.h"

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
        //Empty
        
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleFirebaseRemoteConfigService::_finalizeService()
    {
        //Empty
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
        
        ConstString value_cs = Helper::NSStringToConstString( value );
        
        return value_cs;
    }
    //////////////////////////////////////////////////////////////////////////
    String AppleFirebaseRemoteConfigService::getValueJSON( const ConstString & _key ) const
    {
        FIRRemoteConfigValue * firValue = [[FIRRemoteConfig remoteConfig] configValueForKey:@(_key.c_str())];
        
        NSString * value = [firValue stringValue];
        
        String value_str = Helper::NSStringToString( value );
        
        return value_str;
    }
    //////////////////////////////////////////////////////////////////////////
}
