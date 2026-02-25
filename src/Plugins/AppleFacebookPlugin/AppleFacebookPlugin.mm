#import "AppleFacebookPlugin.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/iOS/iOSApplication.h"
#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"

#import <AuthenticationServices/AuthenticationServices.h>
#import <SafariServices/SafariServices.h>
#import <FBSDKCoreKit/FBSDKCoreKit-Swift.h>

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "AppleFacebookScriptEmbedding.h"
#endif

@implementation AppleFacebookPlugin

- (instancetype)init {
    self = [super init];

    if( self != nil )
    {
        self.m_provider = nil;
        self.m_loginManager = [[FBSDKLoginManager alloc] init];
        self.m_shareDelegate = [[AppleFacebookShareDelegate alloc] initWithFacebook:self];
    }

    return self;
}

#pragma mark - iOSPluginInterface

- (void)onRunBegin {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    Mengine::Helper::addScriptEmbedding<Mengine::AppleFacebookScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE );
#endif
}

- (void)onStopEnd {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    Mengine::Helper::removeScriptEmbedding<Mengine::AppleFacebookScriptEmbedding>();
#endif
}

- (void)onFinalize {
    self.m_provider = nil;
    self.m_loginManager = nil;
    self.m_shareDelegate = nil;
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    [FBSDKApplicationDelegate.sharedInstance application:application didFinishLaunchingWithOptions:launchOptions];

    return YES;
}

#pragma mark - UISceneSession lifecycle

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url options:(NSDictionary<NSString *, id> *)options handled:(BOOL *)handler {
    *handler = YES;
    
    return [FBSDKApplicationDelegate.sharedInstance application:application openURL:url options:options];
}

#pragma mark - AppleFacebookInterface

+ (instancetype)sharedInstance {
    static AppleFacebookPlugin * sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[AppleFacebookPlugin class]];
    });

    return sharedInstance;
}

- (void)setProvider:(id<AppleFacebookProviderInterface>)provider {
    self.m_provider = provider;
}

- (id<AppleFacebookProviderInterface>)getProvider {
    return self.m_provider;
}

- (BOOL)login:(NSArray<NSString *> *)permissions {
    UIViewController * viewController = [iOSDetail getRootViewController];

    NSString * sessionId = [iOSApplication.sharedInstance getSessionId];

    FBSDKLoginConfiguration * configuration = [[FBSDKLoginConfiguration alloc] initWithPermissions:permissions
                                                                                            tracking:FBSDKLoginTrackingLimited
                                                                                               nonce:sessionId];

    __weak AppleFacebookPlugin * weakSelf = self;

    [self.m_loginManager logInFromViewController:viewController configuration:configuration completion:^(FBSDKLoginManagerLoginResult * _Nullable result, NSError * _Nullable error) {
        AppleFacebookPlugin * strongSelf = weakSelf;

        if( strongSelf == nil )
        {
            return;
        }

        id<AppleFacebookProviderInterface> provider = strongSelf.m_provider;

        if( error != nil )
        {
            NSString * errorMessage = [AppleDetail getMessageFromNSError:error];

            IOS_LOGGER_ERROR( @"Facebook login error: %@", errorMessage );

            if( provider != nil )
            {
                [AppleDetail addMainQueueOperation:^{
                    [provider onFacebookError:-2 message:errorMessage];
                }];
            }

            return;
        }

        if( result.isCancelled == YES )
        {
            IOS_LOGGER_MESSAGE( @"Facebook login cancel" );

            if( provider != nil )
            {
                [AppleDetail addMainQueueOperation:^{
                    [provider onFacebookLoginCancel];
                }];
            }

            return;
        }

        IOS_LOGGER_MESSAGE( @"Facebook login successful" );

        if( provider != nil )
        {
            NSMutableDictionary<NSString *, NSString *> * params = [NSMutableDictionary dictionary];

            FBSDKProfile * profile = [FBSDKProfile currentProfile];

            if( profile != nil )
            {
                if( profile.userID != nil )
                {
                    [params setObject:profile.userID forKey:@"profile.userID"];
                }

                if( profile.imageURL != nil )
                {
                    NSURL * imageURL = profile.imageURL.absoluteURL;

                    if( imageURL != nil && imageURL.absoluteString != nil )
                    {
                        [params setObject:imageURL.absoluteString forKey:@"profile.imageURL"];
                    }
                }
            }

            FBSDKAuthenticationToken * authenticationToken = [FBSDKAuthenticationToken currentAuthenticationToken];

            if( authenticationToken != nil && authenticationToken.tokenString != nil )
            {
                [params setObject:authenticationToken.tokenString forKey:@"authentication.token"];
            }

            [AppleDetail addMainQueueOperation:^{
                [provider onFacebookLoginSuccess:params];
            }];
        }
    }];

    return YES;
}

