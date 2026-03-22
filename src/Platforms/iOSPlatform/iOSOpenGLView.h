#pragma once

#include "Config/Config.h"

#if defined(MENGINE_ENVIRONMENT_RENDER_OPENGL)

#ifndef GLES_SILENCE_DEPRECATION
#define GLES_SILENCE_DEPRECATION
#endif

#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES2/gl.h>

@interface iOSOpenGLView : UIView

- (instancetype)initWithFrame:(CGRect)frame context:(EAGLContext *)context;

- (BOOL)beginRender;
- (BOOL)endRender;

- (BOOL)createFramebuffer;
- (void)destroyFramebuffer;

- (GLint)backingWidth;
- (GLint)backingHeight;

@end

#endif
