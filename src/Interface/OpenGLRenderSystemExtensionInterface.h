#pragma once

#include "Interface/UnknownInterface.h"

#include "Environment/OpenGL/OpenGLRenderIncluder.h"

namespace Mengine
{
    class OpenGLRenderSystemExtensionInterface
        : public UnknownInterface
    {
    public:
        virtual GLuint genTexture() = 0;
        virtual void deleteTexture( GLuint _id ) = 0;

        virtual GLuint genFramebuffer() = 0;
        virtual void deleteFramebuffer( GLuint _id ) = 0;

        virtual GLuint genBuffer() = 0;
        virtual void deleteBuffer( GLuint _id ) = 0;
    };
}
