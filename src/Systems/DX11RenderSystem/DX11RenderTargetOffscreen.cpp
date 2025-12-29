#include "DX11RenderTargetOffscreen.h"

#include "DX11RenderEnum.h"
#include "DX11RenderErrorHelper.h"

#include "Kernel/Assertion.h"

#include "stdex/memorycopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX11RenderTargetOffscreen::DX11RenderTargetOffscreen()
        : m_hwWidthInv( 0.f )
        , m_hwHeightInv( 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX11RenderTargetOffscreen::~DX11RenderTargetOffscreen()
    {
        MENGINE_ASSERTION_FATAL( m_pD3DTexture == nullptr, "Texture is not null" );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderTargetOffscreen::initialize( uint32_t _width, uint32_t _height, ID3D11Texture2D * _textureSource )
    {
        m_pD3DTextureSource.Attach( _textureSource );

        D3D11_TEXTURE2D_DESC textureDesc;
        m_pD3DTextureSource->GetDesc( &textureDesc );

        textureDesc.Usage = D3D11_USAGE_STAGING;
        textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

        const ID3D11DevicePtr & pD3DDevice = this->getDirect3D11Device();

        ID3D11Texture2D * pD3DTexture;
        MENGINE_IF_DX11_CALL( pD3DDevice, CreateTexture2D, (&textureDesc, nullptr, &pD3DTexture) )
        {
            return false;
        }

        m_pD3DTexture.Attach( pD3DTexture );

        m_textureDesc = textureDesc;

        m_hwWidthInv = 1.f / m_textureDesc.Width;
        m_hwHeightInv = 1.f / m_textureDesc.Height;

        float u = float( _width ) / float( m_textureDesc.Width );
        float v = float( _height ) / float( m_textureDesc.Height );

        mt::uv4_from_mask( &m_uv, mt::vec4f( 0.f, 0.f, u, v ) );

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
        return m_hwWidthInv;
    }
    //////////////////////////////////////////////////////////////////////////
    float DX11RenderTargetOffscreen::getHWHeightInv() const
    {
        return  m_hwHeightInv;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderTargetOffscreen::getUpscalePow2() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::uv4f & DX11RenderTargetOffscreen::getUV() const
    {
        return m_uv;
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
        {
            pImmediateContext->Unmap( m_pD3DTexture.Get(), 0 );

            return false;
        }

        stdex::memorycopy( _buffer, 0, mappedResource.pData, _pitch );

        pImmediateContext->Unmap( m_pD3DTexture.Get(), 0 );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderTargetOffscreen::onRenderReset()
    {
        //ToDo
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderTargetOffscreen::onRenderRestore()
    {
        //ToDo

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}