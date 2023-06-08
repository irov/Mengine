#include "DX11RenderTargetTexture.h"

#include "DX11RenderEnum.h"
#include "DX11RenderErrorHelper.h"

#include "Kernel/TextureHelper.h"
#include "Kernel/Assertion.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX11RenderTargetTexture::DX11RenderTargetTexture()
        : m_width( 0 )
        , m_height( 0 )
        , m_hwPixelFormat( PF_UNKNOWN )
        , m_hwWidthInv( 0.f )
        , m_hwHeightInv( 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX11RenderTargetTexture::~DX11RenderTargetTexture()
    {
        MENGINE_ASSERTION_FATAL( m_pD3DTexture == nullptr );
        MENGINE_ASSERTION_FATAL( m_pD3DResourceView == nullptr );
        MENGINE_ASSERTION_FATAL( m_pRenderTargetView == nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderTargetTexture::initialize( uint32_t _width, uint32_t _height, EPixelFormat _format )
    {
        DXGI_FORMAT D3DFormat = Helper::toD3DFormat( _format );

        if( D3DFormat == DXGI_FORMAT_UNKNOWN )
        {
            // TODO: handle log
            return false;
        }

        m_width = _width;
        m_height = _height;

        m_textureDesc.Format = D3DFormat;
        m_textureDesc.Width = Helper::getTexturePow2( _width );
        m_textureDesc.Height = Helper::getTexturePow2( _height );
        m_textureDesc.MipLevels = 1;
        m_textureDesc.ArraySize = 1;
        m_textureDesc.SampleDesc.Count = 1;
        m_textureDesc.SampleDesc.Quality = 0;
        m_textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        m_textureDesc.Usage = D3D11_USAGE_DEFAULT;
        m_textureDesc.CPUAccessFlags = 0;
        m_textureDesc.MiscFlags = 0;
        m_hwPixelFormat = _format;

        m_hwWidthInv = 1.f / (float)m_textureDesc.Width;
        m_hwHeightInv = 1.f / (float)m_textureDesc.Height;

        float u = float( _width ) / float( m_textureDesc.Width );
        float v = float( _height ) / float( m_textureDesc.Height );

        mt::uv4_from_mask( &m_uv, mt::vec4f( 0.f, 0.f, u, v ) );

        const ID3D11DevicePtr & pD3DDevice = this->getDirect3D11Device();

        ID3D11Texture2D * pD3DTexture;
        IF_DXCALL( pD3DDevice, CreateTexture2D, (&m_textureDesc, nullptr, &pD3DTexture) )
        {
            return nullptr;
        }

        m_pD3DTexture.Attach( pD3DTexture );

        D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
        ZeroMemory( &shaderResourceViewDesc, sizeof( D3D11_SHADER_RESOURCE_VIEW_DESC ) );

        shaderResourceViewDesc.Format = m_textureDesc.Format;
        shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
        shaderResourceViewDesc.Texture2D.MipLevels = m_textureDesc.MipLevels;

        ID3D11ShaderResourceView * pD3DResourceView;
        IF_DXCALL( pD3DDevice, CreateShaderResourceView, (m_pD3DTexture.Get(), &shaderResourceViewDesc, &pD3DResourceView) )
        {
            return nullptr;
        }

        m_pD3DResourceView.Attach( pD3DResourceView );

        D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
        ZeroMemory( &renderTargetViewDesc, sizeof( D3D11_RENDER_TARGET_VIEW_DESC ) );

        renderTargetViewDesc.Format = m_textureDesc.Format;
        renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        renderTargetViewDesc.Texture2D.MipSlice = 0;

        ID3D11RenderTargetView * pRenderTargetView;
        IF_DXCALL( pD3DDevice, CreateRenderTargetView, (m_pD3DTexture.Get(), &renderTargetViewDesc, &pRenderTargetView) )
        {
            return nullptr;
        }

        m_pRenderTargetView.Attach( pRenderTargetView );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderTargetTexture::finalize()
    {
        MENGINE_ASSERTION_FATAL( m_pRenderTargetViewOld == nullptr, "invalid release render target view" );
        MENGINE_ASSERTION_FATAL( m_pDepthStencilMainOld == nullptr, "invalid release depth stencil main" );

        m_pD3DTexture = nullptr;
        m_pD3DResourceView = nullptr;
        m_pRenderTargetView = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    EPixelFormat DX11RenderTargetTexture::getHWPixelFormat() const
    {
        return m_hwPixelFormat;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderTargetTexture::getHWMipmaps() const
    {
        return 1U;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderTargetTexture::getHWWidth() const
    {
        return m_textureDesc.Width;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderTargetTexture::getHWHeight() const
    {
        return m_textureDesc.Height;
    }
    //////////////////////////////////////////////////////////////////////////
    float DX11RenderTargetTexture::getHWWidthInv() const
    {
        return m_hwWidthInv;
    }
    //////////////////////////////////////////////////////////////////////////
    float DX11RenderTargetTexture::getHWHeightInv() const
    {
        return m_hwHeightInv;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderTargetTexture::getUpscalePow2() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderTargetTexture::begin() const
    {
        ID3D11DeviceContextPtr pImmediateContext = this->getDirect3D11ImmediateContext();

        ID3D11RenderTargetView * pRenderTargetViewOld;
        ID3D11DepthStencilView * pDepthStencilMainOld;
        pImmediateContext->OMGetRenderTargets( 1, &pRenderTargetViewOld, &pDepthStencilMainOld );

        m_pRenderTargetViewOld = pRenderTargetViewOld;
        m_pDepthStencilMainOld = pDepthStencilMainOld;

        UINT NumViewports = 1;
        D3D11_VIEWPORT VPOld;
        pImmediateContext->RSGetViewports( &NumViewports, &VPOld );

        m_VPOld = VPOld;

        ID3D11RenderTargetView * pRenderTargetView = m_pRenderTargetView.Get();

        pImmediateContext->OMSetRenderTargets( 1, &pRenderTargetView, m_pDepthStencilMain.Get() );

        D3D11_VIEWPORT VP;
        VP.TopLeftX = 0.f;
        VP.TopLeftY = 0.f;
        VP.Width = (float)m_width;
        VP.Height = (float)m_height;
        VP.MinDepth = 0.f;
        VP.MaxDepth = 1.f;

        pImmediateContext->RSSetViewports( 1, &VP );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderTargetTexture::end() const
    {
        ID3D11DeviceContextPtr pImmediateContext = this->getDirect3D11ImmediateContext();

        ID3D11RenderTargetView * pRenderTargetViewOld = m_pRenderTargetViewOld.Get();

        pImmediateContext->OMSetRenderTargets( 1, &pRenderTargetViewOld, m_pDepthStencilMainOld.Get() );

        m_pRenderTargetViewOld = nullptr;
        m_pDepthStencilMainOld = nullptr;

        pImmediateContext->RSSetViewports( 1, &m_VPOld );
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::uv4f & DX11RenderTargetTexture::getUV() const
    {
        return m_uv;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderTargetTexture::getData( void * const _buffer, size_t _pitch ) const
    {
        MENGINE_UNUSED( _buffer );
        MENGINE_UNUSED( _pitch );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    const ID3D11DevicePtr & DX11RenderTargetTexture::getDirect3dDevice11() const
    {
        const ID3D11DevicePtr & pD3DDevice = this->getDirect3D11Device();

        return pD3DDevice;
    }
    //////////////////////////////////////////////////////////////////////////
    const ID3D11Texture2DPtr & DX11RenderTargetTexture::getD3DTexture() const
    {
        return m_pD3DTexture;
    }
    //////////////////////////////////////////////////////////////////////////
    const ID3D11ShaderResourceViewPtr & DX11RenderTargetTexture::getD3DShaderResource() const
    {
        return m_pD3DResourceView;
    }
    //////////////////////////////////////////////////////////////////////////
    const D3D11_TEXTURE2D_DESC & DX11RenderTargetTexture::GetTextureDesc() const
    {
        return m_textureDesc;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderTargetTexture::onRenderReset()
    {
        //ToDo
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderTargetTexture::onRenderRestore()
    {
        //ToDo

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}