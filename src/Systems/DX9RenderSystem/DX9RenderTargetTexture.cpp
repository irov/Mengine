#include "DX9RenderTargetTexture.h"

#include "Interface/RenderSystemInterface.h"

#include "DX9RenderEnum.h"
#include "DX9RenderErrorHelper.h"

#include "Kernel/TextureHelper.h"
#include "Kernel/Assertion.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX9RenderTargetTexture::DX9RenderTargetTexture()
        : m_width( 0 )
        , m_height( 0 )
        , m_hwWidth( 0 )
        , m_hwHeight( 0 )
        , m_hwPixelFormat( PF_UNKNOWN )
        , m_hwWidthInv( 0.f )
        , m_hwHeightInv( 0.f )
        , m_pD3DTexture( nullptr )
        , m_pD3DSurfaceCurrent( nullptr )
        , m_pD3DSurfaceOld( nullptr )
        , m_upscalePow2( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX9RenderTargetTexture::~DX9RenderTargetTexture()
    {
        MENGINE_ASSERTION_FATAL( m_pD3DTexture == nullptr );
        MENGINE_ASSERTION_FATAL( m_pD3DSurfaceOld == nullptr );
        MENGINE_ASSERTION_FATAL( m_pD3DSurfaceCurrent == nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderTargetTexture::initialize( uint32_t _width, uint32_t _height, EPixelFormat _format )
    {
        D3DFORMAT d3dFormat = Helper::toD3DFormat( _format );

        uint32_t HWWidth = Helper::getTexturePow2( _width );
        uint32_t HWHeight = Helper::getTexturePow2( _height );

        IDirect3DTexture9 * pD3DTexture;
        IF_DXCALL( m_pD3DDevice, CreateTexture, (HWWidth, HWHeight, 1, D3DUSAGE_RENDERTARGET, d3dFormat, D3DPOOL_DEFAULT, &pD3DTexture, NULL) )
        {
            return false;
        }

        D3DSURFACE_DESC texDesc;
        IF_DXCALL( pD3DTexture, GetLevelDesc, (0, &texDesc) )
        {
            DXRELEASE( pD3DTexture );

            return false;
        }

        m_width = _width;
        m_height = _height;

        m_hwWidth = texDesc.Width;
        m_hwHeight = texDesc.Height;
        m_hwPixelFormat = Helper::fromD3DFormat( texDesc.Format );

        m_hwWidthInv = 1.f / (float)m_hwWidth;
        m_hwHeightInv = 1.f / (float)m_hwHeight;

        if( this->_initialize() == false )
        {
            DXRELEASE( pD3DTexture );

            return false;
        }

        m_pD3DTexture = pD3DTexture;

        m_upscalePow2 = m_hwWidth != _width || m_hwHeight != _height;

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
        MENGINE_ASSERTION_FATAL( m_pD3DSurfaceOld == nullptr );
        MENGINE_ASSERTION_FATAL( m_pD3DSurfaceCurrent == nullptr );

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
        return m_hwPixelFormat;
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
    bool DX9RenderTargetTexture::getUpscalePow2() const
    {
        return m_upscalePow2;
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

        D3DVIEWPORT9 VPOld;
        IF_DXCALL( m_pD3DDevice, GetViewport, (&VPOld) )
        {
            return false;
        }

        m_VPOld = VPOld;

        DXCALL( m_pD3DDevice, SetRenderTarget, (0, pD3DSurface) );

        m_pD3DSurfaceOld = pD3DSurfaceOld;
        m_pD3DSurfaceCurrent = pD3DSurface;

        D3DVIEWPORT9 VP;
        VP.X = 0;
        VP.Y = 0;
        VP.Width = m_width;
        VP.Height = m_height;
        VP.MinZ = 0.f;
        VP.MaxZ = 1.f;

        IF_DXCALL( m_pD3DDevice, SetViewport, (&VP) )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderTargetTexture::end() const
    {
        DXCALL( m_pD3DDevice, SetRenderTarget, (0, m_pD3DSurfaceOld) );

        DXCALL( m_pD3DDevice, SetViewport, (&m_VPOld) );

        if( m_pD3DSurfaceOld != nullptr )
        {
            m_pD3DSurfaceOld->Release();
            m_pD3DSurfaceOld = nullptr;
        }

        m_pD3DSurfaceCurrent->Release();
        m_pD3DSurfaceCurrent = nullptr;
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

        D3DFORMAT D3DFormat = Helper::toD3DFormat( m_hwPixelFormat );

        IDirect3DTexture9 * pD3DTexture;
        IF_DXCALL( m_pD3DDevice, CreateTexture, (m_hwWidth, m_hwHeight, 1, D3DUSAGE_RENDERTARGET, D3DFormat, D3DPOOL_DEFAULT, &pD3DTexture, nullptr) )
        {
            return false;
        }

        m_pD3DTexture = pD3DTexture;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}