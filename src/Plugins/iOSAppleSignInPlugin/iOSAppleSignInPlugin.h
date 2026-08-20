#pragma once

#import "Environment/iOS/iOSPluginInterface.h"

#import "iOSAppleSignInInterface.h"

#import <AuthenticationServices/AuthenticationServices.h>

@interface iOSAppleSignInPlugin
    : NSObject<iOSPluginInterface, iOSAppleSignInInterface, ASAuthorizationControllerDelegate, ASAuthorizationControllerPresentationContextProviding>

@property (nonatomic, strong) id<iOSAppleSignInProviderInterface> m_provider;
@property (nonatomic, strong) ASAuthorizationController * m_authorizationController;
@property (nonatomic, strong) ASAuthorizationAppleIDButton * m_authorizationButton;
@property (nonatomic, assign) BOOL m_observingCredentialRevocation;

@end
