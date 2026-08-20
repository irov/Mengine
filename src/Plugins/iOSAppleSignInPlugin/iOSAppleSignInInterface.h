#pragma once

#import "Environment/Apple/AppleIncluder.h"

typedef NS_ENUM(NSInteger, iOSAppleSignInCredentialState) {
    iOSAppleSignInCredentialStateUnknown = -1,
    iOSAppleSignInCredentialStateRevoked = 0,
    iOSAppleSignInCredentialStateAuthorized = 1,
    iOSAppleSignInCredentialStateNotFound = 2,
    iOSAppleSignInCredentialStateTransferred = 3,
};

typedef NS_ENUM(NSInteger, iOSAppleSignInButtonType) {
    iOSAppleSignInButtonTypeSignIn = 0,
    iOSAppleSignInButtonTypeContinue = 1,
    iOSAppleSignInButtonTypeSignUp = 2,
};

typedef NS_ENUM(NSInteger, iOSAppleSignInButtonStyle) {
    iOSAppleSignInButtonStyleWhite = 0,
    iOSAppleSignInButtonStyleWhiteOutline = 1,
    iOSAppleSignInButtonStyleBlack = 2,
};

@protocol iOSAppleSignInProviderInterface <NSObject>

// identityToken and authorizationCode are short-lived and must be sent to the
// backend immediately. Email and name components are normally returned only
// on the first authorization for a user.
- (void)onAppleSignInSuccess:(NSDictionary<NSString *, id> *)credential;
- (void)onAppleSignInCancel;
- (void)onAppleSignInError:(NSInteger)code message:(NSString *)errorMessage;

- (void)onAppleSignInCredentialState:(NSString *)userId state:(iOSAppleSignInCredentialState)state;
- (void)onAppleSignInCredentialStateError:(NSString *)userId code:(NSInteger)code message:(NSString *)errorMessage;
- (void)onAppleSignInCredentialRevoked:(NSString *)userId;

@end

@protocol iOSAppleSignInInterface <NSObject>

+ (instancetype)sharedInstance;

- (void)setProvider:(id<iOSAppleSignInProviderInterface>)provider;
- (id<iOSAppleSignInProviderInterface>)getProvider;

- (BOOL)isAvailable;
- (BOOL)loginWithEmail:(BOOL)requestEmail
              fullName:(BOOL)requestFullName
                 state:(NSString *)state
                 nonce:(NSString *)nonce;

- (NSString *)getStoredUserId;
- (void)clearStoredCredential;
- (BOOL)checkCredentialState:(NSString *)userId;

// The button frame uses UIKit points relative to the root view.
- (BOOL)showButtonWithType:(iOSAppleSignInButtonType)type
                      style:(iOSAppleSignInButtonStyle)style
                          x:(CGFloat)x
                          y:(CGFloat)y
                      width:(CGFloat)width
                     height:(CGFloat)height
               cornerRadius:(CGFloat)cornerRadius;
- (void)hideButton;
- (BOOL)isButtonVisible;

@end
