#import "iOSAppleSignInPlugin.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/Apple/AppleKeyChain.h"
#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "Kernel/ScriptEmbeddingHelper.h"

#   include "iOSAppleSignInScriptEmbedding.h"
#endif

static NSString * const kMengineAppleSignInUserIdKey = @"mengine.apple.signin.user-id";

static NSString * iOSAppleSignInStringFromData( NSData * data )
{
    if( data == nil )
    {
        return nil;
    }

    return [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
}

@implementation iOSAppleSignInPlugin

- (instancetype)init {
    self = [super init];

    if( self != nil )
    {
        self.m_provider = nil;
        self.m_authorizationController = nil;
        self.m_authorizationButton = nil;
        self.m_observingCredentialRevocation = NO;
    }

    return self;
}

#pragma mark - iOSPluginInterface

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    MENGINE_UNUSED( application );
    MENGINE_UNUSED( launchOptions );

    [self startObservingCredentialRevocation];

    return YES;
}

- (void)onRunBegin {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    Mengine::Helper::addScriptEmbedding<Mengine::iOSAppleSignInScriptEmbedding>( MENGINE_DOCUMENT_FUNCTION );
#endif
}

- (void)onStopEnd {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    Mengine::Helper::removeScriptEmbedding<Mengine::iOSAppleSignInScriptEmbedding>();
#endif
}

- (void)onFinalize {
    [self stopObservingCredentialRevocation];
    [self hideButton];

    self.m_authorizationController.delegate = nil;
    self.m_authorizationController.presentationContextProvider = nil;
    self.m_authorizationController = nil;
    self.m_provider = nil;
}

#pragma mark - iOSAppleSignInInterface

+ (instancetype)sharedInstance {
    static iOSAppleSignInPlugin * sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[iOSAppleSignInPlugin class]];
    });

    return sharedInstance;
}

- (void)setProvider:(id<iOSAppleSignInProviderInterface>)provider {
    self.m_provider = provider;

    if( provider == nil )
    {
        return;
    }

    NSString * userId = [self getStoredUserId];

    if( userId.length != 0 )
    {
        [self checkCredentialState:userId];
    }
}

- (id<iOSAppleSignInProviderInterface>)getProvider {
    return self.m_provider;
}

- (BOOL)isAvailable {
    if( @available(iOS 13.0, *) )
    {
        return YES;
    }

    return NO;
}

- (BOOL)loginWithEmail:(BOOL)requestEmail
              fullName:(BOOL)requestFullName
                 state:(NSString *)state
                 nonce:(NSString *)nonce {
    if( [NSThread isMainThread] == NO )
    {
        IOS_LOGGER_ERROR( @"Apple Sign In login must be called on the main thread" );

        return NO;
    }

    if( [self isAvailable] == NO )
    {
        IOS_LOGGER_ERROR( @"Apple Sign In is not available" );

        return NO;
    }

    if( self.m_authorizationController != nil )
    {
        IOS_LOGGER_ERROR( @"Apple Sign In authorization is already in progress" );

        return NO;
    }

    ASAuthorizationAppleIDProvider * appleIDProvider = [[ASAuthorizationAppleIDProvider alloc] init];
    ASAuthorizationAppleIDRequest * request = [appleIDProvider createRequest];
    NSMutableArray<ASAuthorizationScope> * requestedScopes = [NSMutableArray arrayWithCapacity:2];

    if( requestEmail == YES )
    {
        [requestedScopes addObject:ASAuthorizationScopeEmail];
    }

    if( requestFullName == YES )
    {
        [requestedScopes addObject:ASAuthorizationScopeFullName];
    }

    request.requestedScopes = requestedScopes;

    NSString * storedUserId = [self getStoredUserId];

    if( storedUserId.length != 0 )
    {
        request.user = storedUserId;
    }

    if( state.length != 0 )
    {
        request.state = state;
    }

    if( nonce.length != 0 )
    {
        request.nonce = nonce;
    }

    ASAuthorizationController * authorizationController = [[ASAuthorizationController alloc] initWithAuthorizationRequests:@[request]];
    authorizationController.delegate = self;
    authorizationController.presentationContextProvider = self;

    self.m_authorizationController = authorizationController;

    [authorizationController performRequests];

    return YES;
}

