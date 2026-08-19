#import "iOSViewController.h"

#include "Interface/PlatformServiceInterface.h"

#include "Environment/iOS/iOSPlatformServiceExtensionInterface.h"

@interface iOSViewController ()

@property (nonatomic, strong) UIViewController * m_launchScreenViewController;
@property (nonatomic, assign) CGRect m_safeAreaViewport;
@property (nonatomic, assign) BOOL m_safeAreaViewportValid;
@property (nonatomic, copy) iOSSafeAreaInsetsDidChangeCallback safeAreaInsetsDidChangeCallback;

@end

@implementation iOSViewController

- (void)viewSafeAreaInsetsDidChange {
    [super viewSafeAreaInsetsDidChange];

    UIEdgeInsets viewInsets = self.view.safeAreaInsets;
    CGFloat scale = self.view.contentScaleFactor;
    CGSize viewSize = self.view.bounds.size;

    CGFloat beginX = viewInsets.left * scale;
    CGFloat beginY = viewInsets.top * scale;
    CGFloat endX = (viewSize.width - viewInsets.right) * scale;
    CGFloat endY = (viewSize.height - viewInsets.bottom) * scale;

    self.m_safeAreaViewport = CGRectMake(beginX, beginY, endX - beginX, endY - beginY);
    self.m_safeAreaViewportValid = YES;

    if (self.safeAreaInsetsDidChangeCallback != nil) {
        self.safeAreaInsetsDidChangeCallback();
    }
}

- (BOOL)getSafeAreaViewport:(CGRect * _Nonnull)viewport {
    if (self.m_safeAreaViewportValid == NO) {
        return NO;
    }

    *viewport = self.m_safeAreaViewport;

    return YES;
}

- (void)setView:(UIView *)view {
    [super setView:view];

    UIView * launchScreenView = self.m_launchScreenViewController.viewIfLoaded;

    if (launchScreenView == nil || launchScreenView == view) {
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
