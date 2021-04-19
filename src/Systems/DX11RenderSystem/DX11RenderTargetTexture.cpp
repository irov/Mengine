#include "DX11RenderTargetTexture.h"

#include "DX11RenderEnum.h"
#include "DX11ErrorHelper.h"

#include "Kernel/Assertion.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX11RenderTargetTexture::DX11RenderTargetTexture()
        : m_width( 0 )
        , m_height( 0 )
        , m_channels( 0 )
        , m_format( PF_UNKNOWN )
        , m_hwWidth( 0 )
        , m_hwHeight( 0 )
        , m_hwWidthInv( 0.f )
        , m_hwHeightInv( 0.f )
        , m_pD3DTexture( nullptr )
        , m_pD3DSurface( nullptr )
        , m_pD3DSurfaceOld( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX11RenderTargetTexture::~DX11RenderTargetTexture()
    {
        MENGINE_ASSERTION_FATAL( m_pD3DTexture == nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderTargetTexture::initialize( uint32_t _width, uint32_t _height, uint32_t _channels, EPixelFormat _format )
    {
        m_width = _width;
        m_height = _height;
        m_channels = _channels;
        m_format = _format;

        D3DFORMAT d3dformat = Helper::toD3DFormat( m_format );

        IDirect3DTexture9 * pD3DTexture;
        IF_DXCALL( m_pD3DDevice, CreateTexture, (m_width, m_height, 1, D3DUSAGE_RENDERTARGET, d3dformat, D3DPOOL_DEFAULT, &pD3DTexture, NULL) )
        {
            return false;
        }

        D3DSURFACE_DESC texDesc;
        IF_DXCALL( pD3DTexture, GetLevelDesc, (0, &texDesc) )
        {
            DXRELEASE( pD3DTexture );

            return false;
        }

        m_hwWidth = texDesc.Width;
        m_hwHeight = texDesc.Height;

        m_hwWidthInv = 1.f / (float)m_hwWidth;
        m_hwHeightInv = 1.f / (float)m_hwHeight;

        if( this->_initialize() == false )
        {
            DXRELEASE( pD3DTexture );

            return false;
        }

        m_pD3DTexture = pD3DTexture;

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
        return m_channels;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderTargetTexture::getHWDepth() const
    {
        return 1U;
    }
    //////////////////////////////////////////////////////////////////////////
    EPixelFormat DX11RenderTargetTexture::getHWPixelFormat() const
    {
        return m_format;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderTargetTexture::getHWMipmaps() const
    {
        return 1U;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderTargetTexture::getHWWidth() const
    {
        return m_hwWidth;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderTargetTexture::getHWHeight() const
    {
        return m_hwHeight;
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
        IDirect3DSurface9 * pD3DSurface;
        DXCALL( m_pD3DTexture, GetSurfaceLevel, (0, &pD3DSurface) );

        if( pD3DSurface == nullptr )
        {
            return false;
        }

        IDirect3DSurface9 * pD3DSurfaceOld;
        DXCALL( m_pD3DDevice, GetRenderTarget, (0, &pD3DSurfaceOld) );

        DXCALL( m_pD3DDevice, SetRenderTarget, (0, pD3DSurface) );

        m_pD3DSurfaceOld = pD3DSurfaceOld;
        m_pD3DSurface = pD3DSurface;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderTargetTexture::end()
    {
        DXCALL( m_pD3DDevice, SetRenderTarget, (0, m_pD3DSurfaceOld) );

        if( m_pD3DSurfaceOld != nullptr )
        {
            m_pD3DSurfaceOld->Release();
            m_pD3DSurfaceOld = nullptr;
        }

        m_pD3DSurface->Release();
        m_pD3DSurface = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderTargetTexture::getData( void * const _buffer, size_t _pitch ) const
    {
        MENGINE_UNUSED( _buffer );
        MENGINE_UNUSED( _pitch );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    ID3D11Device * DX11RenderTargetTexture::getDirect3dDevice9() const
    {
        return m_pD3DDevice;
    }
    //////////////////////////////////////////////////////////////////////////
    IDirect3DTexture9 * DX11RenderTargetTexture::getDirect3dTexture9() const
    {
        return m_pD3DTexture;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderTargetTexture::onRenderReset()
    {
        DXRELEASE( m_pD3DTexture );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderTargetTexture::onRenderRestore()
    {
        MENGINE_ASSERTION_FATAL( m_pD3DTexture == nullptr );

        D3DFORMAT D3DFormat = Helper::toD3DFormat( m_format );

        IDirect3DTexture9 * pD3DTexture;
        IF_DXCALL( m_pD3DDevice, CreateTexture, (m_width, m_height, 1, D3DUSAGE_RENDERTARGET, D3DFormat, D3DPOOL_DEFAULT, &pD3DTexture, nullptr) )
        {
            return false;
        }

        m_pD3DTexture = pD3DTexture;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}