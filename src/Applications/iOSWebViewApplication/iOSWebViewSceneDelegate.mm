#import "iOSWebViewSceneDelegate.h"

#include "Interface/LifecycleServiceInterface.h"

#import "iOSWebViewApplicationDelegate.h"

@implementation iOSWebViewSceneDelegate

#pragma mark - Scene

- (void)scene:(UIScene *)_scene willConnectToSession:(UISceneSession *)_session options:(UISceneConnectionOptions *)_options
{
    MENGINE_UNUSED( _session );
    MENGINE_UNUSED( _options );

    if( [_scene isKindOfClass:UIWindowScene.class] == NO )
    {
        return;
    }

    iOSWebViewApplicationDelegate * delegate = (iOSWebViewApplicationDelegate *)UIApplication.sharedApplication.delegate;
    UIWindowScene * scene = (UIWindowScene *)_scene;
    if( [delegate connectScene:scene] == NO )
    {
        [delegate stopApplication];

        return;
    }
}

- (void)sceneDidDisconnect:(UIScene *)_scene
{
    MENGINE_UNUSED( _scene );

    iOSWebViewApplicationDelegate * delegate = (iOSWebViewApplicationDelegate *)UIApplication.sharedApplication.delegate;
    [delegate disconnectScene];
}

#pragma mark - Activity

- (void)sceneDidBecomeActive:(UIScene *)_scene
{
    MENGINE_UNUSED( _scene );

    if( SERVICE_PROVIDER_EXIST() == false )
    {
        return;
    }

    LIFECYCLE_SERVICE()
        ->onApplicationDidBecomeActive();

    iOSWebViewApplicationDelegate * delegate = (iOSWebViewApplicationDelegate *)UIApplication.sharedApplication.delegate;
    [delegate resumeRuntime];
}

- (void)sceneWillResignActive:(UIScene *)_scene
{
    MENGINE_UNUSED( _scene );

    if( SERVICE_PROVIDER_EXIST() == false )
    {
        return;
    }

    iOSWebViewApplicationDelegate * delegate = (iOSWebViewApplicationDelegate *)UIApplication.sharedApplication.delegate;

    const Mengine::Configuration & configuration = SERVICE_PROVIDER_GET()
        ->getConfiguration();

    if( configuration.nopause == false )
    {
        [delegate pauseRuntime];
    }

    LIFECYCLE_SERVICE()
        ->onApplicationWillResignActive();
}

#pragma mark - Foreground

- (void)sceneWillEnterForeground:(UIScene *)_scene
{
    MENGINE_UNUSED( _scene );

    if( SERVICE_PROVIDER_EXIST() == false )
    {
        return;
    }

    LIFECYCLE_SERVICE()
        ->onApplicationWillEnterForeground();
}

- (void)sceneDidEnterBackground:(UIScene *)_scene
{
    MENGINE_UNUSED( _scene );

    if( SERVICE_PROVIDER_EXIST() == false )
    {
        return;
    }

    LIFECYCLE_SERVICE()
        ->onApplicationDidEnterBackground();
}

@end
