#import "iOSMailCompose.h"

#include "Kernel/Logger.h"

@implementation iOSMailCompose

+ (BOOL)canSendMail {
    BOOL result = [MFMailComposeViewController canSendMail];

    return result;
}

- (instancetype)initWithViewController:(UIViewController * _Nonnull)viewController email:(NSString * _Nonnull)email subject:(NSString * _Nonnull)subject message:(NSString * _Nonnull)message completion:(FBlockMailComposeCompletion _Nonnull)completion {
    self = [super init];

    MFMailComposeViewController * mailCompose = [[MFMailComposeViewController alloc] init];
    
    [mailCompose setModalPresentationStyle:UIModalPresentationFormSheet];
    
    [mailCompose setMailComposeDelegate:self];

    [mailCompose setToRecipients:[NSArray arrayWithObjects:email, nil]];
    [mailCompose setSubject:subject];
    [mailCompose setMessageBody:message isHTML:NO];

    [viewController presentViewController:mailCompose animated:YES completion:^{
    }];

    self.m_mailCompose = mailCompose;
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
            LOGGER_MESSAGE( "MFMailComposeResultFailed: The email message was not saved or queued, possibly due to an error [%s]"
                , [[error localizedDescription] UTF8String]
            );
            break;
        default:
            break;
    }

    [self.m_mailCompose dismissViewControllerAnimated:YES completion:^{
        self.m_completion();
    }];
}

@end
