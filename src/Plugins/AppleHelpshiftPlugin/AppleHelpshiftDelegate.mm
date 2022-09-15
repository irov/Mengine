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
        LOGGER_MESSAGE("Helpshift session started.");
        
        self.m_service->getProvider()->sessionStarted();
    }
    
    if([eventName isEqualToString:HelpshiftEventNameSessionEnded]) {
        LOGGER_MESSAGE("Helpshift session ended.");
        
        self.m_service->getProvider()->sessionEnded();
    }
    
    if([eventName isEqualToString:HelpshiftEventNameReceivedUnreadMessageCount]) {
        int count = [data[HelpshiftEventDataUnreadMessageCount] intValue];
        int countInCache = [data[HelpshiftEventDataUnreadMessageCountIsFromCache] intValue];
        
        LOGGER_MESSAGE("Unread count: %i\nunreadCount from cache %i", count, countInCache);
        
        self.m_service->getProvider()->receivedUnreadMessage(count, countInCache);
    }
    
    if([eventName isEqualToString:HelpshiftEventNameConversationStatus]) {
        
        const char * issueId = [self convertToChar:[data objectForKey:HelpshiftEventDataLatestIssueId]];
        const char * publishId = [self convertToChar:[data objectForKey:HelpshiftEventDataLatestIssuePublishId]];
        bool issueOpen =[data[HelpshiftEventDataIsIssueOpen] boolValue];
                
        LOGGER_MESSAGE("Issue ID: %s, Publish ID: %s, Is issue open: %d", issueId, publishId, issueOpen );
        
        self.m_service->getProvider()->conversationStatus(issueId, publishId, issueOpen);
    }
    
    if([eventName isEqualToString:HelpshiftEventNameWidgetToggle]) {
        bool visible = [self convertToChar:[data objectForKey:HelpshiftEventDataVisible]];
        
        LOGGER_MESSAGE("Is chat screen visible: %d", visible);
        
        self.m_service->getProvider()->eventWidgetToggle(visible);
    }
    
    if([eventName isEqualToString:HelpshiftEventNameMessageAdd]) {
        const char * body = [self convertToChar:[data objectForKey:HelpshiftEventDataMessageBody]];
        const char * type = [self convertToChar:[data objectForKey:HelpshiftEventDataMessageType]];
        
        LOGGER_MESSAGE("New message added with body: %s, with type: %s", body, type );
        
        self.m_service->getProvider()->eventMessageAdd(body, type);
    }
    
    if([eventName isEqualToString:HelpshiftEventNameConversationStart]) {
        const char * text = [self convertToChar:[data objectForKey:HelpshiftEventDataMessage]];
        LOGGER_MESSAGE("Conversation started with text: %s", text);
        
        self.m_service->getProvider()->converstationStart(text);
    }
    
    if([eventName isEqualToString:HelpshiftEventNameCSATSubmit]) {
        const char * rating = [self convertToChar:[data objectForKey:HelpshiftEventDataRating]];
        const char * feedback = [self convertToChar:[data objectForKey:HelpshiftEventDataAdditionalFeedback]];
        
        LOGGER_MESSAGE("CSAT Submitted with rating: %s, with feedback: %s",rating, feedback);
        
        self.m_service->getProvider()->eventCSATSubmit(rating, feedback);
    }
    
    if([eventName isEqualToString:HelpshiftEventNameConversationEnd]) {
        LOGGER_MESSAGE("Conversation ended.");
        
        self.m_service->getProvider()->converstationEnded();
    }
    
    if([eventName isEqualToString:HelpshiftEventNameConversationRejected]) {
        LOGGER_MESSAGE("Conversation rejected.");
        
        self.m_service->getProvider()->converstationRejected();
    }
    
    if([eventName isEqualToString:HelpshiftEventNameConversationResolved]) {
        LOGGER_MESSAGE("Conversation resolved.");
        
        self.m_service->getProvider()->converstationResolved();
    }
    
    if([eventName isEqualToString:HelpshiftEventNameConversationReopened]) {
        LOGGER_MESSAGE("Conversation reopened.");
        
        self.m_service->getProvider()->converstationReopen();
    }
}


- (void) authenticationFailedForUserWithReason:(HelpshiftAuthenticationFailureReason)reason{
    
    switch (reason) {
        case HelpshiftAuthenticationFailureReasonInvalidAuthToken:
            LOGGER_MESSAGE("Helpshift authentication Failed - InvalidAuthToken");
            
            self.m_service->getProvider()->authenticationInvalidAuthToken();
            break;
        case HelpshiftAuthenticationFailureReasonAuthTokenNotProvided:
            LOGGER_MESSAGE("Helpshift authentication Failed - AuthTokenNotProvided");
            
            self.m_service->getProvider()->authenticationTokenNotProvided();
            break;
        default:
            break;
    }
}


-(const char *)convertToChar:(nullable NSObject*)object{
    if (object == nullptr){
        return "empty";
    }
    NSString *value = [NSString stringWithString:[NSString stringWithFormat:@"%@",object]];
    return [value UTF8String];
}
@end
