#pragma once

#include "Interface/UnknownInterface.h"

#include "Kernel/Mixin.h"

#include "d3d9.h"

namespace Mengine
{
    class DX9RenderImageExtensionInterface
        : public UnknownInterface
    {
    public:
        virtual IDirect3DDevice9 * getD3DDevice() const = 0;
        virtual IDirect3DTexture9 * getD3DTexture() const = 0;
    };
}
