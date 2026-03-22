#include "iOSOpenGLView.h"

#if defined(MENGINE_ENVIRONMENT_RENDER_OPENGL)

#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/ES2/glext.h>

@implementation iOSOpenGLView
{
    EAGLContext * _context;
    GLuint _framebuffer;
    GLuint _colorRenderbuffer;
    GLint _backingWidth;
    GLint _backingHeight;
}

+ (Class)layerClass
{
    return [CAEAGLLayer class];
}

- (instancetype)initWithFrame:(CGRect)frame context:(EAGLContext *)context
{
    self = [super initWithFrame:frame];

    if( self != nil )
    {
        _context = context;
        _framebuffer = 0;
        _colorRenderbuffer = 0;
        _backingWidth = 0;
        _backingHeight = 0;

        CAEAGLLayer * eaglLayer = (CAEAGLLayer *)self.layer;
        eaglLayer.opaque = YES;
        eaglLayer.drawableProperties = @{
            kEAGLDrawablePropertyRetainedBacking : @(NO),
            kEAGLDrawablePropertyColorFormat : kEAGLColorFormatRGBA8
        };

        self.contentScaleFactor = [UIScreen mainScreen].scale;
        self.multipleTouchEnabled = YES;
    }

    return self;
}

- (BOOL)beginRender
{
    if( _context == nil || _framebuffer == 0 || _colorRenderbuffer == 0 )
    {
        return NO;
    }

    if( [EAGLContext setCurrentContext:_context] == NO )
    {
        return NO;
    }

    glBindFramebuffer( GL_FRAMEBUFFER, _framebuffer );
    glBindRenderbuffer( GL_RENDERBUFFER, _colorRenderbuffer );

    return YES;
}

- (BOOL)endRender
{
    if( _context == nil || _colorRenderbuffer == 0 )
    {
        return NO;
    }

    if( [EAGLContext setCurrentContext:_context] == NO )
    {
        return NO;
    }

    glBindRenderbuffer( GL_RENDERBUFFER, _colorRenderbuffer );

    if( [_context presentRenderbuffer:GL_RENDERBUFFER] == NO )
    {
        return NO;
    }

    return YES;
}

- (BOOL)createFramebuffer
{
    [EAGLContext setCurrentContext:_context];

    glGenFramebuffers( 1, &_framebuffer );
    glGenRenderbuffers( 1, &_colorRenderbuffer );

    glBindFramebuffer( GL_FRAMEBUFFER, _framebuffer );
    glBindRenderbuffer( GL_RENDERBUFFER, _colorRenderbuffer );

    [_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)self.layer];

    glGetRenderbufferParameteriv( GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &_backingWidth );
    glGetRenderbufferParameteriv( GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &_backingHeight );

    glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _colorRenderbuffer );

    GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );

    if( status != GL_FRAMEBUFFER_COMPLETE )
    {
        return NO;
    }

    return YES;
}

- (void)destroyFramebuffer
{
    [EAGLContext setCurrentContext:_context];

    if( _framebuffer != 0 )
    {
        glDeleteFramebuffers( 1, &_framebuffer );
        _framebuffer = 0;
    }

    if( _colorRenderbuffer != 0 )
    {
        glDeleteRenderbuffers( 1, &_colorRenderbuffer );
        _colorRenderbuffer = 0;
    }

    _backingWidth = 0;
    _backingHeight = 0;
}

- (GLint)backingWidth
{
    return _backingWidth;
}

- (GLint)backingHeight
{
    return _backingHeight;
}

- (void)layoutSubviews
{
    [super layoutSubviews];

    [self destroyFramebuffer];
    [self createFramebuffer];
}

@end

#endif
