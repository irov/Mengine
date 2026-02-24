#pragma once

#import "Environment/Apple/AppleIncluder.h"

@protocol AppleHelpshiftProviderInterface <NSObject>

- (void)sessionStarted;
- (void)sessionEnded;
- (void)receivedUnreadMessage:(NSInteger)count countInCache:(NSInteger)countInCache;
- (void)conversationStatus:(NSString *)issueId publishId:(NSString *)publishId issueOpen:(BOOL)issueOpen;
- (void)eventWidgetToggle:(BOOL)visible;
- (void)eventMessageAdd:(NSString *)body type:(NSString *)type;
- (void)converstationStart:(NSString *)text;
- (void)eventCSATSubmit:(NSString *)rating feedback:(NSString *)feedback;
- (void)converstationEnded;
- (void)converstationRejected;
- (void)converstationResolved;
- (void)converstationReopen;

- (void)authenticationInvalidAuthToken;
- (void)authenticationTokenNotProvided;
- (void)authenticationUnknownError;

@end

@protocol AppleHelpshiftInterface <NSObject>

+ (instancetype)sharedInstance;

- (void)setProvider:(id<AppleHelpshiftProviderInterface>)provider;
- (id<AppleHelpshiftProviderInterface>)getProvider;

- (void)showFAQs;
- (void)showConversation;
- (void)showFAQSection:(NSString *)sectionId;
- (void)showSingleFAQ:(NSString *)faqId;
- (void)setLanguage:(NSString *)language;

@end
