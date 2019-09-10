#include "DX9RenderTargetTexture.h"

#include "DX9RenderEnum.h"
#include "DX9ErrorHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX9RenderTargetTexture::DX9RenderTargetTexture()
        : m_width( 0 )
        , m_height( 0 )
        , m_channels( 0 )
        , m_format( PF_UNKNOWN )
        , m_pD3DDevice( nullptr )
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
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderTargetTexture::initialize( LPDIRECT3DDEVICE9 _device, uint32_t _width, uint32_t _height, uint32_t _channels, EPixelFormat _format )
    {
        m_pD3DDevice = _device;

        m_width = _width;
        m_height = _height;
        m_channels = _channels;
        m_format = _format;

        D3DFORMAT d3dformat = s_toD3DFormat( m_format );

        LPDIRECT3DTEXTURE9 renderTexture;
        IF_DXCALL( m_pD3DDevice, CreateTexture, (m_width, m_height, 1, D3DUSAGE_RENDERTARGET, d3dformat, D3DPOOL_DEFAULT, &renderTexture, NULL) )
        {
            return false;
        }

        D3DSURFACE_DESC texDesc;
        IF_DXCALL( renderTexture, GetLevelDesc, (0, &texDesc) )
        {
            return false;
        }

        m_hwWidth = texDesc.Width;
        m_hwHeight = texDesc.Height;

        m_hwWidthInv = 1.f / (float)m_hwWidth;
        m_hwHeightInv = 1.f / (float)m_hwHeight;

        if( this->_initialize() == false )
        {
            return false;
        }

        m_pD3DTexture = renderTexture;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderTargetTexture::_initialize()
    {
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
    uint32_t DX9RenderTargetTexture::getWidth() const
    {
        return m_width;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX9RenderTargetTexture::getHeight() const
    {
        return m_height;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX9RenderTargetTexture::getChannels() const
    {
        return m_channels;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX9RenderTargetTexture::getDepth() const
    {
        return 1U;
    }
    //////////////////////////////////////////////////////////////////////////
    EPixelFormat DX9RenderTargetTexture::getPixelFormat() const
    {
        return m_format;
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
    bool DX9RenderTargetTexture::begin()
    {
        LPDIRECT3DSURFACE9 pD3DSurface;
        DXCALL( m_pD3DTexture, GetSurfaceLevel, (0, &pD3DSurface) );

        if( pD3DSurface == nullptr )
        {
            return false;
        }

        LPDIRECT3DSURFACE9 pD3DSurfaceOld;
        DXCALL( m_pD3DDevice, GetRenderTarget, (0, &pD3DSurfaceOld) );

        DXCALL( m_pD3DDevice, SetRenderTarget, (0, pD3DSurface) );

        m_pD3DSurfaceOld = pD3DSurfaceOld;
        m_pD3DSurface = pD3DSurface;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderTargetTexture::end()
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
    bool DX9RenderTargetTexture::getData( void * _buffer, size_t _pitch )
    {
        MENGINE_UNUSED( _buffer );
        MENGINE_UNUSED( _pitch );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    LPDIRECT3DDEVICE9 DX9RenderTargetTexture::getDirect3dDevice9() const
    {
        return m_pD3DDevice;
    }
    //////////////////////////////////////////////////////////////////////////
    LPDIRECT3DTEXTURE9 DX9RenderTargetTexture::getDirect3dTexture9() const
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
        D3DFORMAT d3dformat = s_toD3DFormat( m_format );

        LPDIRECT3DTEXTURE9 renderTexture;
        IF_DXCALL( m_pD3DDevice, CreateTexture, (m_width, m_height, 1, D3DUSAGE_RENDERTARGET, d3dformat, D3DPOOL_DEFAULT, &renderTexture, NULL) )
        {
            return false;
        }

        m_pD3DTexture = renderTexture;

        return true;
    }
}
