#pragma once

#include "Config/Config.h"

#import <MessageUI/MessageUI.h>
#import <UIKit/UIKit.h>

typedef void (^FBlockMailComposeCompletion)(void);

@interface iOSMailComposeDelegate : NSObject<MFMailComposeViewControllerDelegate>
- (nullable instancetype)initWithCompletion:(FBlockMailComposeCompletion _Nonnull)completion;

@property(nonatomic, copy, nonnull) FBlockMailComposeCompletion m_completion;
@end
