#pragma once

#include "Interface/UnknownInterface.h"

#include "Environment/DirectX11/DirectX11RenderIncluder.h"

namespace Mengine
{
    class DX11RenderImageExtensionInterface
        : public UnknownInterface
    {
    public:
        virtual const ID3D11DevicePtr & getD3DDevice() const = 0;
        virtual const ID3D11Texture2DPtr & getD3DTexture() const = 0;
		virtual const ID3D11ShaderResourceViewPtr & getD3DShaderResource() const = 0;
    };
}
