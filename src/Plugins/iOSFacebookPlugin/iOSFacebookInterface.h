#pragma once

#import "Environment/Apple/AppleIncluder.h"

@protocol iOSFacebookProviderInterface <NSObject>

- (void)onFacebookLoginSuccess:(NSDictionary<NSString *, NSString *> *)params;
- (void)onFacebookLoginCancel;

- (void)onFacebookError:(NSInteger)code message:(NSString *)errorMessage;

- (void)onFacebookShareSuccess:(NSString *)postId;
- (void)onFacebookShareCancel;
- (void)onFacebookShareError:(NSInteger)code message:(NSString *)errorMessage;

- (void)onFacebookProfilePictureLinkGetSuccess:(NSString *)userId pictureURL:(NSString *)pictureURL;
- (void)onFacebookProfilePictureLinkGetError:(NSInteger)code message:(NSString *)errorMessage;

@end

@protocol iOSFacebookInterface <NSObject>

+ (instancetype)sharedInstance;

- (void)setProvider:(id<iOSFacebookProviderInterface>)provider;
- (id<iOSFacebookProviderInterface>)getProvider;

- (BOOL)login:(NSArray<NSString *> *)permissions;
- (void)logout;
- (BOOL)isLoggedIn;
- (NSString *)getAccessToken;
- (NSString *)getUserId;
- (void)shareLink:(NSString *)link picture:(NSString *)picture;
- (void)getProfilePictureLink;

@end
