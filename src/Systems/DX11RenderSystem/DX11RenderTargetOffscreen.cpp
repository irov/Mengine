#include "DX11RenderTargetOffscreen.h"

#include "DX11RenderEnum.h"
#include "DX11ErrorHelper.h"

#include "Kernel/Assertion.h"

#include "stdex/memorycopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX11RenderTargetOffscreen::DX11RenderTargetOffscreen()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX11RenderTargetOffscreen::~DX11RenderTargetOffscreen()
    {
        MENGINE_ASSERTION_FATAL( m_pD3DTexture == nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderTargetOffscreen::initialize( ID3D11Texture2D * _textureSource )
    {
        D3D11_TEXTURE2D_DESC textureDesc;
        _textureSource->GetDesc( &textureDesc );

        textureDesc.Usage = D3D11_USAGE_STAGING;
        textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

        const ID3D11DevicePtr & pD3DDevice = this->getDirect3D11Device();

        ID3D11Texture2D * pD3DTexture;
        IF_DXCALL( pD3DDevice, CreateTexture2D, (&textureDesc, nullptr, &pD3DTexture) )
        {
            return false;
        }

        m_pD3DTexture.Attach( pD3DTexture );

        m_pD3DTextureSource.Attach( _textureSource );

        m_textureDesc = textureDesc;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderTargetOffscreen::finalize()
    {
        m_pD3DTexture = nullptr;
        m_pD3DTextureSource = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderTargetOffscreen::begin() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderTargetOffscreen::end() const
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderTargetOffscreen::getHWMipmaps() const
    {
        return 1U;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderTargetOffscreen::getHWWidth() const
    {
        return m_textureDesc.Width;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderTargetOffscreen::getHWHeight() const
    {
        return m_textureDesc.Height;
    }
    //////////////////////////////////////////////////////////////////////////
    EPixelFormat DX11RenderTargetOffscreen::getHWPixelFormat() const
    {
        return EPixelFormat::PF_UNKNOWN;
    }
    //////////////////////////////////////////////////////////////////////////
    float DX11RenderTargetOffscreen::getHWWidthInv() const
    {
        return 1.0f / m_textureDesc.Width;
    }
    //////////////////////////////////////////////////////////////////////////
    float DX11RenderTargetOffscreen::getHWHeightInv() const
    {
        return  1.0f / m_textureDesc.Height;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderTargetOffscreen::calcViewport( const mt::vec2f & _size, Viewport * const _viewport ) const
    {
        float uv_width = _size.x * getHWWidthInv();
        float uv_height = _size.y * getHWHeightInv();

        _viewport->begin = mt::vec2f( 0.f, 0.f );
        _viewport->end = mt::vec2f( uv_width, uv_height );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderTargetOffscreen::getData( void * const _buffer, size_t _pitch ) const
    {
        ID3D11DeviceContextPtr pImmediateContext = this->getDirect3D11ImmediateContext();

        ID3D11Texture2DPtr textureSource = m_pD3DTextureSource;

        if( textureSource == nullptr )
        {
            textureSource = m_renderTargetTexture->getD3DTexture();
        }

        pImmediateContext->CopyResource( m_pD3DTexture.Get(), textureSource.Get() );

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        pImmediateContext->Map( m_pD3DTexture.Get(), 0, D3D11_MAP_READ, 0, &mappedResource );

        if( mappedResource.RowPitch != _pitch )
            return false;

        stdex::memorycopy( _buffer, 0, mappedResource.pData, _pitch );

        pImmediateContext->Unmap( m_pD3DTexture.Get(), 0 );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderTargetOffscreen::onRenderReset()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderTargetOffscreen::onRenderRestore()
    {
        return true;
    }
}