- (NSString *)getStoredUserId {
    return [AppleKeyChain getStringForKey:kMengineAppleSignInUserIdKey defaultValue:nil];
}

- (void)clearStoredCredential {
    [AppleKeyChain removeKey:kMengineAppleSignInUserIdKey];
}

- (BOOL)checkCredentialState:(NSString *)userId {
    NSString * resolvedUserId = userId;

    if( resolvedUserId.length == 0 )
    {
        resolvedUserId = [self getStoredUserId];
    }

    if( resolvedUserId.length == 0 )
    {
        IOS_LOGGER_ERROR( @"Apple Sign In credential state requires a user id" );

        return NO;
    }

    ASAuthorizationAppleIDProvider * appleIDProvider = [[ASAuthorizationAppleIDProvider alloc] init];
    __weak iOSAppleSignInPlugin * weakSelf = self;

    [appleIDProvider getCredentialStateForUserID:resolvedUserId completion:^(ASAuthorizationAppleIDProviderCredentialState credentialState, NSError * error) {
        [AppleDetail addMainQueueOperation:^{
            iOSAppleSignInPlugin * strongSelf = weakSelf;

            if( strongSelf == nil )
            {
                return;
            }

            id<iOSAppleSignInProviderInterface> provider = strongSelf.m_provider;

            if( error != nil && credentialState != ASAuthorizationAppleIDProviderCredentialNotFound )
            {
                NSString * errorMessage = [AppleDetail getMessageFromNSError:error];

                IOS_LOGGER_ERROR( @"Apple Sign In credential state error: %@", errorMessage );

                if( provider != nil )
                {
                    [provider onAppleSignInCredentialStateError:resolvedUserId code:error.code message:errorMessage];
                }

                return;
            }

            if( error != nil )
            {
                IOS_LOGGER_WARNING( @"Apple Sign In credential was not found: %@", [AppleDetail getMessageFromNSError:error] );
            }

            iOSAppleSignInCredentialState state = (iOSAppleSignInCredentialState)credentialState;

            if( provider != nil )
            {
                [provider onAppleSignInCredentialState:resolvedUserId state:state];
            }

            if( credentialState == ASAuthorizationAppleIDProviderCredentialRevoked )
            {
                NSString * storedUserId = [strongSelf getStoredUserId];

                if( [storedUserId isEqualToString:resolvedUserId] == YES )
                {
                    [strongSelf clearStoredCredential];
                }

                if( provider != nil )
                {
                    [provider onAppleSignInCredentialRevoked:resolvedUserId];
                }
            }
            else if( credentialState == ASAuthorizationAppleIDProviderCredentialNotFound )
            {
                NSString * storedUserId = [strongSelf getStoredUserId];

                if( [storedUserId isEqualToString:resolvedUserId] == YES )
                {
                    [strongSelf clearStoredCredential];
                }
            }
        }];
    }];

    return YES;
}

