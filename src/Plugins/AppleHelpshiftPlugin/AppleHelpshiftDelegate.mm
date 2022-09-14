#import "AppleHelpshiftDelegate.h"

#include "Environment/Apple/AppleUtils.h"

#include "Kernel/Logger.h"

@implementation AppleHelpshiftDelegate

#pragma mark -

@synthesize m_service;

- (instancetype _Nonnull)initWithService: (Mengine::AppleHelpshiftServiceInterface* _Nonnull) service {
    self = [super init];
    
    self.m_service = service;
    
    return self;
}

- (void) handleHelpshiftEvent:(nonnull NSString *)eventName withData:(nullable NSDictionary *)data{
    if([eventName isEqualToString:HelpshiftEventNameSessionStarted]) {
        NSLog(@"Helpshift session started.");
    }
    
    if([eventName isEqualToString:HelpshiftEventNameSessionEnded]) {
        NSLog(@"Helpshift session ended.");
    }
    
    if([eventName isEqualToString:HelpshiftEventNameReceivedUnreadMessageCount] && data != nullptr) {
        int count = [data[HelpshiftEventDataUnreadMessageCount] intValue];
        NSLog(@"Unread count: %d", [data[HelpshiftEventDataUnreadMessageCount] intValue]);
        NSLog(@"Is unreadCount served from local cache : %d", [data[HelpshiftEventDataUnreadMessageCountIsFromCache] intValue]);
    }
    
    if([eventName isEqualToString:HelpshiftEventNameConversationStatus] && data != nullptr) {
        NSLog(@"Issue ID: %@", data[HelpshiftEventDataLatestIssueId]);
        NSLog(@"Publish ID: %@", data[HelpshiftEventDataLatestIssuePublishId]);
        NSLog(@"Is issue open: %@", data[HelpshiftEventDataIsIssueOpen]);
    }
    
    if([eventName isEqualToString:HelpshiftEventNameWidgetToggle]) {
        NSLog(@"Is chat screen visible: %@", data[HelpshiftEventDataVisible]);
    }
    
    if([eventName isEqualToString:HelpshiftEventNameMessageAdd]) {
        NSLog(@"New message added with body: %@", data[HelpshiftEventDataMessageBody]);
        NSLog(@"New message added with type: %@", data[HelpshiftEventDataMessageType]);
    }
    
    if([eventName isEqualToString:HelpshiftEventNameConversationStart]) {
        NSLog(@"Conversation started with text: %@", data[HelpshiftEventDataMessage]);
    }
    
    if([eventName isEqualToString:HelpshiftEventNameCSATSubmit]) {
        NSLog(@"CSAT Submitted with rating: %@", data[HelpshiftEventDataRating]);
        NSLog(@"CSAT Submitted with feedback: %@", data[HelpshiftEventDataAdditionalFeedback]);
    }
    
    if([eventName isEqualToString:HelpshiftEventNameConversationEnd]) {
        NSLog(@"Conversation ended.");
    }
    
    if([eventName isEqualToString:HelpshiftEventNameConversationRejected]) {
        NSLog(@"Conversation rejected.");
    }
    
    if([eventName isEqualToString:HelpshiftEventNameConversationResolved]) {
        NSLog(@"Conversation resolved.");
    }
    
    if([eventName isEqualToString:HelpshiftEventNameConversationReopened]) {
        NSLog(@"Conversation reopened.");
    }
}


- (void) authenticationFailedForUserWithReason:(HelpshiftAuthenticationFailureReason)reason{
    
    switch (reason) {
        case HelpshiftAuthenticationFailureReasonInvalidAuthToken:
            NSLog(@"Helpshift authentication Failed -> HelpshiftAuthenticationFailureReasonInvalidAuthToken");
            break;
        case HelpshiftAuthenticationFailureReasonAuthTokenNotProvided:
            NSLog(@"Helpshift authentication Failed -> HelpshiftAuthenticationFailureReasonAuthTokenNotProvided");
            break;
        default:
            break;
    }
}

@end
