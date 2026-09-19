#include "iOSMetalView.h"

#if defined(MENGINE_ENVIRONMENT_RENDER_METAL)

@implementation iOSMetalView
{
    id<MTLDevice> _device;
}

+ (Class)layerClass
{
    return [CAMetalLayer class];
}

- (instancetype)initWithFrame:(CGRect)frame device:(id<MTLDevice>)device
{
    self = [super initWithFrame:frame];

    if( self != nil )
    {
        _device = device;

        CAMetalLayer * metalLayer = (CAMetalLayer *)self.layer;
        metalLayer.device = _device;
        metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
        metalLayer.framebufferOnly = YES;
        metalLayer.opaque = YES;

        self.contentScaleFactor = [UIScreen mainScreen].scale;
        self.multipleTouchEnabled = YES;
    }

    return self;
}

- (NSInteger)drawableWidth
{
    CAMetalLayer * metalLayer = (CAMetalLayer *)self.layer;

    return (NSInteger)(metalLayer.drawableSize.width);
}

- (NSInteger)drawableHeight
{
    CAMetalLayer * metalLayer = (CAMetalLayer *)self.layer;

    return (NSInteger)(metalLayer.drawableSize.height);
}

- (id<MTLDevice>)metalDevice
{
    return _device;
}

- (void)layoutSubviews
{
    [super layoutSubviews];

    CAMetalLayer * metalLayer = (CAMetalLayer *)self.layer;
    CGFloat scale = self.contentScaleFactor;
    CGSize boundsSize = self.bounds.size;

    metalLayer.drawableSize = CGSizeMake( boundsSize.width * scale, boundsSize.height * scale );
}

@end

#endif
