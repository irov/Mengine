#import "iOSViewController.h"

#include "Interface/PlatformServiceInterface.h"

#include "Environment/iOS/iOSPlatformServiceExtensionInterface.h"

@interface iOSViewController ()

@property (nonatomic, strong) UIViewController * m_launchScreenViewController;
@property (nonatomic, assign) CGRect m_safeAreaViewport;
@property (nonatomic, assign) BOOL m_safeAreaViewportValid;
@property (nonatomic, copy) iOSSafeAreaViewportChangedCallback safeAreaViewportChangedCallback;

- (void)updateSafeAreaViewport_;

@end

@implementation iOSViewController

- (void)viewSafeAreaInsetsDidChange {
    [super viewSafeAreaInsetsDidChange];

    [self updateSafeAreaViewport_];
}

- (void)viewDidLayoutSubviews {
    [super viewDidLayoutSubviews];

    [self updateSafeAreaViewport_];
}

- (void)updateSafeAreaViewport_ {
    CGRect viewport;
    if ([self getSafeAreaViewport:&viewport] == NO) {
        self.m_safeAreaViewportValid = NO;
        return;
    }

    if (self.m_safeAreaViewportValid == YES && CGRectEqualToRect(self.m_safeAreaViewport, viewport) == YES) {
        return;
    }

    self.m_safeAreaViewport = viewport;
    self.m_safeAreaViewportValid = YES;

    if (self.safeAreaViewportChangedCallback != nil) {
        self.safeAreaViewportChangedCallback();
    }
}

- (BOOL)getSafeAreaViewport:(CGRect * _Nonnull)viewport {
    UIView * view = self.viewIfLoaded;

    if (view == nil) {
        return NO;
    }

    UIEdgeInsets viewInsets = view.safeAreaInsets;
    CGFloat scale = view.contentScaleFactor;
    CGSize viewSize = view.bounds.size;

    CGFloat beginX = viewInsets.left * scale;
    CGFloat beginY = viewInsets.top * scale;
    CGFloat endX = (viewSize.width - viewInsets.right) * scale;
    CGFloat endY = (viewSize.height - viewInsets.bottom) * scale;

    *viewport = CGRectMake(beginX, beginY, endX - beginX, endY - beginY);

    return YES;
}

- (void)setView:(UIView *)view {
    [super setView:view];

    UIView * launchScreenView = self.m_launchScreenViewController.viewIfLoaded;

    if (launchScreenView == nil) {
        return;
    }

    if (launchScreenView == view) {
        return;
    }

    launchScreenView.frame = view.bounds;
    launchScreenView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;

    [view addSubview:launchScreenView];
}

- (void)showLaunchScreen {
    NSString * launchStoryboardName = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"UILaunchStoryboardName"];

    if (launchStoryboardName.length == 0) {
        return;
    }

    UIStoryboard * launchStoryboard = [UIStoryboard storyboardWithName:launchStoryboardName bundle:nil];
    UIViewController * launchViewController = [launchStoryboard instantiateInitialViewController];

    if (launchViewController == nil) {
        return;
    }

    UIView * launchScreenView = launchViewController.view;
    launchScreenView.frame = UIScreen.mainScreen.bounds;
    launchScreenView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;

    UIView * containerView = [[UIView alloc] initWithFrame:UIScreen.mainScreen.bounds];
    containerView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;

    self.view = containerView;

    [self addChildViewController:launchViewController];
    [containerView addSubview:launchScreenView];
    [launchViewController didMoveToParentViewController:self];

    self.m_launchScreenViewController = launchViewController;
}

- (BOOL)dismissLaunchScreen {
    UIViewController * launchViewController = self.m_launchScreenViewController;

    if (launchViewController == nil) {
        return NO;
    }

    [launchViewController willMoveToParentViewController:nil];
    [launchViewController.view removeFromSuperview];
    [launchViewController removeFromParentViewController];

    self.m_launchScreenViewController = nil;

    return YES;
}

- (BOOL)prefersStatusBarHidden {
    return YES;
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    MENGINE_UNUSED( event );
    
    Mengine::iOSPlatformServiceExtensionInterface * extension = PLATFORM_SERVICE()
        ->getDynamicUnknown();
    
    if( extension != nullptr )
    {
        extension->handleTouchBegan( touches, self.view );
    }
}

- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    MENGINE_UNUSED( event );
    
    Mengine::iOSPlatformServiceExtensionInterface * extension = PLATFORM_SERVICE()
        ->getDynamicUnknown();
    
    if( extension != nullptr )
    {
        extension->handleTouchMoved( touches, self.view );
    }
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    MENGINE_UNUSED( event );
    
    Mengine::iOSPlatformServiceExtensionInterface * extension = PLATFORM_SERVICE()
        ->getDynamicUnknown();
    
    if( extension != nullptr )
    {
        extension->handleTouchEnded( touches, self.view );
    }
}

- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    MENGINE_UNUSED( event );
    
    Mengine::iOSPlatformServiceExtensionInterface * extension = PLATFORM_SERVICE()
        ->getDynamicUnknown();
    
    if( extension != nullptr )
    {
        extension->handleTouchCancelled( touches, self.view );
    }
}

@end
