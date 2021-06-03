#pragma once

#include "Interface/UnknownInterface.h"

#include "Environment/OpenGL/OpenGLRenderIncluder.h"

namespace Mengine
{
    class OpenGLRenderImageExtensionInterface
        : public UnknownInterface
    {
    public:
        virtual GLuint getUID() const = 0;

    public:
        virtual void bind( uint32_t _stage ) = 0;
        virtual void unbind( uint32_t _stage ) = 0;
    };
}
