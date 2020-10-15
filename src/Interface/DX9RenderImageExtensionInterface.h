#pragma once

#include "Interface/UnknownInterface.h"

#include "Environment/DirectX9/DirectX9RenderIncluder.h"

#include "Kernel/Mixin.h"

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