- (BOOL)showButtonWithType:(iOSAppleSignInButtonType)type
                      style:(iOSAppleSignInButtonStyle)style
                          x:(CGFloat)x
                          y:(CGFloat)y
                      width:(CGFloat)width
                     height:(CGFloat)height
               cornerRadius:(CGFloat)cornerRadius {
    if( [NSThread isMainThread] == NO )
    {
        IOS_LOGGER_ERROR( @"Apple Sign In button must be shown on the main thread" );

        return NO;
    }

    if( width <= 0.0 || height <= 0.0 )
    {
        IOS_LOGGER_ERROR( @"Apple Sign In button has invalid size: [%lf, %lf]", (double)width, (double)height );

        return NO;
    }

    ASAuthorizationAppleIDButtonType authorizationButtonType;

    switch( type )
    {
    case iOSAppleSignInButtonTypeSignIn:
        authorizationButtonType = ASAuthorizationAppleIDButtonTypeSignIn;
        break;
    case iOSAppleSignInButtonTypeContinue:
        authorizationButtonType = ASAuthorizationAppleIDButtonTypeContinue;
        break;
    case iOSAppleSignInButtonTypeSignUp:
        authorizationButtonType = ASAuthorizationAppleIDButtonTypeSignUp;
        break;
    default:
        IOS_LOGGER_ERROR( @"Apple Sign In button has invalid type: %ld", (long)type );
        return NO;
    }

    ASAuthorizationAppleIDButtonStyle authorizationButtonStyle;

    switch( style )
    {
    case iOSAppleSignInButtonStyleWhite:
        authorizationButtonStyle = ASAuthorizationAppleIDButtonStyleWhite;
        break;
    case iOSAppleSignInButtonStyleWhiteOutline:
        authorizationButtonStyle = ASAuthorizationAppleIDButtonStyleWhiteOutline;
        break;
    case iOSAppleSignInButtonStyleBlack:
        authorizationButtonStyle = ASAuthorizationAppleIDButtonStyleBlack;
        break;
    default:
        IOS_LOGGER_ERROR( @"Apple Sign In button has invalid style: %ld", (long)style );
        return NO;
    }

    UIViewController * rootViewController = [iOSDetail getRootViewController];

    if( rootViewController == nil )
    {
        IOS_LOGGER_ERROR( @"Apple Sign In button root view controller is unavailable" );

        return NO;
    }

    [self hideButton];

    ASAuthorizationAppleIDButton * authorizationButton = [ASAuthorizationAppleIDButton buttonWithType:authorizationButtonType style:authorizationButtonStyle];
    authorizationButton.frame = CGRectMake( x, y, width, height );
    authorizationButton.cornerRadius = MAX( 0.0, MIN( cornerRadius, height * 0.5 ) );
    authorizationButton.accessibilityIdentifier = @"MengineAppleSignInButton";
    [authorizationButton addTarget:self action:@selector(handleAuthorizationButtonPress:) forControlEvents:UIControlEventTouchUpInside];

    [rootViewController.view addSubview:authorizationButton];

    self.m_authorizationButton = authorizationButton;

    return YES;
}

- (void)hideButton {
    if( self.m_authorizationButton == nil )
    {
        return;
    }

    [self.m_authorizationButton removeTarget:self action:@selector(handleAuthorizationButtonPress:) forControlEvents:UIControlEventTouchUpInside];
    [self.m_authorizationButton removeFromSuperview];
    self.m_authorizationButton = nil;
}

- (BOOL)isButtonVisible {
    return self.m_authorizationButton != nil && self.m_authorizationButton.superview != nil && self.m_authorizationButton.hidden == NO;
}

#pragma mark - ASAuthorizationControllerDelegate

- (void)authorizationController:(ASAuthorizationController *)controller didCompleteWithAuthorization:(ASAuthorization *)authorization {
    if( controller == self.m_authorizationController )
    {
        self.m_authorizationController = nil;
    }

    if( [authorization.credential isKindOfClass:[ASAuthorizationAppleIDCredential class]] == NO )
    {
        [self notifyAuthorizationError:ASAuthorizationErrorInvalidResponse message:@"Apple Sign In returned an unexpected credential type"];

        return;
    }

    ASAuthorizationAppleIDCredential * credential = (ASAuthorizationAppleIDCredential *)authorization.credential;
    NSString * identityToken = iOSAppleSignInStringFromData( credential.identityToken );

    if( identityToken.length == 0 )
    {
        [self notifyAuthorizationError:ASAuthorizationErrorInvalidResponse message:@"Apple Sign In returned an empty identity token"];

        return;
    }

    NSMutableDictionary<NSString *, id> * params = [NSMutableDictionary dictionary];
    params[@"user"] = credential.user;
    params[@"identityToken"] = identityToken;
    params[@"realUserStatus"] = @(credential.realUserStatus);
    params[@"emailScopeAuthorized"] = @([credential.authorizedScopes containsObject:ASAuthorizationScopeEmail]);
    params[@"fullNameScopeAuthorized"] = @([credential.authorizedScopes containsObject:ASAuthorizationScopeFullName]);

    NSString * authorizationCode = iOSAppleSignInStringFromData( credential.authorizationCode );

    if( authorizationCode.length != 0 )
    {
        params[@"authorizationCode"] = authorizationCode;
    }

    if( credential.state.length != 0 )
    {
        params[@"state"] = credential.state;
    }

    if( credential.email.length != 0 )
    {
        params[@"email"] = credential.email;
    }

    NSPersonNameComponents * fullName = credential.fullName;

    if( fullName.namePrefix.length != 0 )
    {
        params[@"namePrefix"] = fullName.namePrefix;
    }

    if( fullName.givenName.length != 0 )
    {
        params[@"givenName"] = fullName.givenName;
    }

    if( fullName.middleName.length != 0 )
    {
        params[@"middleName"] = fullName.middleName;
    }

    if( fullName.familyName.length != 0 )
    {
        params[@"familyName"] = fullName.familyName;
    }

    if( fullName.nameSuffix.length != 0 )
    {
        params[@"nameSuffix"] = fullName.nameSuffix;
    }

    if( fullName.nickname.length != 0 )
    {
        params[@"nickname"] = fullName.nickname;
    }

    if( [AppleKeyChain setStringForKey:kMengineAppleSignInUserIdKey value:credential.user] == NO )
    {
        IOS_LOGGER_ERROR( @"Apple Sign In failed to persist the user id" );
    }

    IOS_LOGGER_MESSAGE( @"Apple Sign In authorization succeeded" );

    id<iOSAppleSignInProviderInterface> provider = self.m_provider;

    if( provider != nil )
    {
        [AppleDetail addMainQueueOperation:^{
            [provider onAppleSignInSuccess:params];
        }];
    }
}

