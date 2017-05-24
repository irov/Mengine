#	pragma once

#if _WIN32
#	include "OpenGLRenderHeaderWin32.h"
#elif __APPLE__
#   include <TargetConditionals.h>
#	if TARGET_OS_IPHONE
#	define MENGINE_OPENGL_ES
#	include "OpenGLRenderHeaderIOS.h"
#	elif __MACH__
#	include "OpenGLRenderHeaderMacOS.h"
#	endif

#endif

#define MENGINE_DECLARE_GLEXT_II(x, y) x##y
#define MENGINE_DECLARE_GLEXT_I(x, y)  MENGINE_DECLARE_GLEXT_II(x, y)
#define MENGINE_DECLARE_GLEXT(x)  MENGINE_DECLARE_GLEXT_I(x, MENGINE_SUFIX_GLEXT)

#define mglActiveTexture MENGINE_DECLARE_GLEXT(glActiveTexture)
#define mglClientActiveTexture MENGINE_DECLARE_GLEXT(glClientActiveTexture)
#define mglCompressedTexImage2D MENGINE_DECLARE_GLEXT(glCompressedTexImage2D)
#define mglBlendEquation MENGINE_DECLARE_GLEXT(glBlendEquation)
