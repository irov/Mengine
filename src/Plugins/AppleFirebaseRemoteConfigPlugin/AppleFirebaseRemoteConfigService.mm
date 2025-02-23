#include "AppleFirebaseRemoteConfigService.h"

#import "Environment/Apple/AppleString.h"
#import "Environment/Apple/AppleDetail.h"

#if defined(MENGINE_USE_SCRIPT_SERVICE)
#   include "Interface/ScriptServiceInterface.h"

#   include "AppleFirebaseRemoteConfigScriptEmbedding.h"
#endif

#import "AppleFirebaseRemoteConfigApplicationDelegate.h"

#include "Kernel/NotificationHelper.h"
#include "Kernel/Logger.h"

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
#if defined(MENGINE_USE_SCRIPT_SERVICE)
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
#if defined(MENGINE_USE_SCRIPT_SERVICE)
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFirebaseRemoteConfigService::hasRemoteConfig( const ConstString & _key ) const
    {
        NSString * key_ns = [AppleString NSStringFromConstString:_key];
        
        if( [[AppleFirebaseRemoteConfigApplicationDelegate sharedInstance] existRemoteConfigValue:key_ns] == NO )
        {
            return false;
        }
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFirebaseRemoteConfigService::getRemoteConfigValue( const ConstString & _key, Params * const _params ) const
    {
        NSString * key_ns = [AppleString NSStringFromConstString:_key];
        
        NSDictionary * value = [[AppleFirebaseRemoteConfigApplicationDelegate sharedInstance] getRemoteConfigValue:key_ns];
        
        if( value == nil )
        {
            LOGGER_WARNING("Apple firebase remote config not found key: %s"
               , _key.c_str()
            );
            
            return false;
        }
        
        [AppleDetail getParamsFromNSDictionary:value outParams:_params];
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
