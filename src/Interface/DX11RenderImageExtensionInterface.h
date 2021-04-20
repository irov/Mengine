#pragma once

#include "Interface/UnknownInterface.h"

#include "Environment/DirectX11/DirectX11RenderIncluder.h"

namespace Mengine
{
    class DX11RenderImageExtensionInterface
        : public UnknownInterface
    {
    public:
        virtual ID3D11Device * getD3DDevice() const = 0;
        virtual ID3D11Texture2D * getD3DTexture() const = 0;
		virtual ID3D11ShaderResourceView * getD3DShaderResource() const = 0;
    };
}
