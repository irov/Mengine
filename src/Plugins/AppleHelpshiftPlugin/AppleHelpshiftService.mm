#include "AppleHelpshiftService.h"

#include "Environment/Apple/AppleUtils.h"

#include "Kernel/Logger.h"
#include "Kernel/ConfigHelper.h"
#import <Foundation/Foundation.h>

////////////////////////////////////////////////////////////////////////////
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
    void AppleHelpshiftService::showConversation()
    {
        NSDictionary* configDictionary = nullptr;
        [Helpshift showConversationWith:[UIApplication sharedApplication].delegate.window.rootViewController config:configDictionary];
    }
    /////////////////////////////////////////////////////////////////////////////
    void AppleHelpshiftService::showFAQs()
    {
        NSDictionary* configDictionary = nullptr;
        [Helpshift showFAQsWith:[UIApplication sharedApplication].delegate.window.rootViewController config:configDictionary];
    }
    /////////////////////////////////////////////////////////////////////////////
    void AppleHelpshiftService::showFAQSection(const char* _setionId)
    {
        NSDictionary* configDictionary = nullptr;
        NSString* sectionId = [NSString stringWithUTF8String:_setionId];
        [Helpshift showFAQSection:sectionId with:[UIApplication sharedApplication].delegate.window.rootViewController config:configDictionary];
    }
//    /////////////////////////////////////////////////////////////////////////////
    void AppleHelpshiftService::showSingleFAQ(const char* _faqId)
    {
        NSDictionary* configDictionary = nullptr;
        NSString* faqId =[NSString stringWithUTF8String:_faqId];
        [Helpshift showSingleFAQ:faqId with:[UIApplication sharedApplication].delegate.window.rootViewController config:configDictionary];
    }
//    /////////////////////////////////////////////////////////////////////////////
    //lang vars - en, de, es, fr, it, ru, zh-Hans,zh-Hant, zh-HK, zh-Hant-HK, zh-SG, zh-Hant-SG, pt, ko, ja, tr, nl, cs, hu,, th, sl, vi, ar, pl, no, sv, fi, ro, el, da, ms, iw, sk, uk, ca, hr, bn, bg, gu, hi, kn, lv, ml, mr, pa, fa, ta, te
    void AppleHelpshiftService::setLanguage( const char* _lang)
    {
        NSString* lang = [NSString stringWithUTF8String:_lang];
        [Helpshift setLanguage:lang];
    }
    /////////////////////////////////////////////////////////////////////////////
    bool AppleHelpshiftService::_initializeService()
    {
        try{
            NSDictionary *config = @{
#ifdef MENGINE_DEBUG
                @"enableLogging":@YES,
#endif
                //        @"enableInAppNotification":@YES,
                
                //        @"inAppNotificationAppearance": @{
                //            @"bannerBackgroundColor": @"000000",
                //            @"textColor": @"FFFFFF"
                //        }
                
                //        @"presentFullScreenOniPad":@YES,
            };
            
            
            NSString *platform_id = [NSString stringWithUTF8String: CONFIG_VALUE( "HelpshiftPlugin", "PlatformId", "" )];
            NSString *domain = [NSString stringWithUTF8String: CONFIG_VALUE( "HelpshiftPlugin", "Domain", "" )];
            
            [Helpshift installWithPlatformId:platform_id
                                      domain:domain
                                      config:config];
            
        }catch(...){
            return false;
        }
        
        return true;
    }
    ////////////////////////////////////////////////////////////////////////
    void AppleHelpshiftService::_finalizeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
}
