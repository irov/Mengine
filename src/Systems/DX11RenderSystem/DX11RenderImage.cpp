#include "DX11RenderImage.h"

#include "DX11ErrorHelper.h"
#include "DX11RenderEnum.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX11RenderImage::DX11RenderImage()
        : m_pD3DTexture( nullptr )
        , m_pD3DResourceView( nullptr )
        , m_hwChannels( 0 )
        , m_hwDepth( 0 )
        , m_hwPixelFormat( PF_UNKNOWN )
        , m_hwWidthInv( 0.f )
        , m_hwHeightInv( 0.f )
        , m_pitch( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX11RenderImage::~DX11RenderImage()
    {
        MENGINE_ASSERTION_FATAL( m_pD3DTexture == nullptr );
        MENGINE_ASSERTION_FATAL( m_pD3DResourceView == nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderImage::initialize( uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, EPixelFormat _pixelFormat )
    {
        DXGI_FORMAT D3DFormat = Helper::toD3DFormat( _pixelFormat );
        if( D3DFormat == DXGI_FORMAT_UNKNOWN )
        {
            // TODO: handle log
            return false;
        }

		ZeroMemory(&m_textureDesc, sizeof(D3D11_TEXTURE2D_DESC));

        m_textureDesc.Width = _width;
        m_textureDesc.Height = _height;
        m_textureDesc.MipLevels = _mipmaps;
        m_textureDesc.ArraySize = 1;
        m_textureDesc.Format = D3DFormat;
        m_textureDesc.SampleDesc.Count = 1;
        m_textureDesc.SampleDesc.Quality = 0;
        m_textureDesc.Usage = D3D11_USAGE_DEFAULT;
        m_textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        m_textureDesc.CPUAccessFlags = 0;
        m_textureDesc.MiscFlags = 0;

        m_hwChannels = _channels;
        m_hwDepth = _depth;
        m_hwPixelFormat = _pixelFormat;

        m_hwWidthInv = 1.f / (float)m_textureDesc.Width;
        m_hwHeightInv = 1.f / (float)m_textureDesc.Height;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderImage::finalize()
    {
        m_renderImageProvider = nullptr;

        DXRELEASE( m_pD3DTexture );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderImage::bind( uint32_t _stage )
    {
        ID3D11DeviceContext * pImmediateContext = nullptr;
        m_pD3DDevice->GetImmediateContext( &pImmediateContext );

#ifdef MENGINE_DEBUG

        /*
        D3D11_RASTERIZER_DESC RasterState;
        pImmediateContext->RSGetState(&RasterState);

        DXCALL( m_pD3DDevice, GetRenderState, (D3DRS_FILLMODE, &fillmode) );

        if( fillmode != D3DFILL_WIREFRAME )
        {
            DXCALL( m_pD3DDevice, SetTexture, (_stage, m_pD3DTexture) );
        }*/

        pImmediateContext->PSSetShaderResources( _stage, 1, &m_pD3DResourceView );

#else
        pImmediateContext->PSSetShaderResources( _stage, 1, &m_pD3DResourceView );
#endif
        pImmediateContext->Release();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderImage::unbind( uint32_t _stage )
    {
		MENGINE_UNUSED(_stage);

        /*ID3D11DeviceContext * pImmediateContext = nullptr;
        m_pD3DDevice->GetImmediateContext( &pImmediateContext );

        pImmediateContext->PSSetShaderResources( _stage, 1, nullptr );

        pImmediateContext->Release();*/
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderImage::setRenderImageProvider( const RenderImageProviderInterfacePtr & _renderImageProvider )
    {
        m_renderImageProvider = _renderImageProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderImageProviderInterfacePtr & DX11RenderImage::getRenderImageProvider() const
    {
        return m_renderImageProvider;
    }
    ///////////////////////////////////////////////////////////////////////////
    Pointer DX11RenderImage::lock( size_t * const _pitch, uint32_t _level, const Rect & _rect, bool _readOnly )
    {
        MENGINE_UNUSED( _readOnly );

        uint32_t rect_width = _rect.getWidth();
        uint32_t rect_height = _rect.getHeight();

        uint32_t miplevel_width = rect_width >> _level;
        uint32_t miplevel_height = rect_height >> _level;

        size_t size = Helper::getTextureMemorySize( miplevel_width, miplevel_height, m_hwChannels, 1, m_hwPixelFormat );

		MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
			->createMemoryBuffer(MENGINE_DOCUMENT_FACTORABLE);

		MENGINE_ASSERTION_MEMORY_PANIC(memory, "invalid create memory (l %u w %u h %u c %u f %u)"
			, _level
			, m_textureDesc.Width
			, m_textureDesc.Height
			, m_hwChannels
			, m_hwPixelFormat
		);

		void * buffer = memory->newBuffer(size);

		MENGINE_ASSERTION_MEMORY_PANIC(buffer, "invalid new memory %zu (l %u w %u h %u c %u f %u)"
			, size
			, _level
			, m_textureDesc.Width
			, m_textureDesc.Height
			, m_hwChannels
			, m_hwPixelFormat
		);

        m_lockMemory = memory;

        m_pitch = size / miplevel_height;

        *_pitch = m_pitch;

		return buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderImage::unlock( uint32_t _level, bool _successful )
    {
        MENGINE_UNUSED( _successful );
        MENGINE_UNUSED( _level );

		// TODO: 1 mip only logic here
		D3D11_SUBRESOURCE_DATA InitialData;
		InitialData.pSysMem = m_lockMemory->getBuffer();
		InitialData.SysMemPitch = m_pitch;
		InitialData.SysMemSlicePitch = m_lockMemory->getSize();

		IF_DXCALL(m_pD3DDevice, CreateTexture2D, (&m_textureDesc, &InitialData, &m_pD3DTexture))
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

		m_lockMemory = nullptr;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    ID3D11Device * DX11RenderImage::getD3DDevice() const
    {
        ID3D11Device * device9 = this->getDirect3D11Device();

        return device9;
    }
    //////////////////////////////////////////////////////////////////////////
    ID3D11Texture2D * DX11RenderImage::getD3DTexture() const
    {
        return m_pD3DTexture;
    }
    //////////////////////////////////////////////////////////////////////////
    ID3D11ShaderResourceView * DX11RenderImage::getD3DShaderResource() const
    {
        return m_pD3DResourceView;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderImage::getHWWidth() const
    {
        return m_textureDesc.Width;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderImage::getHWHeight() const
    {
        return m_textureDesc.Height;
    }
    //////////////////////////////////////////////////////////////////////////
    EPixelFormat DX11RenderImage::getHWPixelFormat() const
    {
        return m_hwPixelFormat;
    }
    //////////////////////////////////////////////////////////////////////////
    float DX11RenderImage::getHWWidthInv() const
    {
        return m_hwWidthInv;
    }
    //////////////////////////////////////////////////////////////////////////
    float DX11RenderImage::getHWHeightInv() const
    {
        return m_hwHeightInv;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderImage::getHWChannels() const
    {
        return m_hwChannels;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderImage::getHWDepth() const
    {
        return 1; //ToDo
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderImage::getHWMipmaps() const
    {
        return m_textureDesc.MipLevels;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderImage::onRenderReset()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderImage::onRenderRestore()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    UnknownPointer DX11RenderImage::getRenderImageExtention()
    {
        return this;
    }
    //////////////////////////////////////////////////////////////////////////
}
