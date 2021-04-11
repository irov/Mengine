#pragma once

#include "Interface/UnknownInterface.h"

#include "Environment/DirectX11/DirectX11RenderIncluder.h"

namespace Mengine
{
    class DX11RenderSystemExtensionInterface
        : public UnknownInterface
    {
    public:
        virtual ID3D11Device * getDirect3D11Device() const = 0;
        virtual ID3D11DeviceContext * getDirect3D11DeviceContext() const = 0;
    };
}