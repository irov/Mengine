#import "AppleHelpshiftDelegate.h"

#include "Environment/Apple/AppleUtils.h"

#include "Kernel/Logger.h"

@implementation AppleHelpshiftDelegate

#pragma mark -

@synthesize m_service;

- (instancetype _Nonnull)initWithService: (Mengine::AppleHelpshiftServiceInterface * _Nonnull) service {
    self = [super init];
    
    self.m_service = service;
    
    return self;
}

- (void) handleHelpshiftEvent:(nonnull NSString *)eventName withData:(nullable NSDictionary *)data {
    const Mengine::AppleHelpshiftProviderInterfacePtr & provider = self.m_service->getProvider();
    
    if([eventName isEqualToString:HelpshiftEventNameSessionStarted]) {
        LOGGER_MESSAGE("session started.");
        
        provider->sessionStarted();
    } else if([eventName isEqualToString:HelpshiftEventNameSessionEnded]) {
        LOGGER_MESSAGE("session ended.");
        
        provider->sessionEnded();
    } else if([eventName isEqualToString:HelpshiftEventNameReceivedUnreadMessageCount]) {
        int count = [data[HelpshiftEventDataUnreadMessageCount] intValue];
        int countInCache = [data[HelpshiftEventDataUnreadMessageCountIsFromCache] intValue];
        
        LOGGER_MESSAGE("unread count: %i\nunreadCount from cache %i", count, countInCache);
        
        provider->receivedUnreadMessage( count, countInCache );
    } else if([eventName isEqualToString:HelpshiftEventNameConversationStatus]) {
        const char * issueId = [self convertToChar:[data objectForKey:HelpshiftEventDataLatestIssueId]];
        const char * publishId = [self convertToChar:[data objectForKey:HelpshiftEventDataLatestIssuePublishId]];
        bool issueOpen = [data[HelpshiftEventDataIsIssueOpen] boolValue];
                
        LOGGER_MESSAGE("issue id: %s, publish id: %s, is issue open: %d", issueId, publishId, issueOpen );
        
        provider->conversationStatus( issueId, publishId, issueOpen );
    } else if([eventName isEqualToString:HelpshiftEventNameWidgetToggle]) {
        bool visible = [data[HelpshiftEventDataVisible] boolValue];
        
        LOGGER_MESSAGE("is chat screen visible: %d", visible);
        
        provider->eventWidgetToggle( visible );
    } else if([eventName isEqualToString:HelpshiftEventNameMessageAdd]) {
        const char * body = [self convertToChar:[data objectForKey:HelpshiftEventDataMessageBody]];
        const char * type = [self convertToChar:[data objectForKey:HelpshiftEventDataMessageType]];
        
        LOGGER_MESSAGE("new message added with body: %s, with type: %s", body, type );
        
        provider->eventMessageAdd( body, type );
    } else if([eventName isEqualToString:HelpshiftEventNameConversationStart]) {
        const char * text = [self convertToChar:[data objectForKey:HelpshiftEventDataMessage]];
        LOGGER_MESSAGE("conversation started with text: %s", text);
        
        provider->converstationStart( text );
    } else if([eventName isEqualToString:HelpshiftEventNameCSATSubmit]) {
        const char * rating = [self convertToChar:[data objectForKey:HelpshiftEventDataRating]];
        const char * feedback = [self convertToChar:[data objectForKey:HelpshiftEventDataAdditionalFeedback]];
        
        LOGGER_MESSAGE("CSAT Submitted with rating: %s, with feedback: %s",rating, feedback);
        
        provider->eventCSATSubmit( rating, feedback );
    } else if([eventName isEqualToString:HelpshiftEventNameConversationEnd]) {
        LOGGER_MESSAGE("conversation ended.");
        
        provider->converstationEnded();
    } else if([eventName isEqualToString:HelpshiftEventNameConversationRejected]) {
        LOGGER_MESSAGE("conversation rejected.");
        
        provider->converstationRejected();
    } else if([eventName isEqualToString:HelpshiftEventNameConversationResolved]) {
        LOGGER_MESSAGE("conversation resolved.");
        
        provider->converstationResolved();
    } else if([eventName isEqualToString:HelpshiftEventNameConversationReopened]) {
        LOGGER_MESSAGE("conversation reopened.");
        
        provider->converstationReopen();
    }
}


- (void) authenticationFailedForUserWithReason:(HelpshiftAuthenticationFailureReason)reason{
    const Mengine::AppleHelpshiftProviderInterfacePtr & provider = self.m_service->getProvider();
    
    switch (reason)
    {
    case HelpshiftAuthenticationFailureReasonInvalidAuthToken:
        {
            LOGGER_MESSAGE("authentication Failed - InvalidAuthToken");
            
            provider->authenticationInvalidAuthToken();
        }break;
    case HelpshiftAuthenticationFailureReasonAuthTokenNotProvided:
        {
            LOGGER_MESSAGE("authentication Failed - AuthTokenNotProvided");
            
            provider->authenticationTokenNotProvided();
        }break;
    default:
        {
            LOGGER_MESSAGE("authentication Failed - UNKNOWN");
            
            provider->authenticationUnknownError();
        }break;
    }
}

- (const char * _Nonnull) convertToChar:(nullable NSObject*)object{
    if (object == nullptr) {
        return "empty";
    }
    
    NSString * value = [NSString stringWithString:[NSString stringWithFormat:@"%@",object]];
    
    return [value UTF8String];
}

@end
