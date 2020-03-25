#pragma once

#include "Interface/UnknownInterface.h"

#include "Environment/OpenGL/OpenGLRenderIncluder.h"

#include "Kernel/Mixin.h"

namespace Mengine
{
    class OpenGLRenderImageExtensionInterface
        : public UnknownInterface
    {
    public:
        virtual GLuint getUID() const = 0;
    };
}
