#import "AppleHelpshiftDelegate.h"

#import "Environment/Apple/AppleDetail.h"

#import "Environment/iOS/iOSLog.h"

@implementation AppleHelpshiftDelegate

#pragma mark -

@synthesize m_helpshift;

- (instancetype _Nonnull)initWithHelpshift:(id<AppleHelpshiftInterface>)helpshift {
    self = [super init];
    
    self.m_helpshift = helpshift;
    
    return self;
}

- (void) handleHelpshiftEvent:(nonnull NSString *)eventName withData:(nullable NSDictionary *)data {
    id<AppleHelpshiftProviderInterface> provider = [self.m_helpshift getProvider];

    if( provider == nil )
    {
        return;
    }
    
    if([eventName isEqualToString:HelpshiftEventNameSessionStarted]) {
        IOS_LOGGER_MESSAGE(@"session started.");
        
        [AppleDetail addMainQueueOperation:^{
            [provider sessionStarted];
        }];
    } else if([eventName isEqualToString:HelpshiftEventNameSessionEnded]) {
        IOS_LOGGER_MESSAGE(@"session ended.");
        
        [AppleDetail addMainQueueOperation:^{
            [provider sessionEnded];
        }];
    } else if([eventName isEqualToString:HelpshiftEventNameReceivedUnreadMessageCount]) {
        NSInteger count = [data[HelpshiftEventDataUnreadMessageCount] integerValue];
        NSInteger countInCache = [data[HelpshiftEventDataUnreadMessageCountIsFromCache] integerValue];
        
        IOS_LOGGER_MESSAGE(@"unread count: %ld unreadCount from cache %ld", (long)count, (long)countInCache);
        
        [AppleDetail addMainQueueOperation:^{
            [provider receivedUnreadMessage:count countInCache:countInCache];
        }];
    } else if([eventName isEqualToString:HelpshiftEventNameConversationStatus]) {
        NSString * issueId = [self convertToString:[data objectForKey:HelpshiftEventDataLatestIssueId]];
        NSString * publishId = [self convertToString:[data objectForKey:HelpshiftEventDataLatestIssuePublishId]];
        bool issueOpen = [data[HelpshiftEventDataIsIssueOpen] boolValue];
                
        IOS_LOGGER_MESSAGE(@"issue id: %@, publish id: %@, is issue open: %d", issueId, publishId, issueOpen );
        
        [AppleDetail addMainQueueOperation:^{
            [provider conversationStatus:issueId publishId:publishId issueOpen:issueOpen];
        }];
    } else if([eventName isEqualToString:HelpshiftEventNameWidgetToggle]) {
        bool visible = [data[HelpshiftEventDataVisible] boolValue];
        
        IOS_LOGGER_MESSAGE(@"is chat screen visible: %d", visible);
        
        [AppleDetail addMainQueueOperation:^{
            [provider eventWidgetToggle:visible];
        }];
    } else if([eventName isEqualToString:HelpshiftEventNameMessageAdd]) {
        NSString * body = [self convertToString:[data objectForKey:HelpshiftEventDataMessageBody]];
        NSString * type = [self convertToString:[data objectForKey:HelpshiftEventDataMessageType]];
        
        IOS_LOGGER_MESSAGE(@"new message added with body: %@, with type: %@", body, type);
        
        [AppleDetail addMainQueueOperation:^{
            [provider eventMessageAdd:body type:type];
        }];
    } else if([eventName isEqualToString:HelpshiftEventNameConversationStart]) {
        NSString * text = [self convertToString:[data objectForKey:HelpshiftEventDataMessage]];
        IOS_LOGGER_MESSAGE(@"conversation started with text: %@", text);
        
        [AppleDetail addMainQueueOperation:^{
            [provider converstationStart:text];
        }];
    } else if([eventName isEqualToString:HelpshiftEventNameCSATSubmit]) {
        NSString * rating = [self convertToString:[data objectForKey:HelpshiftEventDataRating]];
        NSString * feedback = [self convertToString:[data objectForKey:HelpshiftEventDataAdditionalFeedback]];
        
        IOS_LOGGER_MESSAGE(@"CSAT Submitted with rating: %@, with feedback: %@", rating, feedback);
        
        [AppleDetail addMainQueueOperation:^{
            [provider eventCSATSubmit:rating feedback:feedback];
        }];
    } else if([eventName isEqualToString:HelpshiftEventNameConversationEnd]) {
        IOS_LOGGER_MESSAGE(@"conversation ended.");
        
        [AppleDetail addMainQueueOperation:^{
            [provider converstationEnded];
        }];
    } else if([eventName isEqualToString:HelpshiftEventNameConversationRejected]) {
        IOS_LOGGER_MESSAGE(@"conversation rejected.");
        
        [AppleDetail addMainQueueOperation:^{
            [provider converstationRejected];
        }];
    } else if([eventName isEqualToString:HelpshiftEventNameConversationResolved]) {
        IOS_LOGGER_MESSAGE(@"conversation resolved.");
        
        [AppleDetail addMainQueueOperation:^{
            [provider converstationResolved];
        }];
    } else if([eventName isEqualToString:HelpshiftEventNameConversationReopened]) {
        IOS_LOGGER_MESSAGE(@"conversation reopened.");
        
        [AppleDetail addMainQueueOperation:^{
            [provider converstationReopen];
        }];
    }
}


- (void) authenticationFailedForUserWithReason:(HelpshiftAuthenticationFailureReason)reason{
    id<AppleHelpshiftProviderInterface> provider = [self.m_helpshift getProvider];

    if( provider == nil )
    {
        return;
    }
    
    switch (reason)
    {
    case HelpshiftAuthenticationFailureReasonInvalidAuthToken:
        {
            IOS_LOGGER_MESSAGE(@"authentication Failed - InvalidAuthToken");
            
            [AppleDetail addMainQueueOperation:^{
                [provider authenticationInvalidAuthToken];
            }];
        }break;
    case HelpshiftAuthenticationFailureReasonAuthTokenNotProvided:
        {
            IOS_LOGGER_MESSAGE(@"authentication Failed - AuthTokenNotProvided");
            
            [AppleDetail addMainQueueOperation:^{
                [provider authenticationTokenNotProvided];
            }];
        }break;
    default:
        {
            IOS_LOGGER_MESSAGE(@"authentication Failed - UNKNOWN");
            
            [AppleDetail addMainQueueOperation:^{
                [provider authenticationUnknownError];
            }];
        }break;
    }
}

- (NSString * _Nonnull) convertToString:(nullable NSObject*)object {
    if (object == nil) {
        return @"empty";
    }
    
    NSString * value = [NSString stringWithFormat:@"%@", object];
    
    return value;
}

@end
