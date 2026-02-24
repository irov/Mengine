#pragma once

#import "Environment/Apple/AppleIncluder.h"

@protocol AppleFacebookProviderInterface <NSObject>

- (void)onFacebookLoginSuccess:(NSDictionary<NSString *, NSString *> *)params;
- (void)onFacebookLoginCancel;

- (void)onFacebookError:(NSInteger)code message:(NSString *)errorMessage;

- (void)onFacebookShareSuccess:(NSString *)postId;
- (void)onFacebookShareCancel;
- (void)onFacebookShareError:(NSInteger)code message:(NSString *)errorMessage;

- (void)onFacebookProfilePictureLinkGetSuccess:(NSString *)userId pictureURL:(NSString *)pictureURL;
- (void)onFacebookProfilePictureLinkGetError:(NSInteger)code message:(NSString *)errorMessage;

@end

@protocol AppleFacebookInterface <NSObject>

+ (instancetype)sharedInstance;

- (void)setProvider:(id<AppleFacebookProviderInterface>)provider;
- (id<AppleFacebookProviderInterface>)getProvider;

- (BOOL)login:(NSArray<NSString *> *)permissions;
- (void)logout;
- (BOOL)isLoggedIn;
- (NSString * _Nullable)getAccessToken;
- (NSString * _Nullable)getUserId;
- (void)shareLink:(NSString *)link picture:(NSString *)picture;
- (void)getProfilePictureLink;

@end
