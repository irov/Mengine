#pragma once

#include "Interface/UnknownInterface.h"

#include "Environment/DirectX9/DX9RenderIncluder.h"

namespace Mengine
{
    class DX9RenderImageExtensionInterface
        : public UnknownInterface
    {
    public:
        virtual IDirect3DDevice9 * getD3DDevice() const = 0;
        virtual IDirect3DTexture9 * getD3DTexture() const = 0;

    public:
        virtual void bind( uint32_t _stage ) = 0;
        virtual void unbind( uint32_t _stage ) = 0;
    };
}
