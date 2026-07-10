#pragma once

#include "Config/Config.h"

#define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG 0x8C00
#define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG 0x8C01
#define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG 0x8C02
#define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG 0x8C03

#define GL_ETC1_RGB8_OES 0x8D64

#if defined(MENGINE_PLATFORM_IOS)
#   include <TargetConditionals.h>

#   define GL_GLEXT_PROTOTYPES

#   if !defined(MENGINE_ENVIRONMENT_PLATFORM_IOS)
#       error "OpenGL ES not supported"
#   endif

#   include <OpenGLES/ES2/gl.h>
#   include <OpenGLES/ES2/glext.h>

#   define MENGINE_RENDER_OPENGL_ES
#   define MENGINE_RENDER_OPENGL_ES_IOS
#elif defined(MENGINE_PLATFORM_ANDROID)
#   define GL_GLEXT_PROTOTYPES

#   include <GLES2/gl2.h>
#   include <GLES2/gl2ext.h>

#   define MENGINE_RENDER_OPENGL_ES
#   define MENGINE_RENDER_OPENGL_ES_ANDROID
#elif defined(MENGINE_PLATFORM_WINDOWS)
#   error "OpenGL not supported"
#elif defined(MENGINE_PLATFORM_LINUX)
#   error "OpenGL not supported"
#elif defined(MENGINE_PLATFORM_MACOS)
#   if !defined(MENGINE_ENVIRONMENT_PLATFORM_MACOS)
#       error "OpenGL not supported"
#   endif

#   ifndef GL_SILENCE_DEPRECATION
#       define GL_SILENCE_DEPRECATION
#   endif

#   include <OpenGL/gl3.h>
#   include <OpenGL/gl3ext.h>

#   define MENGINE_RENDER_OPENGL_NORMAL
#   define MENGINE_RENDER_OPENGL_NORMAL_OSX
#endif
