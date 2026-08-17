#import "MacOSApplicationDelegate.h"

#import "Interface/PlatformServiceInterface.h"
#import "Interface/ApplicationInterface.h"

#import "Environment/MacOS/MacOSPlatformServiceExtensionInterface.h"

#import "Kernel/Logger.h"

#import "Environment/Apple/AppleGameControllerInput.h"

@interface MacOSApplicationDelegate ()

@property (nonatomic, strong) AppleGameControllerInput * m_gameControllerInput;

@end

@implementation MacOSApplicationDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)notification
{
    MENGINE_UNUSED( notification );

    self.m_gameControllerInput = [[AppleGameControllerInput alloc] init];
    [self.m_gameControllerInput start];
}

- (void)applicationDidBecomeActive:(NSNotification *)notification
{
    MENGINE_UNUSED( notification );

    if( SERVICE_PROVIDER_EXIST() == false || SERVICE_IS_INITIALIZE( Mengine::PlatformServiceInterface ) == false )
    {
        return;
    }

    Mengine::MacOSPlatformServiceExtensionInterface * extension = PLATFORM_SERVICE()
        ->getDynamicUnknown();

    extension->handleApplicationDidBecomeActive();
}

- (void)applicationWillResignActive:(NSNotification *)notification
{
    MENGINE_UNUSED( notification );

    if( SERVICE_PROVIDER_EXIST() == false || SERVICE_IS_INITIALIZE( Mengine::PlatformServiceInterface ) == false )
    {
        return;
    }

    Mengine::MacOSPlatformServiceExtensionInterface * extension = PLATFORM_SERVICE()
        ->getDynamicUnknown();

    extension->handleApplicationWillResignActive();
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
    MENGINE_UNUSED( sender );

    if( SERVICE_PROVIDER_EXIST() == true && SERVICE_IS_INITIALIZE( Mengine::ApplicationInterface ) == true )
    {
        APPLICATION_SERVICE()
            ->quit();
    }

    return NSTerminateNow;
}

- (void)applicationWillTerminate:(NSNotification *)notification
{
    MENGINE_UNUSED( notification );

    [self.m_gameControllerInput stop];
    self.m_gameControllerInput = nil;

    if( SERVICE_PROVIDER_EXIST() == false || SERVICE_IS_INITIALIZE( Mengine::PlatformServiceInterface ) == false )
    {
        return;
    }

    Mengine::MacOSPlatformServiceExtensionInterface * extension = PLATFORM_SERVICE()
        ->getDynamicUnknown();

    extension->handleApplicationWillTerminate();
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
    MENGINE_UNUSED( sender );

    return YES;
}

- (BOOL)applicationSupportsSecureRestorableState:(NSApplication *)application
{
    MENGINE_UNUSED( application );

    return YES;
}

- (BOOL)applicationShouldSaveApplicationState:(NSApplication *)application
{
    MENGINE_UNUSED( application );

    return NO;
}

- (BOOL)applicationShouldRestoreApplicationState:(NSApplication *)application
{
    MENGINE_UNUSED( application );

    return NO;
}

@end
