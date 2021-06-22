#include "DX11RenderImage.h"

#include "DX11RenderImageLockedFactoryStorage.h"

#include "DX11RenderErrorHelper.h"
#include "DX11RenderEnum.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/TextureHelper.h"
#include "Kernel/PixelFormatHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX11RenderImage::DX11RenderImage()
        : m_hwMipmaps( 0 )
        , m_hwWidth( 0 )
        , m_hwHeight( 0 )
        , m_hwChannels( 0 )
        , m_hwDepth( 0 )
        , m_hwPixelFormat( PF_UNKNOWN )
        , m_hwWidthInv( 0.f )
        , m_hwHeightInv( 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX11RenderImage::~DX11RenderImage()
    {
        MENGINE_ASSERTION_FATAL( m_pD3DTexture == nullptr );
        MENGINE_ASSERTION_FATAL( m_pD3DResourceView == nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderImage::initialize( uint32_t _mipmaps, uint32_t _width, uint32_t _height, EPixelFormat _pixelFormat )
    {
        DXGI_FORMAT d3dFormat = Helper::toD3DFormat( _pixelFormat );

        if( d3dFormat == DXGI_FORMAT_UNKNOWN )
        {
            // TODO: handle log
            return false;
        }

        m_hwPixelFormat = Helper::toPixelFormat( d3dFormat );

        uint32_t hwChannels = Helper::getPixelFormatChannels( m_hwPixelFormat );
        uint32_t hwDepth = Helper::getPixelFormatDepth( m_hwPixelFormat );

        m_hwMipmaps = _mipmaps;
        m_hwWidth = Helper::getTexturePow2( _width );
        m_hwHeight = Helper::getTexturePow2( _height );
        m_hwChannels = hwChannels;
        m_hwDepth = hwDepth;

        m_hwWidthInv = 1.f / (float)m_hwWidth;
        m_hwHeightInv = 1.f / (float)m_hwHeight;

        D3D11_TEXTURE2D_DESC textureDesc;
        ZeroMemory( &textureDesc, sizeof( D3D11_TEXTURE2D_DESC ) );

        textureDesc.Width = m_hwWidth;
        textureDesc.Height = m_hwHeight;
        textureDesc.MipLevels = m_hwMipmaps;
        textureDesc.ArraySize = 1;
        textureDesc.Format = d3dFormat;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.SampleDesc.Quality = 0;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        textureDesc.CPUAccessFlags = 0;
        textureDesc.MiscFlags = 0;

        const ID3D11DevicePtr & pD3DDevice = this->getDirect3D11Device();

        ID3D11Texture2D * pD3DTexture;
        IF_DXCALL( pD3DDevice, CreateTexture2D, (&textureDesc, nullptr, &pD3DTexture) )
        {
            return false;
        }

        m_pD3DTexture.Attach( pD3DTexture );

        D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
        ZeroMemory( &shaderResourceViewDesc, sizeof( D3D11_SHADER_RESOURCE_VIEW_DESC ) );

        shaderResourceViewDesc.Format = textureDesc.Format;
        shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
        shaderResourceViewDesc.Texture2D.MipLevels = textureDesc.MipLevels;

        ID3D11ShaderResourceView * pD3DResourceView;
        IF_DXCALL( pD3DDevice, CreateShaderResourceView, (m_pD3DTexture.Get(), &shaderResourceViewDesc, &pD3DResourceView) )
        {
            return false;
        }

        m_pD3DResourceView.Attach( pD3DResourceView );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderImage::finalize()
    {
        m_renderImageProvider = nullptr;

        m_pD3DTexture = nullptr;
        m_pD3DResourceView = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderImage::bind( const ID3D11DeviceContextPtr & _pImmediateContext, uint32_t _stage )
    {
        ID3D11ShaderResourceView * pD3DResourceView = m_pD3DResourceView.Get();

#ifdef MENGINE_DEBUG

        /*
        D3D11_RASTERIZER_DESC RasterState;
        pImmediateContext->RSGetState(&RasterState);

        DXCALL( m_pD3DDevice, GetRenderState, (D3DRS_FILLMODE, &fillmode) );

        if( fillmode != D3DFILL_WIREFRAME )
        {
            DXCALL( m_pD3DDevice, SetTexture, (_stage, m_pD3DTexture) );
        }*/



        _pImmediateContext->PSSetShaderResources( _stage, 1, &pD3DResourceView );

#else
        _pImmediateContext->PSSetShaderResources( _stage, 1, &pD3DResourceView );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderImage::unbind( const ID3D11DeviceContextPtr & _pImmediateContext, uint32_t _stage )
    {
        _pImmediateContext->PSSetShaderResources( _stage, 1, nullptr );
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
    RenderImageLockedInterfacePtr DX11RenderImage::lock( uint32_t _level, const Rect & _rect, bool _readOnly )
    {
        MENGINE_UNUSED( _readOnly );

        uint32_t rect_offsetX = _rect.getOffsetX();
        uint32_t rect_offsetY = _rect.getOffsetY();
        uint32_t rect_width = _rect.getWidth();
        uint32_t rect_height = _rect.getHeight();

        uint32_t miplevel_offsetX = rect_offsetX >> _level;
        uint32_t miplevel_offsetY = rect_offsetY >> _level;
        uint32_t miplevel_width = rect_width >> _level;
        uint32_t miplevel_height = rect_height >> _level;

        DX11RenderImageLockedPtr imageLocked = DX11RenderImageLockedFactoryStorage::createObject( MENGINE_DOCUMENT_FACTORABLE );

        const ID3D11DevicePtr & pD3DDevice = this->getDirect3D11Device();

        if( imageLocked->initialize( pD3DDevice, m_pD3DTexture, miplevel_offsetX, miplevel_offsetY, miplevel_width, miplevel_height ) == false )
        {
            return nullptr;
        }

        ID3D11DeviceContextPtr pImmediateContext = this->getDirect3D11ImmediateContext();

        if( imageLocked->lock( pImmediateContext ) == false )
        {
            return nullptr;
        }

        return imageLocked;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderImage::unlock( const RenderImageLockedInterfacePtr & _locked, uint32_t _level, bool _successful )
    {
        MENGINE_UNUSED( _level );

        DX11RenderImageLocked * imageLocked = _locked.getT<DX11RenderImageLocked *>();

        ID3D11DeviceContextPtr pImmediateContext = this->getDirect3D11ImmediateContext();

        if( _successful == false )
        {
            imageLocked->unlock( pImmediateContext, nullptr );
        }
        else
        {
            imageLocked->unlock( pImmediateContext, m_pD3DTexture );
        }

        imageLocked->finalize();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const ID3D11DevicePtr & DX11RenderImage::getD3DDevice() const
    {
        const ID3D11DevicePtr & pD3DDevice = this->getDirect3D11Device();

        return pD3DDevice;
    }
    //////////////////////////////////////////////////////////////////////////
    const ID3D11Texture2DPtr & DX11RenderImage::getD3DTexture() const
    {
        return m_pD3DTexture;
    }
    //////////////////////////////////////////////////////////////////////////
    const ID3D11ShaderResourceViewPtr & DX11RenderImage::getD3DShaderResource() const
    {
        return m_pD3DResourceView;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderImage::getHWWidth() const
    {
        return m_hwWidth;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderImage::getHWHeight() const
    {
        return m_hwHeight;
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
    bool DX11RenderImage::getUpscalePow2() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderImage::getHWMipmaps() const
    {
        return m_hwMipmaps;
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
}