- (void)logout {
    [self.m_loginManager logOut];

    [FBSDKAuthenticationToken setCurrentAuthenticationToken:nil];
    [FBSDKProfile setCurrentProfile:nil];
}

- (BOOL)isLoggedIn {
    FBSDKAuthenticationToken * authenticationToken = [FBSDKAuthenticationToken currentAuthenticationToken];

    return authenticationToken != nil;
}

- (NSString * _Nullable)getAccessToken {
    FBSDKAuthenticationToken * authenticationToken = [FBSDKAuthenticationToken currentAuthenticationToken];

    if( authenticationToken == nil )
    {
        return nil;
    }

    return authenticationToken.tokenString;
}

- (NSString * _Nullable)getUserId {
    FBSDKProfile * profile = [FBSDKProfile currentProfile];

    if( profile == nil )
    {
        return nil;
    }

    return profile.userID;
}

- (void)shareLink:(NSString *)link picture:(NSString *)picture {
    id<AppleFacebookProviderInterface> provider = self.m_provider;

    NSString * strPicture = picture != nil ? picture : @"";
    NSString * strLink = link != nil ? link : @"";

    if( strLink.length <= 0 && strPicture.length <= 0 )
    {
        IOS_LOGGER_ERROR( @"Facebook link and picture is empty" );

        if( provider != nil )
        {
            [AppleDetail addMainQueueOperation:^{
                [provider onFacebookShareError:-1 message:@"empty data"];
            }];
        }

        return;
    }

    if( self.m_shareDelegate == nil )
    {
        IOS_LOGGER_ERROR( @"Facebook share delegate is nil" );

        if( provider != nil )
        {
            [AppleDetail addMainQueueOperation:^{
                [provider onFacebookShareError:-2 message:@"share delegate nil"];
            }];
        }

        return;
    }

    if( strPicture.length <= 0 )
    {
        dispatch_async( dispatch_get_main_queue(), ^{
            FBSDKShareLinkContent * content = [FBSDKShareLinkContent alloc];
            content.contentURL = [NSURL URLWithString:strLink];

            UIViewController * viewController = [iOSDetail getRootViewController];

            FBSDKShareDialog * dialog = [FBSDKShareDialog dialogWithViewController:viewController
                                                                        withContent:content
                                                                           delegate:self.m_shareDelegate];

            [dialog show];
        });

        return;
    }

    NSURL * url = [NSURL URLWithString:strPicture];

    if( url == nil )
    {
        IOS_LOGGER_ERROR( @"Facebook picture not convert to NSURL" );

        if( provider != nil )
        {
            [AppleDetail addMainQueueOperation:^{
                [provider onFacebookShareError:-3 message:@"error picture to NSURL"];
            }];
        }

        return;
    }

    NSData * data = [NSData dataWithContentsOfURL:url];

    if( data == nil )
    {
        IOS_LOGGER_ERROR( @"Facebook picture not convert to NSData" );

        if( provider != nil )
        {
            [AppleDetail addMainQueueOperation:^{
                [provider onFacebookShareError:-4 message:@"empty NSURL to NSData"];
            }];
        }

        return;
    }

    UIImage * img = [UIImage imageWithData:data];

    if( img == nil )
    {
        IOS_LOGGER_ERROR( @"Facebook picture not convert to UIImage" );

        if( provider != nil )
        {
            [AppleDetail addMainQueueOperation:^{
                [provider onFacebookShareError:-5 message:@"empty NSData to UIImage"];
            }];
        }

        return;
    }

    dispatch_async( dispatch_get_main_queue(), ^{
        FBSDKSharePhoto * sharePhoto = [[FBSDKSharePhoto alloc] initWithImage:img isUserGenerated:true];

        FBSDKShareMediaContent * content = [FBSDKShareMediaContent alloc];
        content.contentURL = [NSURL URLWithString:strLink];
        content.media = @[sharePhoto];

        UIViewController * viewController = [iOSDetail getRootViewController];

        FBSDKShareDialog * dialog = [FBSDKShareDialog dialogWithViewController:viewController
                                                                    withContent:content
                                                                       delegate:self.m_shareDelegate];

        [dialog show];
    });
}

