#import <MessageUI/MessageUI.h>
#import <UIKit/UIKit.h>

@interface iOSMailCompose : NSObject<MFMailComposeViewControllerDelegate>
+ (BOOL)canSendMail;

- (instancetype)initWithViewController:(UIViewController *)viewController;

@property( nonatomic, strong ) MARewardedAd * _Nullable m_mailCompose;
@end