- (void)authorizationController:(ASAuthorizationController *)controller didCompleteWithError:(NSError *)error {
    if( controller == self.m_authorizationController )
    {
        self.m_authorizationController = nil;
    }

    id<iOSAppleSignInProviderInterface> provider = self.m_provider;

    if( [error.domain isEqualToString:ASAuthorizationErrorDomain] == YES && error.code == ASAuthorizationErrorCanceled )
    {
        IOS_LOGGER_MESSAGE( @"Apple Sign In authorization canceled" );

        if( provider != nil )
        {
            [AppleDetail addMainQueueOperation:^{
                [provider onAppleSignInCancel];
            }];
        }

        return;
    }

    NSString * errorMessage = [AppleDetail getMessageFromNSError:error];

    IOS_LOGGER_ERROR( @"Apple Sign In authorization error: %@", errorMessage );

    if( provider != nil )
    {
        [AppleDetail addMainQueueOperation:^{
            [provider onAppleSignInError:error.code message:errorMessage];
        }];
    }
}

#pragma mark - ASAuthorizationControllerPresentationContextProviding

- (ASPresentationAnchor)presentationAnchorForAuthorizationController:(ASAuthorizationController *)controller {
    MENGINE_UNUSED( controller );

    return [iOSDetail getRootWindow];
}

#pragma mark - Private

- (void)notifyAuthorizationError:(NSInteger)code message:(NSString *)message {
    IOS_LOGGER_ERROR( @"Apple Sign In authorization error: %@", message );

    id<iOSAppleSignInProviderInterface> provider = self.m_provider;

    if( provider != nil )
    {
        [AppleDetail addMainQueueOperation:^{
            [provider onAppleSignInError:code message:message];
        }];
    }
}

- (void)startObservingCredentialRevocation {
    if( self.m_observingCredentialRevocation == YES )
    {
        return;
    }

    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(handleCredentialRevokedNotification:)
                                                 name:ASAuthorizationAppleIDProviderCredentialRevokedNotification
                                               object:nil];

    self.m_observingCredentialRevocation = YES;
}

- (void)stopObservingCredentialRevocation {
    if( self.m_observingCredentialRevocation == NO )
    {
        return;
    }

    [[NSNotificationCenter defaultCenter] removeObserver:self
                                                    name:ASAuthorizationAppleIDProviderCredentialRevokedNotification
                                                  object:nil];

    self.m_observingCredentialRevocation = NO;
}

- (void)handleCredentialRevokedNotification:(NSNotification *)notification {
    MENGINE_UNUSED( notification );

    NSString * userId = [self getStoredUserId];

    if( userId.length == 0 )
    {
        IOS_LOGGER_WARNING( @"Apple Sign In credential revocation received without a stored user id" );

        return;
    }

    [self checkCredentialState:userId];
}

- (void)handleAuthorizationButtonPress:(ASAuthorizationAppleIDButton *)button {
    MENGINE_UNUSED( button );

    [self loginWithEmail:YES fullName:YES state:nil nonce:nil];
}

@end
