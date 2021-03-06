#pragma once

#include "Config/Config.h"

#include <d3d11.h>

#include <wrl/client.h>

namespace Mengine
{
    template<class T>
    using DX11ResourcePtr = Microsoft::WRL::ComPtr<T>;

    typedef DX11ResourcePtr<ID3D11DeviceContext> ID3D11DeviceContextPtr;
    typedef DX11ResourcePtr<ID3D11Device> ID3D11DevicePtr;
    typedef DX11ResourcePtr<ID3D11DeviceContext> ID3D11DeviceContextPtr;
    typedef DX11ResourcePtr<IDXGISwapChain> IDXGISwapChainPtr;
    typedef DX11ResourcePtr<ID3D11Texture2D> ID3D11Texture2DPtr;
    typedef DX11ResourcePtr<ID3D11ShaderResourceView> ID3D11ShaderResourceViewPtr;
    typedef DX11ResourcePtr<ID3D11RenderTargetView> ID3D11RenderTargetViewPtr;
    typedef DX11ResourcePtr<ID3D11DepthStencilView> ID3D11DepthStencilViewPtr;
    typedef DX11ResourcePtr<ID3D11Buffer> ID3D11BufferPtr;
    typedef DX11ResourcePtr<ID3D11VertexShader> ID3D11VertexShaderPtr;
    typedef DX11ResourcePtr<ID3D11PixelShader> ID3D11PixelShaderPtr;
    typedef DX11ResourcePtr<ID3D11InputLayout> ID3D11InputLayoutPtr;
}