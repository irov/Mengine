#include "AppleHelpshiftService.h"

#include "Environment/Apple/AppleUtils.h"

#include "Kernel/Logger.h"
#include "Kernel/ConfigHelper.h"

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
            
        const Char * AppleHelpshiftPlugin_PlatformId = CONFIG_VALUE( "AppleHelpshiftPlugin", "PlatformId", "" );
        const Char * AppleHelpshiftPlugin_Domain = CONFIG_VALUE( "AppleHelpshiftPlugin", "Domain", "" );
            
        @try {
            [Helpshift installWithPlatformId:[NSString stringWithUTF8String: AppleHelpshiftPlugin_PlatformId]
                                      domain:[NSString stringWithUTF8String: AppleHelpshiftPlugin_Domain]
                                      config:config];
        } @catch (NSException * ex) {
            LOGGER_ERROR("Helpshift install with platformId '%s' domain '%s' throw exception: %s"
                , AppleHelpshiftPlugin_PlatformId
                , AppleHelpshiftPlugin_Domain
                , [ex.reason UTF8String]
            );
            
            return false;
        }
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleHelpshiftService::_finalizeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleHelpshiftService::showConversation()
    {
        UIViewController * viewController = [UIApplication sharedApplication].delegate.window.rootViewController;
        
        NSDictionary* configDictionary = nullptr;
        
        [Helpshift showConversationWith:viewController config:configDictionary];
    }
    /////////////////////////////////////////////////////////////////////////////
    void AppleHelpshiftService::showFAQs()
    {
        UIViewController * viewController = [UIApplication sharedApplication].delegate.window.rootViewController;
        
        NSDictionary* configDictionary = nullptr;
        
        [Helpshift showFAQsWith:viewController config:configDictionary];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleHelpshiftService::showFAQSection( const Char * _sectionId )
    {
        UIViewController * viewController = [UIApplication sharedApplication].delegate.window.rootViewController;
        
        NSDictionary* configDictionary = nullptr;
        NSString* sectionId = [NSString stringWithUTF8String:_sectionId];
        
        [Helpshift showFAQSection:sectionId with:viewController config:configDictionary];
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleHelpshiftService::showSingleFAQ( const Char * _faqId )
    {
        UIViewController * viewController = [UIApplication sharedApplication].delegate.window.rootViewController;
        
        NSDictionary* configDictionary = nullptr;
        NSString* faqId =[NSString stringWithUTF8String:_faqId];
        
        [Helpshift showSingleFAQ:faqId with:viewController config:configDictionary];
    }
    //////////////////////////////////////////////////////////////////////////
    // lang vars - en, de, es, fr, it, ru, zh-Hans,zh-Hant, zh-HK, zh-Hant-HK, zh-SG, zh-Hant-SG, pt, ko, ja, tr, nl, cs, hu,, th, sl, vi, ar, pl, no, sv, fi, ro, el, da, ms, iw, sk, uk, ca, hr, bn, bg, gu, hi, kn, lv, ml, mr, pa, fa, ta, te
    void AppleHelpshiftService::setLanguage( const Char * _language )
    {
        NSString * language = [NSString stringWithUTF8String:_language];
        
        [Helpshift setLanguage:language];
    }
    //////////////////////////////////////////////////////////////////////////
}
