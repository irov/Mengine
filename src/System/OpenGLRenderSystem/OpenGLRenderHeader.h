#pragma once

#define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG                      0x8C00
#define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG                      0x8C01
#define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG                     0x8C02
#define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG                     0x8C03

#define GL_ETC1_RGB8_OES                                        0x8D64

#if __APPLE__ && !MACOSX
#   include <TargetConditionals.h>
#    define GL_GLEXT_PROTOTYPES
#   include "SDL_opengles.h"
#   include "SDL_opengles2.h"
#endif

#if __ANDROID__
#    define GL_GLEXT_PROTOTYPES
#   include "SDL_opengles.h"
#   include "SDL_opengles2.h"
#   include "SDL_opengles2_gl2.h"
#   include "SDL_opengles2_gl2ext.h"
#endif

#if WIN32 || MACOSX
#include "SDL_opengl.h"
#include "SDL_opengl_glext.h"
#endif
