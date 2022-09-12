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

#   include "SDL_opengles.h"
#   include "SDL_opengles2.h"

#   define MENGINE_RENDER_OPENGL_ES
#   define MENGINE_RENDER_OPENGL_ES_IOS
#elif defined(MENGINE_PLATFORM_ANDROID)
#   define GL_GLEXT_PROTOTYPES

#   include "SDL_opengles.h"
#   include "SDL_opengles2.h"
#   include "SDL_opengles2_gl2.h"
#   include "SDL_opengles2_gl2ext.h"

#   define MENGINE_RENDER_OPENGL_ES
#   define MENGINE_RENDER_OPENGL_ES_ANDROID
#elif defined(MENGINE_PLATFORM_WINDOWS)
#   include "SDL_opengl.h"
#   include "SDL_opengl_glext.h"

#   define MENGINE_RENDER_OPENGL_NORMAL
#   define MENGINE_RENDER_OPENGL_NORMAL_WINDOWS
#elif defined(MENGINE_PLATFORM_LINUX)
#   include "SDL_opengl.h"
#   include "SDL_opengl_glext.h"

#   define MENGINE_RENDER_OPENGL_NORMAL
#   define MENGINE_RENDER_OPENGL_NORMAL_LINUX
#elif defined(MENGINE_PLATFORM_MACOS)
#   include "SDL_opengl.h"
#   include "SDL_opengl_glext.h"

#   define MENGINE_RENDER_OPENGL_NORMAL
#   define MENGINE_RENDER_OPENGL_NORMAL_OSX
#endif
