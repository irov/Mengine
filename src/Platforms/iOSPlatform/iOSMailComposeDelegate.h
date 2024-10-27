#pragma once

#include "Config/Config.h"

#import <MessageUI/MessageUI.h>
#import <UIKit/UIKit.h>

typedef void (^FBlockMailComposeCompletion)(void);

@interface iOSMailComposeDelegate : NSObject<MFMailComposeViewControllerDelegate>
- (instancetype)initWithCompletion:(FBlockMailComposeCompletion)completion;
- (void)dealloc;

@property(nonatomic, strong) FBlockMailComposeCompletion m_completion;
@end
