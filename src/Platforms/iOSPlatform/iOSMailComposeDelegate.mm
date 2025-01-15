#import "iOSMailComposeDelegate.h"

#import "Environment/iOS/iOSLog.h"

@implementation iOSMailComposeDelegate

- (instancetype)initWithCompletion:(FBlockMailComposeCompletion)completion {
    self = [super init];
    
    self.m_completion = completion;
    
    return self;
}

- (void)dealloc {
    self.m_completion = nil;
}

- (void)mailComposeController:(MFMailComposeViewController *)controller didFinishWithResult:(MFMailComposeResult)result error:(nullable NSError *)error {
    switch(result) {
        case MFMailComposeResultCancelled:
            IOS_LOGGER_MESSAGE( @"MFMailComposeResultCancelled: The user cancelled the operation. No email message was queued" );
            break;
        case MFMailComposeResultSaved:
            IOS_LOGGER_MESSAGE( @"MFMailComposeResultSaved: The email message was saved in the useris Drafts folder" );
            break;
        case MFMailComposeResultSent:
            IOS_LOGGER_MESSAGE( @"MFMailComposeResultSent: The email message was queued in the useris outbox. It is ready to send the next time the user connects to email" );
            break;
        case MFMailComposeResultFailed:
            IOS_LOGGER_MESSAGE( @"MFMailComposeResultFailed: The email message was not saved or queued, possibly due to an error: %@"
                , [error description]
            );
            break;
        default:
            break;
    }

    [controller dismissViewControllerAnimated:YES completion:^ {
        self.m_completion();
    }];
}

@end
