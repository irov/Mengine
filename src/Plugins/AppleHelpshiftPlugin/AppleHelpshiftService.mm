#include "AppleHelpshiftService.h"

#include "Environment/Apple/AppleUtils.h"
#include "Environment/iOS/iOSDetail.h"

#include "Kernel/Logger.h"
#include "Kernel/ConfigHelper.h"

#include "Config/StdString.h"

#import <Foundation/Foundation.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AppleHelpshiftService, Mengine::AppleHelpshiftService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleHelpshiftService::AppleHelpshiftService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleHelpshiftService::~AppleHelpshiftService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleHelpshiftService::setProvider( const AppleHelpshiftProviderInterfacePtr & _provider )
    {
        m_provider = _provider;
    }
    //////////////////////////////////////////////////////////////////////////
    const AppleHelpshiftProviderInterfacePtr & AppleHelpshiftService::getProvider() const
    {
        return m_provider;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleHelpshiftService::_initializeService()
    {
        NSDictionary * config = @{
            @"enableLogging": MENGINE_DEBUG_VALUE(@YES, @NO),
            @"enableInAppNotification": @YES,
            @"inAppNotificationAppearance": @{
                @"bannerBackgroundColor": @"000000",
                @"textColor": @"FFFFFF"
            },
            @"presentFullScreenOniPad": @NO,
            @"enableFullPrivacy": @NO
        };
            
        const Char * AppleHelpshiftPlugin_PlatformId = CONFIG_VALUE( "HelpshiftPlugin", "PlatformId", "" );
        const Char * AppleHelpshiftPlugin_Domain = CONFIG_VALUE( "HelpshiftPlugin", "Domain", "" );
        
        if( MENGINE_STRCMP( AppleHelpshiftPlugin_PlatformId, "" ) == 0 || MENGINE_STRCMP( AppleHelpshiftPlugin_Domain, "" ) == 0 )
        {
            LOGGER_ERROR( "don't setup PlatformId or Domain" );
            
            return false;
        }

        @try {
            [Helpshift installWithPlatformId:[NSString stringWithUTF8String: AppleHelpshiftPlugin_PlatformId]
                                      domain:[NSString stringWithUTF8String: AppleHelpshiftPlugin_Domain]
                                      config:config];
        } @catch (NSException * ex) {
            LOGGER_ERROR("install with platformId '%s' domain '%s' throw exception: %s"
                , AppleHelpshiftPlugin_PlatformId
                , AppleHelpshiftPlugin_Domain
                , [ex.reason UTF8String]
            );
            
            return false;
        }
        
        m_delegate = [[AppleHelpshiftDelegate alloc] initWithService:this];
        
        Helpshift.sharedInstance.delegate = m_delegate;
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleHelpshiftService::_finalizeService()
    {
        m_delegate = nil;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleHelpshiftService::showConversation()
    {
        UIViewController * viewController = Helper::iOSGetRootViewController();
        
        NSDictionary * configDictionary = nullptr;
        
        [Helpshift showConversationWith:viewController config:configDictionary];
    }
    /////////////////////////////////////////////////////////////////////////////
    void AppleHelpshiftService::showFAQs()
    {
        UIViewController * viewController = Helper::iOSGetRootViewController();
        
        NSDictionary * configDictionary = nullptr;
        
        [Helpshift showFAQsWith:viewController config:configDictionary];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleHelpshiftService::showFAQSection( const Char * _sectionId )
    {
        UIViewController * viewController = Helper::iOSGetRootViewController();
        
        NSDictionary * configDictionary = nullptr;
        NSString * sectionId = [NSString stringWithUTF8String:_sectionId];
        
        [Helpshift showFAQSection:sectionId with:viewController config:configDictionary];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleHelpshiftService::showSingleFAQ( const Char * _faqId )
    {
        UIViewController * viewController = Helper::iOSGetRootViewController();
        
        NSDictionary * configDictionary = nullptr;
        NSString * faqId =[NSString stringWithUTF8String:_faqId];
        
        [Helpshift showSingleFAQ:faqId with:viewController config:configDictionary];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleHelpshiftService::setLanguage( const Char * _language )
    {
        NSString * language = [NSString stringWithUTF8String:_language];
        
        [Helpshift setLanguage:language];
    }
    //////////////////////////////////////////////////////////////////////////
}
