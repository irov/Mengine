#include "DX11RenderImage.h"

#include "DX11ErrorHelper.h"
#include "DX11RenderEnum.h"

#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX11RenderImage::DX11RenderImage()
        : m_pD3DTexture( nullptr )
        , m_hwMipmaps( 0 )
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
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderImage::initialize( uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, EPixelFormat _pixelFormat )
    {
        D3DFORMAT D3DFormat = Helper::toD3DFormat( _pixelFormat );

        IDirect3DTexture9 * pD3DTexture = nullptr;
        IF_DXCALL( m_pD3DDevice, CreateTexture, (_width, _height, _mipmaps, 0, D3DFormat, D3DPOOL_MANAGED, &pD3DTexture, NULL) )
        {
            return false;
        }

        D3DSURFACE_DESC texDesc;
        IF_DXCALL( pD3DTexture, GetLevelDesc, (0, &texDesc) )
        {
            DXRELEASE( pD3DTexture );

            return false;
        }

        MENGINE_ASSERTION_FATAL( texDesc.Width != 0 );
        MENGINE_ASSERTION_FATAL( texDesc.Height != 0 );

        m_pD3DTexture = pD3DTexture;

        m_hwMipmaps = _mipmaps;
        m_hwWidth = texDesc.Width;
        m_hwHeight = texDesc.Height;
        m_hwChannels = _channels;
        m_hwDepth = _depth;
        m_hwPixelFormat = _pixelFormat;

        m_hwWidthInv = 1.f / (float)m_hwWidth;
        m_hwHeightInv = 1.f / (float)m_hwHeight;

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
#ifdef MENGINE_DEBUG
        DWORD fillmode;
        DXCALL( m_pD3DDevice, GetRenderState, (D3DRS_FILLMODE, &fillmode) );

        if( fillmode != D3DFILL_WIREFRAME )
        {
            DXCALL( m_pD3DDevice, SetTexture, (_stage, m_pD3DTexture) );
        }
#else
        DXCALL( m_pD3DDevice, SetTexture, (_stage, m_pD3DTexture) );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderImage::unbind( uint32_t _stage )
    {
        DXCALL( m_pD3DDevice, SetTexture, (_stage, nullptr) );
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
        DWORD flags;
        if( _readOnly == true )
        {
            flags = D3DLOCK_READONLY;
        }
        else
        {
            flags = 0;
        }

        RECT rect;
        rect.top = _rect.top;
        rect.bottom = _rect.bottom;
        rect.left = _rect.left;
        rect.right = _rect.right;

        D3DLOCKED_RECT TRect;
        IF_DXCALL( m_pD3DTexture, LockRect, (_level, &TRect, &rect, flags) )
        {
            return nullptr;
        }

        *_pitch = (size_t)TRect.Pitch;

        void * bits = TRect.pBits;

        return bits;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderImage::unlock( uint32_t _level, bool _successful )
    {
        MENGINE_UNUSED( _successful );

        IF_DXCALL( m_pD3DTexture, UnlockRect, (_level) )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    IDirect3DDevice9 * DX11RenderImage::getD3DDevice() const
    {
        IDirect3DDevice9 * device9 = this->getDirect3DDevice9();

        return device9;
    }
    //////////////////////////////////////////////////////////////////////////
    IDirect3DTexture9 * DX11RenderImage::getD3DTexture() const
    {
        return m_pD3DTexture;
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
    UnknownPointer DX11RenderImage::getRenderImageExtention()
    {
        return this;
    }
    //////////////////////////////////////////////////////////////////////////
}
