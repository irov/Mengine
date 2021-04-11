#pragma once

#include "Interface/UnknownInterface.h"

#include "Environment/DirectX11/DirectX11RenderIncluder.h"

namespace Mengine
{
    class DX11RenderSystemExtensionInterface
        : public UnknownInterface
    {
    public:
        virtual IDirect3DDevice * getDirect3DDevice11() const = 0;
    };
}