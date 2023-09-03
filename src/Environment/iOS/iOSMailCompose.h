#import <MessageUI/MessageUI.h>
#import <UIKit/UIKit.h>

typedef void (^FBlockMailComposeCompletion)(void);

@interface iOSMailCompose : NSObject<MFMailComposeViewControllerDelegate>
+ (BOOL)canSendMail;

- (instancetype)initWithViewController:(UIViewController * _Nonnull)viewController email:(NSString * _Nonnull)email subject:(NSString * _Nonnull)subject message:(NSString * _Nonnull)message completion:(FBlockMailComposeCompletion)completion;

@property(nonatomic, strong) MFMailComposeViewController * _Nullable m_mailCompose;
@property(nonatomic, strong) FBlockMailComposeCompletion _Nullable m_completion;
@end
