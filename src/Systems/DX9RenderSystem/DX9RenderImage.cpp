#include "DX9RenderImage.h"
#include "DX9ErrorHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX9RenderImage::DX9RenderImage()
        : m_pD3DDevice( nullptr )
        , m_pD3DTexture( nullptr )
        , m_mode( ERIM_NORMAL )
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
    DX9RenderImage::~DX9RenderImage()
    {
        this->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderImage::initialize( LPDIRECT3DDEVICE9 _pD3DDevice, LPDIRECT3DTEXTURE9 _d3dInterface, ERenderImageMode _mode, uint32_t _mipmaps, uint32_t _hwWidth, uint32_t _hwHeight, uint32_t _hwChannels, uint32_t _hwDepth, EPixelFormat _hwPixelFormat )
    {
        m_pD3DDevice = _pD3DDevice;
        m_pD3DTexture = _d3dInterface;

        m_mode = _mode;

        m_hwMipmaps = _mipmaps;
        m_hwWidth = _hwWidth;
        m_hwHeight = _hwHeight;
        m_hwChannels = _hwChannels;
        m_hwDepth = _hwDepth;
        m_hwPixelFormat = _hwPixelFormat;

        m_hwWidthInv = 1.f / (float)m_hwWidth;
        m_hwHeightInv = 1.f / (float)m_hwHeight;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderImage::finalize()
    {
        m_renderImageProvider = nullptr;

        DXRELEASE( m_pD3DTexture );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderImage::bind( uint32_t _stage )
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
    void DX9RenderImage::setRenderImageProvider( const RenderImageProviderInterfacePtr & _renderImageProvider )
    {
        m_renderImageProvider = _renderImageProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderImageProviderInterfacePtr & DX9RenderImage::getRenderImageProvider() const
    {
        return m_renderImageProvider;
    }
    ///////////////////////////////////////////////////////////////////////////
    Pointer DX9RenderImage::lock( size_t * _pitch, uint32_t _level, const Rect & _rect, bool _readOnly )
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
    bool DX9RenderImage::unlock( uint32_t _level, bool _successful )
    {
        MENGINE_UNUSED( _successful );

        IF_DXCALL( m_pD3DTexture, UnlockRect, (_level) )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    LPDIRECT3DDEVICE9 DX9RenderImage::getD3DDevice() const
    {
        return m_pD3DDevice;
    }
    //////////////////////////////////////////////////////////////////////////
    LPDIRECT3DTEXTURE9 DX9RenderImage::getD3DTexture() const
    {
        return m_pD3DTexture;
    }
    //////////////////////////////////////////////////////////////////////////
    ERenderImageMode DX9RenderImage::getMode() const
    {
        return m_mode;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX9RenderImage::getHWWidth() const
    {
        return m_hwWidth;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX9RenderImage::getHWHeight() const
    {
        return m_hwHeight;
    }
    //////////////////////////////////////////////////////////////////////////
    EPixelFormat DX9RenderImage::getHWPixelFormat() const
    {
        return m_hwPixelFormat;
    }
    //////////////////////////////////////////////////////////////////////////
    float DX9RenderImage::getHWWidthInv() const
    {
        return m_hwWidthInv;
    }
    //////////////////////////////////////////////////////////////////////////
    float DX9RenderImage::getHWHeightInv() const
    {
        return m_hwHeightInv;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX9RenderImage::getHWChannels() const
    {
        return m_hwChannels;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX9RenderImage::getHWDepth() const
    {
        return 1; //ToDo
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX9RenderImage::getHWMipmaps() const
    {
        return m_hwMipmaps;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderImage::onRenderReset()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderImage::onRenderRestore()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    UnknownPointer DX9RenderImage::getRenderImageExtention()
    {
        return this;
    }
}
