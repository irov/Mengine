#pragma once

#include "Config/Config.h"

#define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG 0x8C00
#define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG 0x8C01
#define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG 0x8C02
#define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG 0x8C03

#define GL_ETC1_RGB8_OES 0x8D64

#if defined(MENGINE_PLATFORM_IOS)
#   error "OpenGL is not supported on iOS"
#elif defined(MENGINE_PLATFORM_ANDROID)
#   define GL_GLEXT_PROTOTYPES

#   if MENGINE_RENDER_OPENGL_ES_VERSION == 2
#       include <GLES2/gl2.h>
#       include <GLES2/gl2ext.h>
#       define MENGINE_RENDER_OPENGL_ES2
#   elif MENGINE_RENDER_OPENGL_ES_VERSION == 3
#       include <GLES3/gl3.h>
#       include <GLES3/gl3ext.h>
#       define MENGINE_RENDER_OPENGL_ES3
#   else
#       error "MENGINE_RENDER_OPENGL_ES_VERSION must be 2 or 3"
#   endif

#   define MENGINE_RENDER_OPENGL_ES
#   define MENGINE_RENDER_OPENGL_ES_ANDROID
#elif defined(MENGINE_PLATFORM_WINDOWS)
#   error "OpenGL not supported"
#elif defined(MENGINE_PLATFORM_LINUX)
#   if !defined(MENGINE_ENVIRONMENT_PLATFORM_UNIX)
#       error "OpenGL not supported"
#   endif

#   include <glad/gl.h>

#   define MENGINE_RENDER_OPENGL_NORMAL
#   define MENGINE_RENDER_OPENGL_NORMAL_LINUX
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
