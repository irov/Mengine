#import "iOSMailCompose.h"

#include "Kernel/Logger.h"

@implementation iOSMailCompose

+ (BOOL)canSendMail {
    BOOL result = [MFMailComposeViewController canSendMail];

    return result;
}

- (instancetype)initWithViewController:(UIViewController *)viewController {
    self = [super init];

    MFMailComposeViewController * mailCompose = [[MFMailComposeViewController alloc] init];
    
    mailCompose.mailComposeDelegate = self;

    [mailCompose setSubject:@(_subject)];
    [mailCompose setMessageBody:@(_body) isHTML:NO];
    [mailCompose setToRecipients:[NSArray arrayWithObjects:@(_email), nil]];

    // displaying our modal view controller on the screen (of course animated has to be set on YES if you want to see any transition)
    [viewController presentViewController:mailCompose animated:YES completion:nil];

    self.m_mailCompose = mailCompose;

    return self;
}

- (void)mailComposeController:(MFMailComposeViewController *)controller didFinishWithResult:(MFMailComposeResult)result error:(nullable NSError *)error {
    switch(result) {
        case MFMailComposeResultCancelled:
            LOGGER_MESSAGE( "MFMailComposeResultCancelled: The user cancelled the operation. No email message was queued" );
            break;
        case MFMailComposeResultSaved:
            LOGGER_MESSAGE( "MFMailComposeResultSaved: The email message was saved in the user’s Drafts folder" );
            break;
        case MFMailComposeResultSent:
            LOGGER_MESSAGE( "MFMailComposeResultSent: The email message was queued in the user’s outbox. It is ready to send the next time the user connects to email" );
            break;
        case MFMailComposeResultFailed:
            LOGGER_MESSAGE( "MFMailComposeResultFailed: The email message was not saved or queued, possibly due to an error [%s]"
                , [[error localizedDescription] UTF8String]
            );
            break;
        default:
            break;
    }

    [self dismissViewControllerAnimated:YES completion:nil];
}

@end