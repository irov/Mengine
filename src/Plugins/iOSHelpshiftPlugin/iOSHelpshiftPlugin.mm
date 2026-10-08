#import "iOSHelpshiftPlugin.h"

#import "Environment/Apple/AppleString.h"

#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"

#include "Kernel/ConfigHelper.h"

#import <HelpshiftX/Helpshift.h>

@implementation iOSHelpshiftPlugin

- (instancetype)init {
    self = [super init];

    if( self != nil )
    {
        self.m_provider = nil;
        self.m_delegate = nil;
    }

    return self;
}

+ (instancetype)sharedInstance {
    static iOSHelpshiftPlugin * sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[iOSHelpshiftPlugin class]];
    });

    return sharedInstance;
}

#pragma mark - iOSPluginInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    MENGINE_UNUSED( application );
    MENGINE_UNUSED( launchOptions );

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

    Mengine::String platformId = CONFIG_VALUE_STRING( "HelpshiftPlugin", "PlatformId", "" );
    Mengine::String domain = CONFIG_VALUE_STRING( "HelpshiftPlugin", "Domain", "" );

    if( platformId.empty() == true || domain.empty() == true )
    {
        IOS_LOGGER_ERROR( @"don't setup PlatformId or Domain" );

        return NO;
    }

    NSString * platformIdString = [AppleString NSStringFromString:platformId];
    NSString * domainString = [AppleString NSStringFromString:domain];

    @try {
        [Helpshift installWithPlatformId:platformIdString
                                  domain:domainString
                                  config:config];
    } @catch (NSException * exception) {
        IOS_LOGGER_ERROR( @"install with platformId '%@' domain '%@' throw exception: %s"
            , platformIdString
            , domainString
            , [exception.reason UTF8String]
        );

        return NO;
    }

    self.m_delegate = [[iOSHelpshiftDelegate alloc] initWithHelpshift:self];
    Helpshift.sharedInstance.delegate = self.m_delegate;

    return YES;
}

- (void)onFinalize {
    Helpshift.sharedInstance.delegate = nil;

    self.m_provider = nil;
    self.m_delegate = nil;
}

#pragma mark - iOSHelpshiftInterface

- (void)setProvider:(id<iOSHelpshiftProviderInterface>)provider {
    self.m_provider = provider;
}

- (id<iOSHelpshiftProviderInterface>)getProvider {
    return self.m_provider;
}

- (void)showConversation {
    NSDictionary * configDictionary = nil;

    [Helpshift showConversationWithConfig:configDictionary];
}

- (void)showFAQs {
    NSDictionary * configDictionary = nil;

    [Helpshift showFAQsWithConfig:configDictionary];
}

- (void)showFAQSection:(NSString *)sectionId {
    NSDictionary * configDictionary = nil;

    [Helpshift showFAQSection:sectionId withConfig:configDictionary];
}

- (void)showSingleFAQ:(NSString *)faqId {
    NSDictionary * configDictionary = nil;

    [Helpshift showSingleFAQ:faqId withConfig:configDictionary];
}

- (void)setLanguage:(NSString *)language {
    [Helpshift setLanguage:language];
}

@end
