#include "DX9RenderImage.h"

#include "DX9RenderImageLockedFactoryStorage.h"

#include "DX9RenderErrorHelper.h"
#include "DX9RenderEnum.h"

#include "Kernel/TextureHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"
#include "Kernel/StatisticHelper.h"
#include "Kernel/PixelFormatHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX9RenderImage::DX9RenderImage()
        : m_pD3DTexture( nullptr )
        , m_hwMipmaps( 0 )
        , m_hwWidth( 0 )
        , m_hwHeight( 0 )
        , m_hwPixelFormat( PF_UNKNOWN )
        , m_hwWidthInv( 0.f )
        , m_hwHeightInv( 0.f )
        , m_upscalePow2( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX9RenderImage::~DX9RenderImage()
    {
        MENGINE_ASSERTION_FATAL( m_pD3DTexture == nullptr, "image not released" );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderImage::initialize( uint32_t _mipmaps, uint32_t _width, uint32_t _height, EPixelFormat _pixelFormat )
    {
        D3DFORMAT D3DFormat = Helper::toD3DFormat( _pixelFormat );

        uint32_t HWWidth = Helper::getTexturePow2( _width );
        uint32_t HWHeight = Helper::getTexturePow2( _height );

        MENGINE_ASSERTION_FATAL( HWWidth != 0 && HWHeight != 0, "width [%u] or height [%u] cannot be zero"
            , HWWidth
            , HWHeight
        );

        IDirect3DTexture9 * pD3DTexture = nullptr;
        MENGINE_IF_DXCALL( m_pD3DDevice, CreateTexture, (HWWidth, HWHeight, _mipmaps, 0, D3DFormat, D3DPOOL_MANAGED, &pD3DTexture, NULL) )
        {
            return false;
        }

        D3DSURFACE_DESC texDesc;
        MENGINE_IF_DXCALL( pD3DTexture, GetLevelDesc, (0, &texDesc) )
        {
            MENGINE_DXRELEASE( pD3DTexture );

            return false;
        }

        MENGINE_ASSERTION_FATAL( texDesc.Width != 0, "invalid width" );
        MENGINE_ASSERTION_FATAL( texDesc.Height != 0, "invalid height" );

        m_pD3DTexture = pD3DTexture;

        m_hwMipmaps = _mipmaps;
        m_hwWidth = texDesc.Width;
        m_hwHeight = texDesc.Height;

        m_hwPixelFormat = Helper::fromD3DFormat( texDesc.Format );

        m_hwWidthInv = 1.f / (float)m_hwWidth;
        m_hwHeightInv = 1.f / (float)m_hwHeight;

        m_upscalePow2 = _width != m_hwWidth || _height != m_hwHeight;

        STATISTIC_INC_INTEGER( STATISTIC_RENDER_TEXTURE_ALLOC_NEW );
        STATISTIC_INC_INTEGER( STATISTIC_RENDER_TEXTURE_ALLOC_COUNT );
        STATISTIC_ADD_INTEGER( STATISTIC_RENDER_TEXTURE_ALLOC_SIZE, Helper::getTextureMemorySize( m_hwWidth, m_hwHeight, m_hwPixelFormat ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderImage::finalize()
    {
        m_renderImageProvider = nullptr;

        if( m_pD3DTexture != nullptr )
        {
            STATISTIC_INC_INTEGER( STATISTIC_RENDER_TEXTURE_ALLOC_FREE );
            STATISTIC_DEC_INTEGER( STATISTIC_RENDER_TEXTURE_ALLOC_COUNT );
            STATISTIC_DEL_INTEGER( STATISTIC_RENDER_TEXTURE_ALLOC_SIZE, Helper::getTextureMemorySize( m_hwWidth, m_hwHeight, m_hwPixelFormat ) );

            MENGINE_DXRELEASE( m_pD3DTexture );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderImage::bind( uint32_t _stage )
    {
#if defined(MENGINE_DEBUG)
        DWORD fillmode;
        MENGINE_DXCALL( m_pD3DDevice, GetRenderState, (D3DRS_FILLMODE, &fillmode) );

        if( fillmode != D3DFILL_WIREFRAME )
        {
            MENGINE_DXCALL( m_pD3DDevice, SetTexture, (_stage, m_pD3DTexture) );
        }
#else
        MENGINE_DXCALL( m_pD3DDevice, SetTexture, (_stage, m_pD3DTexture) );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderImage::unbind( uint32_t _stage )
    {
        MENGINE_DXCALL( m_pD3DDevice, SetTexture, (_stage, nullptr) );
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
    RenderImageLockedInterfacePtr DX9RenderImage::lock( uint32_t _level, const Rect & _rect, bool _readOnly )
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
        MENGINE_IF_DXCALL( m_pD3DTexture, LockRect, (_level, &TRect, &rect, flags) )
        {
            return nullptr;
        }

        STATISTIC_INC_INTEGER( STATISTIC_RENDER_TEXTURE_LOCK_COUNT );
        STATISTIC_ADD_INTEGER( STATISTIC_RENDER_TEXTURE_LOCK_PIXEL, (rect.bottom - rect.top) * (rect.right - rect.left) );

        DX9RenderImageLockedPtr imageLocked = DX9RenderImageLockedFactoryStorage::createObject( MENGINE_DOCUMENT_FACTORABLE );

        imageLocked->initialize( TRect );

        return imageLocked;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderImage::unlock( const RenderImageLockedInterfacePtr & _locked, uint32_t _level, bool _successful )
    {
        MENGINE_UNUSED( _locked );
        MENGINE_UNUSED( _successful );

        MENGINE_IF_DXCALL( m_pD3DTexture, UnlockRect, (_level) )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    IDirect3DDevice9 * DX9RenderImage::getD3DDevice() const
    {
        IDirect3DDevice9 * device9 = this->getDirect3DDevice9();

        return device9;
    }
    //////////////////////////////////////////////////////////////////////////
    IDirect3DTexture9 * DX9RenderImage::getD3DTexture() const
    {
        return m_pD3DTexture;
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
    bool DX9RenderImage::getUpscalePow2() const
    {
        return m_upscalePow2;
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
}
