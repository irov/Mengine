#import "MacOSWindowDelegate.h"

@implementation MacOSWindowDelegate
{
    Mengine::MacOSPlatformService * _service;
}

- (instancetype)initWithPlatformService:(Mengine::MacOSPlatformService *)service
{
    self = [super init];

    if( self != nil )
    {
        _service = service;
    }

    return self;
}

- (void)windowDidResize:(NSNotification *)notification
{
    MENGINE_UNUSED( notification );

    _service->handleWindowDidResize();
}

- (void)windowDidBecomeKey:(NSNotification *)notification
{
    MENGINE_UNUSED( notification );

    _service->handleWindowDidBecomeKey();
}

- (void)windowDidResignKey:(NSNotification *)notification
{
    MENGINE_UNUSED( notification );

    _service->handleWindowDidResignKey();
}

- (void)windowDidMiniaturize:(NSNotification *)notification
{
    MENGINE_UNUSED( notification );

    _service->handleWindowDidMiniaturize();
}

- (void)windowDidDeminiaturize:(NSNotification *)notification
{
    MENGINE_UNUSED( notification );

    _service->handleWindowDidDeminiaturize();
}

- (void)windowWillClose:(NSNotification *)notification
{
    MENGINE_UNUSED( notification );

    _service->handleWindowWillClose();
}

@end
