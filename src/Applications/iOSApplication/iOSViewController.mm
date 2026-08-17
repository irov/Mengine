#import "iOSViewController.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/ApplicationInterface.h"

#include "Environment/iOS/iOSPlatformServiceExtensionInterface.h"

@interface iOSViewController ()

@property (nonatomic, strong) UIViewController * m_launchScreenViewController;

@end

@implementation iOSViewController

- (void)viewSafeAreaInsetsDidChange {
    [super viewSafeAreaInsetsDidChange];

    if( SERVICE_IS_INITIALIZE( Mengine::ApplicationInterface ) == false )
    {
        return;
    }

    UIEdgeInsets viewInsets = self.view.safeAreaInsets;
    CGFloat scale = self.view.contentScaleFactor;

    CGSize viewSize = self.view.bounds.size;

    Mengine::Viewport viewport(
        (float)(viewInsets.left * scale),
        (float)(viewInsets.top * scale),
        (float)((viewSize.width - viewInsets.right) * scale),
        (float)((viewSize.height - viewInsets.bottom) * scale)
    );

    APPLICATION_SERVICE()
        ->setSafeAreaViewport( viewport );
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