- (void)getProfilePictureLink {
    id<AppleFacebookProviderInterface> provider = self.m_provider;

    if( provider == nil )
    {
        return;
    }

    FBSDKProfile * profile = [FBSDKProfile currentProfile];

    if( profile != nil )
    {
        NSString * userId = profile.userID != nil ? profile.userID : @"";
        NSString * imageURL = @"";

        if( profile.imageURL != nil )
        {
            NSURL * absoluteURL = profile.imageURL.absoluteURL;

            if( absoluteURL != nil && absoluteURL.absoluteString != nil )
            {
                imageURL = absoluteURL.absoluteString;
            }
        }

        [AppleDetail addMainQueueOperation:^{
            [provider onFacebookProfilePictureLinkGetSuccess:userId pictureURL:imageURL];
        }];

        return;
    }

    [FBSDKProfile loadCurrentProfileWithCompletion:^(FBSDKProfile * _Nullable loadedProfile, NSError * _Nullable error) {
        if( error != nil )
        {
            IOS_LOGGER_ERROR( @"Facebook get picture error: %@", [AppleDetail getMessageFromNSError:error] );

            [AppleDetail addMainQueueOperation:^{
                [provider onFacebookProfilePictureLinkGetError:error.code message:error.description];
            }];

            return;
        }

        if( loadedProfile == nil )
        {
            IOS_LOGGER_ERROR( @"Facebook profile is nil" );

            [AppleDetail addMainQueueOperation:^{
                [provider onFacebookProfilePictureLinkGetError:-1 message:@"profile is nil"];
            }];

            return;
        }

        if( loadedProfile.imageURL == nil )
        {
            IOS_LOGGER_ERROR( @"Facebook profile image URL is nil" );

            [AppleDetail addMainQueueOperation:^{
                [provider onFacebookProfilePictureLinkGetError:-2 message:@"imageURL is nil"];
            }];

            return;
        }

        if( loadedProfile.imageURL.absoluteURL == nil )
        {
            IOS_LOGGER_ERROR( @"Facebook profile image URL absolute is nil" );

            [AppleDetail addMainQueueOperation:^{
                [provider onFacebookProfilePictureLinkGetError:-3 message:@"absoluteURL is nil"];
            }];

            return;
        }

        NSString * userId = loadedProfile.userID != nil ? loadedProfile.userID : @"";
        NSString * imageURL = loadedProfile.imageURL.absoluteURL.absoluteString != nil
            ? loadedProfile.imageURL.absoluteURL.absoluteString
            : @"";

        [AppleDetail addMainQueueOperation:^{
            [provider onFacebookProfilePictureLinkGetSuccess:userId pictureURL:imageURL];
        }];
    }];
}

@end
