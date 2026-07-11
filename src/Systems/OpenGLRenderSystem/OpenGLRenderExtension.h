#pragma once

#include "Environment/OpenGL/OpenGLRenderIncluder.h"

#if defined(MENGINE_RENDER_OPENGL_NORMAL)
#   define glActiveTexture_ glActiveTexture
#   define glCompressedTexImage2D_ glCompressedTexImage2D
#   define glBlendEquation_ glBlendEquation
#   define glBlendFuncSeparate_ glBlendFuncSeparate
#   define glBlendEquationSeparate_ glBlendEquationSeparate

namespace Mengine
{
    bool initialize_GLEXT();

#if defined(MENGINE_RENDER_OPENGL_NORMAL_LINUX)
    bool isGLTextureCompressionS3TCSupported();
#endif
}
#endif
