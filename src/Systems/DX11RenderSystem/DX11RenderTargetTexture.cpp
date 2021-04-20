#include "DX11RenderTargetTexture.h"

#include "DX11RenderEnum.h"
#include "DX11ErrorHelper.h"

#include "Kernel/Assertion.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX11RenderTargetTexture::DX11RenderTargetTexture()
        : m_hwPixelFormat( PF_UNKNOWN )
        , m_hwWidthInv( 0.f )
        , m_hwHeightInv( 0.f )
        , m_pD3DTexture( nullptr )
        , m_pD3DResourceView( nullptr )
        , m_pRenderTargetView( nullptr )
		, m_pRenderTargetViewOld(nullptr)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX11RenderTargetTexture::~DX11RenderTargetTexture()
    {
        MENGINE_ASSERTION_FATAL( m_pD3DTexture == nullptr );
		MENGINE_ASSERTION_FATAL(m_pD3DTexture == nullptr);

    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderTargetTexture::initialize( uint32_t _width, uint32_t _height, uint32_t _channels, EPixelFormat _format )
    {
		DXGI_FORMAT D3DFormat = Helper::toD3DFormat(_format);
		if (D3DFormat == DXGI_FORMAT_UNKNOWN)
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

		IF_DXCALL(m_pD3DDevice, CreateTexture2D, (&m_textureDesc, nullptr, &m_pD3DTexture))
		{
			return nullptr;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

		shaderResourceViewDesc.Format = m_textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = m_textureDesc.MipLevels;

		IF_DXCALL(m_pD3DDevice, CreateShaderResourceView, (m_pD3DTexture, &shaderResourceViewDesc, &m_pD3DResourceView))
		{
			return nullptr;
		}

		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;

		renderTargetViewDesc.Format = m_textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;

		IF_DXCALL(m_pD3DDevice, CreateRenderTargetView, (m_pD3DTexture, &renderTargetViewDesc, &m_pRenderTargetView))
		{
			return nullptr;
		}

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderTargetTexture::_initialize()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderTargetTexture::finalize()
    {
        DXRELEASE( m_pD3DTexture );

        this->_finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderTargetTexture::_finalize()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderTargetTexture::getHWChannels() const
    {
        return m_hwChannels;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderTargetTexture::getHWDepth() const
    {
        return 1U;
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
    bool DX11RenderTargetTexture::begin()
    {
		ID3D11DeviceContext *pImmediateContext = nullptr;
		m_pD3DDevice->GetImmediateContext(&pImmediateContext);

		pImmediateContext->OMGetRenderTargets(1, &m_pRenderTargetViewOld, &m_pDepthStencilMain);
		pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilMain);

		pImmediateContext->Release();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderTargetTexture::end()
    {
		ID3D11DeviceContext *pImmediateContext = nullptr;
		m_pD3DDevice->GetImmediateContext(&pImmediateContext);

		pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetViewOld, m_pDepthStencilMain);

		pImmediateContext->Release();
	}
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderTargetTexture::getData( void * const _buffer, size_t _pitch ) const
    {
        MENGINE_UNUSED( _buffer );
        MENGINE_UNUSED( _pitch );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    ID3D11Device * DX11RenderTargetTexture::getDirect3dDevice11() const
    {
        return m_pD3DDevice;
    }
    //////////////////////////////////////////////////////////////////////////
	ID3D11Texture2D * DX11RenderTargetTexture::getD3DTexture() const
    {
        return m_pD3DTexture;
    }
	//////////////////////////////////////////////////////////////////////////
	ID3D11ShaderResourceView * DX11RenderTargetTexture::getD3DShaderResource() const
	{
		return m_pD3DResourceView;
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