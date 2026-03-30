#import "iOSMailComposeDelegate.h"

#import "Environment/iOS/iOSDetail.h"
#import "Environment/iOS/iOSLog.h"

@implementation iOSMailComposeDelegate

- (instancetype)initWithCompletion:(FBlockMailComposeCompletion _Nonnull)completion {
    self = [super init];
    
    self.m_completion = completion;
    
    return self;
}

- (void)mailComposeController:(MFMailComposeViewController *)controller didFinishWithResult:(MFMailComposeResult)result error:(nullable NSError *)error {
    NSString * alertTitle = nil;
    NSString * alertMessage = nil;

    switch(result) {
        case MFMailComposeResultCancelled:
            IOS_LOGGER_MESSAGE( @"MFMailComposeResultCancelled: The user cancelled the operation. No email message was queued" );
            break;
        case MFMailComposeResultSaved:
            IOS_LOGGER_MESSAGE( @"MFMailComposeResultSaved: The email message was saved in the useris Drafts folder" );
            alertTitle = @"Draft Saved";
            alertMessage = @"The email message was saved in Drafts.";
            break;
        case MFMailComposeResultSent:
            IOS_LOGGER_MESSAGE( @"MFMailComposeResultSent: The email message was queued in the useris outbox. It is ready to send the next time the user connects to email" );
            alertTitle = @"Email Queued";
            alertMessage = @"The message was added to the Outbox. It will be sent when the Mail app connects to the email service.";
            break;
        case MFMailComposeResultFailed:
            IOS_LOGGER_MESSAGE( @"MFMailComposeResultFailed: The email message was not saved or queued, possibly due to an error: %@"
                , [error description]
            );

            alertTitle = @"Send Failed";

            if( error != nil )
            {
                alertMessage = [NSString stringWithFormat:@"The email message could not be queued. %@", [error localizedDescription]];
            }
            else
            {
                alertMessage = @"The email message could not be queued.";
            }

            break;
        default:
            break;
    }

    FBlockMailComposeCompletion completion = self.m_completion;

    [controller dismissViewControllerAnimated:YES completion:^ {
        if( alertTitle != nil && alertMessage != nil )
        {
            [iOSDetail showOkAlertWithTitle:alertTitle
                                    message:alertMessage
                                         ok:^ {
                completion();
            }];

            return;
        }

        completion();
    }];
}

@end
