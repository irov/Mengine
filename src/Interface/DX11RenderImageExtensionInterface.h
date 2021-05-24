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

    public:
        virtual void bind( const ID3D11DeviceContextPtr & _pImmediateContext, uint32_t _stage ) = 0;
        virtual void unbind( const ID3D11DeviceContextPtr & _pImmediateContext, uint32_t _stage ) = 0;
    };
}
