#include "DX11RenderTargetTexture.h"

#include "DX11RenderEnum.h"
#include "DX11ErrorHelper.h"

#include "Kernel/Assertion.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX11RenderTargetTexture::DX11RenderTargetTexture()
        : m_hwPixelFormat( PF_UNKNOWN )
        , m_hwChannels( 0 )
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
    bool DX11RenderTargetTexture::initialize( uint32_t _width, uint32_t _height, uint32_t _channels, EPixelFormat _format )
    {
        DXGI_FORMAT D3DFormat = Helper::toD3DFormat( _format );

        if( D3DFormat == DXGI_FORMAT_UNKNOWN )
        {
            // TODO: handle log
            return false;
        }

        m_textureDesc.Format = D3DFormat;
        m_textureDesc.Width = _width;
        m_textureDesc.Height = _height;
        m_textureDesc.MipLevels = 1;
        m_textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        m_textureDesc.Usage = D3D11_USAGE_DEFAULT;

        m_hwChannels = _channels;
        m_hwPixelFormat = _format;

        m_hwWidthInv = 1.f / (float)m_textureDesc.Width;
        m_hwHeightInv = 1.f / (float)m_textureDesc.Height;

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
    void DX11RenderTargetTexture::calcViewport( const mt::vec2f & _size, Viewport * const _viewport ) const
    {
        float uv_width = _size.x * m_hwWidthInv;
        float uv_height = _size.y * m_hwHeightInv;

        _viewport->begin = mt::vec2f( 0.f, 0.f );
        _viewport->end = mt::vec2f( uv_width, uv_height );
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

        ID3D11RenderTargetView * pRenderTargetView = m_pRenderTargetView.Get();

        pImmediateContext->OMSetRenderTargets( 1, &pRenderTargetView, m_pDepthStencilMain.Get() );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderTargetTexture::end() const
    {
        ID3D11DeviceContextPtr pImmediateContext = this->getDirect3D11ImmediateContext();

        ID3D11RenderTargetView * pRenderTargetViewOld = m_pRenderTargetViewOld.Get();

        pImmediateContext->OMSetRenderTargets( 1, &pRenderTargetViewOld, m_pDepthStencilMainOld.Get() );
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
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderTargetTexture::onRenderRestore()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}