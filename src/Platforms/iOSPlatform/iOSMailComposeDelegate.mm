#import "iOSMailComposeDelegate.h"

#include "Kernel/Logger.h"

@implementation iOSMailComposeDelegate

- (instancetype)initWithCompletion:(FBlockMailComposeCompletion)completion {
    self = [super init];

    self.m_completion = completion;
    
    return self;
}

- (void)mailComposeController:(MFMailComposeViewController *)controller didFinishWithResult:(MFMailComposeResult)result error:(nullable NSError *)error {
    switch(result) {
        case MFMailComposeResultCancelled:
            LOGGER_MESSAGE( "MFMailComposeResultCancelled: The user cancelled the operation. No email message was queued" );
            break;
        case MFMailComposeResultSaved:
            LOGGER_MESSAGE( "MFMailComposeResultSaved: The email message was saved in the useris Drafts folder" );
            break;
        case MFMailComposeResultSent:
            LOGGER_MESSAGE( "MFMailComposeResultSent: The email message was queued in the useris outbox. It is ready to send the next time the user connects to email" );
            break;
        case MFMailComposeResultFailed:
            LOGGER_ERROR( "MFMailComposeResultFailed: The email message was not saved or queued, possibly due to an error [%s]"
                , [[error localizedDescription] UTF8String]
            );
            break;
        default:
            break;
    }

    [controller dismissViewControllerAnimated:YES completion:^{
        self.m_completion();
    }];
}

@end
