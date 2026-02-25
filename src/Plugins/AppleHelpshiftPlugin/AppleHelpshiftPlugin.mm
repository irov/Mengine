#import "AppleHelpshiftPlugin.h"

#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"

#include "Kernel/ConfigHelper.h"

#import <HelpshiftX/Helpshift.h>

@implementation AppleHelpshiftPlugin

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
    static AppleHelpshiftPlugin * sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[AppleHelpshiftPlugin class]];
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

    const Char * platformId = CONFIG_VALUE( "HelpshiftPlugin", "PlatformId", "" );
    const Char * domain = CONFIG_VALUE( "HelpshiftPlugin", "Domain", "" );

    if( platformId[0] == '\0' || domain[0] == '\0' )
    {
        IOS_LOGGER_ERROR( @"don't setup PlatformId or Domain" );

        return NO;
    }

    @try {
        [Helpshift installWithPlatformId:[NSString stringWithUTF8String:platformId]
                                  domain:[NSString stringWithUTF8String:domain]
                                  config:config];
    } @catch (NSException * exception) {
        IOS_LOGGER_ERROR( @"install with platformId '%s' domain '%s' throw exception: %s"
            , platformId
            , domain
            , [exception.reason UTF8String]
        );

        return NO;
    }

    self.m_delegate = [[AppleHelpshiftDelegate alloc] initWithHelpshift:self];
    Helpshift.sharedInstance.delegate = self.m_delegate;

    return YES;
}

- (void)onFinalize {
    Helpshift.sharedInstance.delegate = nil;

    self.m_provider = nil;
    self.m_delegate = nil;
}

#pragma mark - AppleHelpshiftInterface

- (void)setProvider:(id<AppleHelpshiftProviderInterface>)provider {
    self.m_provider = provider;
}

- (id<AppleHelpshiftProviderInterface>)getProvider {
    return self.m_provider;
}

- (void)showConversation {
    UIViewController * viewController = [iOSDetail getRootViewController];

    NSDictionary * configDictionary = nil;

    [Helpshift showConversationWith:viewController config:configDictionary];
}

- (void)showFAQs {
    UIViewController * viewController = [iOSDetail getRootViewController];

    NSDictionary * configDictionary = nil;

    [Helpshift showFAQsWith:viewController config:configDictionary];
}

- (void)showFAQSection:(NSString *)sectionId {
    UIViewController * viewController = [iOSDetail getRootViewController];

    NSDictionary * configDictionary = nil;

    [Helpshift showFAQSection:sectionId with:viewController config:configDictionary];
}

- (void)showSingleFAQ:(NSString *)faqId {
    UIViewController * viewController = [iOSDetail getRootViewController];

    NSDictionary * configDictionary = nil;

    [Helpshift showSingleFAQ:faqId with:viewController config:configDictionary];
}

- (void)setLanguage:(NSString *)language {
    [Helpshift setLanguage:language];
}

@end
