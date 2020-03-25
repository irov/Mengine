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
        virtual LPDIRECT3DDEVICE9 getD3DDevice() const = 0;
        virtual LPDIRECT3DTEXTURE9 getD3DTexture() const = 0;
    };
}
