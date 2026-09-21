#pragma once

#import <UIKit/UIKit.h>

@interface iOSToastView : UIView {
@private
    UIView * m_panel;
    UILabel * m_label;
    NSTimer * m_timer;
}

+ (void)showInView:(UIView * _Nonnull)view message:(NSString * _Nonnull)message;

@end
