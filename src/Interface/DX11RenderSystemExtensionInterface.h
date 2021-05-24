#pragma once

#include "Interface/UnknownInterface.h"

#include "Environment/DirectX11/DirectX11RenderIncluder.h"

namespace Mengine
{
    class DX11RenderSystemExtensionInterface
        : public UnknownInterface
    {
    public:
        virtual const ID3D11DevicePtr & getDirect3D11Device() const = 0;
        virtual const ID3D11DeviceContextPtr & getDirect3D11DeviceContext() const = 0;
    };
}