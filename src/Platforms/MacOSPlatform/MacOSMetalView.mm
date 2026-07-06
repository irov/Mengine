#import "MacOSMetalView.h"

#if defined(MENGINE_ENVIRONMENT_RENDER_METAL)

@interface MacOSMetalView ()

- (void)updateDrawableSize_;

@end

@implementation MacOSMetalView
{
    NSTrackingArea * _trackingArea;
    id<MTLDevice> _device;
    id<CAMetalDrawable> _currentDrawable;
}

- (instancetype)initWithFrame:(NSRect)frameRect device:(id<MTLDevice>)device
{
    self = [super initWithFrame:frameRect];

    if( self != nil )
    {
        _trackingArea = nil;
        _device = device;
        _currentDrawable = nil;

        CAMetalLayer * metalLayer = [CAMetalLayer layer];
        metalLayer.device = _device;
        metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
        metalLayer.framebufferOnly = YES;
        metalLayer.opaque = YES;
        metalLayer.presentsWithTransaction = NO;

        self.wantsLayer = YES;
        self.layer = metalLayer;

        [self updateDrawableSize_];
    }

    return self;
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (BOOL)canBecomeKeyView
{
    return YES;
}

- (void)updateTrackingAreas
{
    [super updateTrackingAreas];

    if( _trackingArea != nil )
    {
        [self removeTrackingArea:_trackingArea];
        _trackingArea = nil;
    }

    NSTrackingAreaOptions options = NSTrackingMouseEnteredAndExited |
        NSTrackingMouseMoved |
        NSTrackingActiveInKeyWindow |
        NSTrackingInVisibleRect;

    _trackingArea = [[NSTrackingArea alloc] initWithRect:self.bounds
                                                 options:options
                                                   owner:self
                                                userInfo:nil];

    [self addTrackingArea:_trackingArea];
}

- (void)setFrameSize:(NSSize)newSize
{
    [super setFrameSize:newSize];
    [self updateDrawableSize_];
}

- (void)setBoundsSize:(NSSize)newSize
{
    [super setBoundsSize:newSize];
    [self updateDrawableSize_];
}

- (void)viewDidChangeBackingProperties
{
    [super viewDidChangeBackingProperties];
    [self updateDrawableSize_];
}

- (void)updateDrawableSize_
{
    CAMetalLayer * metalLayer = (CAMetalLayer *)self.layer;

    if( metalLayer == nil )
    {
        return;
    }

    NSRect backingBounds = [self convertRectToBacking:self.bounds];
    CGFloat backingWidth = backingBounds.size.width;
    CGFloat backingHeight = backingBounds.size.height;

    if( backingWidth < 1.0 )
    {
        backingWidth = 1.0;
    }

    if( backingHeight < 1.0 )
    {
        backingHeight = 1.0;
    }

    metalLayer.drawableSize = CGSizeMake( backingWidth, backingHeight );
    metalLayer.contentsScale = [self.window backingScaleFactor] > 0.0 ? [self.window backingScaleFactor] : [NSScreen mainScreen].backingScaleFactor;
}

- (BOOL)beginRender
{
    if( _device == nil || self.window == nil )
    {
        return NO;
    }

    [self updateDrawableSize_];

    CAMetalLayer * metalLayer = (CAMetalLayer *)self.layer;
    _currentDrawable = [metalLayer nextDrawable];

    if( _currentDrawable == nil )
    {
        return NO;
    }

    return YES;
}

- (BOOL)endRender
{
    _currentDrawable = nil;

    return YES;
}

- (void)updateVSync:(BOOL)vsync
{
    CAMetalLayer * metalLayer = (CAMetalLayer *)self.layer;
    metalLayer.displaySyncEnabled = vsync;
}

- (NSInteger)drawableWidth
{
    CAMetalLayer * metalLayer = (CAMetalLayer *)self.layer;

    return (NSInteger)metalLayer.drawableSize.width;
}

- (NSInteger)drawableHeight
{
    CAMetalLayer * metalLayer = (CAMetalLayer *)self.layer;

    return (NSInteger)metalLayer.drawableSize.height;
}

- (id<MTLDevice>)metalDevice
{
    return _device;
}

- (id<CAMetalDrawable>)currentDrawable
{
    return _currentDrawable;
}

@end

#endif
