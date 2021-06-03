#include "DX9RenderTargetTexture.h"

#include "DX9RenderEnum.h"
#include "DX9ErrorHelper.h"

#include "Kernel/Assertion.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX9RenderTargetTexture::DX9RenderTargetTexture()
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
    DX9RenderTargetTexture::~DX9RenderTargetTexture()
    {
        MENGINE_ASSERTION_FATAL( m_pD3DTexture == nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderTargetTexture::initialize( uint32_t _width, uint32_t _height, uint32_t _channels, EPixelFormat _format )
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
    bool DX9RenderTargetTexture::_initialize()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderTargetTexture::finalize()
    {
        DXRELEASE( m_pD3DTexture );

        this->_finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderTargetTexture::_finalize()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    EPixelFormat DX9RenderTargetTexture::getHWPixelFormat() const
    {
        return m_format;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX9RenderTargetTexture::getHWMipmaps() const
    {
        return 1U;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX9RenderTargetTexture::getHWWidth() const
    {
        return m_hwWidth;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX9RenderTargetTexture::getHWHeight() const
    {
        return m_hwHeight;
    }
    //////////////////////////////////////////////////////////////////////////
    float DX9RenderTargetTexture::getHWWidthInv() const
    {
        return m_hwWidthInv;
    }
    //////////////////////////////////////////////////////////////////////////
    float DX9RenderTargetTexture::getHWHeightInv() const
    {
        return m_hwHeightInv;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderTargetTexture::calcViewport( const mt::vec2f & _size, Viewport * const _viewport ) const
    {
        float uv_width = _size.x * m_hwWidthInv;
        float uv_height = _size.y * m_hwHeightInv;

        _viewport->begin = mt::vec2f( 0.f, 0.f );
        _viewport->end = mt::vec2f( uv_width, uv_height );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderTargetTexture::begin() const
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
    void DX9RenderTargetTexture::end() const
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
    bool DX9RenderTargetTexture::getData( void * const _buffer, size_t _pitch ) const
    {
        MENGINE_UNUSED( _buffer );
        MENGINE_UNUSED( _pitch );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    IDirect3DDevice9 * DX9RenderTargetTexture::getDirect3dDevice9() const
    {
        return m_pD3DDevice;
    }
    //////////////////////////////////////////////////////////////////////////
    IDirect3DTexture9 * DX9RenderTargetTexture::getDirect3dTexture9() const
    {
        return m_pD3DTexture;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderTargetTexture::onRenderReset()
    {
        DXRELEASE( m_pD3DTexture );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderTargetTexture::onRenderRestore()
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