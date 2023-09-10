#import <MessageUI/MessageUI.h>
#import <UIKit/UIKit.h>

typedef void (^FBlockMailComposeCompletion)(void);

@interface iOSMailCompose : NSObject<MFMailComposeViewControllerDelegate>
+ (BOOL)canSendMail;

- (instancetype)initWithViewController:(UIViewController *)viewController email:(NSString *)email subject:(NSString *)subject message:(NSString *)message completion:(FBlockMailComposeCompletion)completion;

@property(nonatomic, strong) MFMailComposeViewController * m_mailCompose;
@property(nonatomic, strong) FBlockMailComposeCompletion m_completion;
@end
