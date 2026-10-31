#pragma once

#include "Interface/UnknownInterface.h"

#include "Environment/OpenGL/OpenGLRenderIncluder.h"

#include "Kernel/Resolution.h"

namespace Mengine
{
    class OpenGLRenderSystemExtensionInterface
        : public UnknownInterface
    {
    public:
        virtual void setRenderTargetResolution( const Resolution & _resolution ) = 0;
        virtual const Resolution & getRenderTargetResolution() const = 0;

        virtual GLuint genTexture() = 0;
        virtual void deleteTexture( GLuint _id ) = 0;

        virtual GLuint genFramebuffer() = 0;
        virtual void deleteFramebuffer( GLuint _id ) = 0;

        virtual GLuint genBuffer() = 0;
        virtual void deleteBuffer( GLuint _id ) = 0;

        virtual GLuint genFragmentShader() = 0;
        virtual void deleteFragmentShader( GLuint _id ) = 0;

        virtual GLuint genVertexShader() = 0;
        virtual void deleteVertexShader( GLuint _id ) = 0;
    };
}
