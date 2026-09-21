#import "iOSToastView.h"

static constexpr NSTimeInterval TOAST_DURATION = 2.0;
static constexpr NSTimeInterval TOAST_FADE_IN = 0.15;
static constexpr NSTimeInterval TOAST_FADE_OUT = 0.2;
static constexpr CGFloat TOAST_MARGIN = 24.0;
static constexpr CGFloat TOAST_BOTTOM = 48.0;
static constexpr CGFloat TOAST_PADDING_X = 20.0;
static constexpr CGFloat TOAST_PADDING_Y = 12.0;
static constexpr CGFloat TOAST_MAX_WIDTH = 360.0;

@interface iOSToastView ()
- (void)showMessage_:(NSString *)message;
- (void)dismiss_;
- (void)applicationDidEnterBackground_:(NSNotification *)notification;
@end

@implementation iOSToastView

+ (void)showInView:(UIView *)view message:(NSString *)message {
    iOSToastView * toast = nil;
    for (UIView * child in view.subviews) {
        if ([child isKindOfClass:[iOSToastView class]] == YES) {
            toast = (iOSToastView *)child;
            break;
        }
    }

    if (toast == nil) {
        toast = [[iOSToastView alloc] initWithFrame:view.bounds];
        [view addSubview:toast];
    }

    [view bringSubviewToFront:toast];
    [toast showMessage_:message];
}

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self == nil) {
        return nil;
    }

    self.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    self.userInteractionEnabled = NO;
    self.alpha = 0.0;

    m_panel = [[UIView alloc] initWithFrame:CGRectZero];
    m_panel.backgroundColor = [UIColor colorWithWhite:0.12 alpha:0.94];
    m_panel.layer.cornerRadius = 16.0;
    [self addSubview:m_panel];

    m_label = [[UILabel alloc] initWithFrame:CGRectZero];
    m_label.font = [UIFont preferredFontForTextStyle:UIFontTextStyleSubheadline];
    m_label.adjustsFontForContentSizeCategory = YES;
    m_label.textColor = UIColor.whiteColor;
    m_label.numberOfLines = 0;
    m_label.textAlignment = NSTextAlignmentCenter;
    [m_panel addSubview:m_label];

    NSNotificationCenter * notifications = [NSNotificationCenter defaultCenter];
    [notifications addObserver:self selector:@selector(applicationDidEnterBackground_:) name:UIApplicationDidEnterBackgroundNotification object:nil];

    return self;
}

- (void)dealloc {
    [m_timer invalidate];
    NSNotificationCenter * notifications = [NSNotificationCenter defaultCenter];
    [notifications removeObserver:self];
}

- (void)layoutSubviews {
    [super layoutSubviews];

    CGRect area = UIEdgeInsetsInsetRect(self.bounds, self.safeAreaInsets);
    CGFloat width = MIN(TOAST_MAX_WIDTH, MAX(0.0, area.size.width - TOAST_MARGIN * 2.0));
    CGSize available = CGSizeMake(MAX(0.0, width - TOAST_PADDING_X * 2.0), CGFLOAT_MAX);
    CGSize textSize = [m_label sizeThatFits:available];
    CGFloat panelWidth = MIN(width, textSize.width + TOAST_PADDING_X * 2.0);
    CGFloat panelHeight = textSize.height + TOAST_PADDING_Y * 2.0;
    CGFloat x = CGRectGetMidX(area) - panelWidth * 0.5;
    CGFloat y = MAX(CGRectGetMinY(area), CGRectGetMaxY(area) - TOAST_BOTTOM - panelHeight);
    m_panel.frame = CGRectMake(x, y, panelWidth, panelHeight);
    m_label.frame = CGRectMake(TOAST_PADDING_X, TOAST_PADDING_Y, MAX(0.0, panelWidth - TOAST_PADDING_X * 2.0), textSize.height);
}

- (void)showMessage_:(NSString *)message {
    [m_timer invalidate];
    m_label.text = message;
    [self setNeedsLayout];
    [self layoutIfNeeded];

    UIViewAnimationOptions options = UIViewAnimationOptionBeginFromCurrentState | UIViewAnimationOptionAllowUserInteraction | UIViewAnimationOptionCurveEaseOut;
    [UIView animateWithDuration:TOAST_FADE_IN delay:0.0 options:options animations:^{
        self.alpha = 1.0;
    } completion:nil];

    __weak iOSToastView * weakSelf = self;
    m_timer = [NSTimer scheduledTimerWithTimeInterval:TOAST_DURATION repeats:NO block:^(NSTimer * timer) {
        (void)timer;
        [weakSelf dismiss_];
    }];
    UIAccessibilityPostNotification(UIAccessibilityAnnouncementNotification, message);
}

- (void)dismiss_ {
    [m_timer invalidate];
    m_timer = nil;
    UIViewAnimationOptions options = UIViewAnimationOptionBeginFromCurrentState | UIViewAnimationOptionAllowUserInteraction | UIViewAnimationOptionCurveEaseIn;
    [UIView animateWithDuration:TOAST_FADE_OUT delay:0.0 options:options animations:^{
        self.alpha = 0.0;
    } completion:^(BOOL finished) {
        if (finished == YES && self.alpha == 0.0) {
            [self removeFromSuperview];
        }
    }];
}

- (void)applicationDidEnterBackground_:(NSNotification *)notification {
    (void)notification;
    [m_timer invalidate];
    m_timer = nil;
    [self removeFromSuperview];
}

- (void)didMoveToWindow {
    [super didMoveToWindow];
    if (self.window == nil) {
        [m_timer invalidate];
        m_timer = nil;
    }
}

@end
