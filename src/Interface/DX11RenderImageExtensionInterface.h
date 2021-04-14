#pragma once

#include "Interface/UnknownInterface.h"

#include "Environment/DirectX11/DirectX11RenderIncluder.h"

namespace Mengine
{
    class DX11RenderImageExtensionInterface
        : public UnknownInterface
    {
    public:
        virtual IDirect3DDevice9 * getD3DDevice() const = 0;
        virtual IDirect3DTexture9 * getD3DTexture() const = 0;
    };
}
