#include "MockupRenderImage.h"

#include "MockupRenderImageLockedFactoryStorage.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MockupRenderImage::MockupRenderImage()
        : m_hwMipmaps( 0 )
        , m_hwWidth( 0 )
        , m_hwHeight( 0 )
        , m_hwPixelFormat( PF_UNKNOWN )
        , m_hwWidthInv( 0.f )
        , m_hwHeightInv( 0.f )
        , m_upscalePow2( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MockupRenderImage::~MockupRenderImage()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderImage::initialize( uint32_t _mipmaps, uint32_t _width, uint32_t _height, EPixelFormat _pixelFormat )
    {
        m_hwMipmaps = _mipmaps;
        m_hwWidth = _width;
        m_hwHeight = _height;
        m_hwPixelFormat = _pixelFormat;

        m_hwWidthInv = 1.f / (float)m_hwWidth;
        m_hwHeightInv = 1.f / (float)m_hwHeight;

        m_upscalePow2 = false;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderImage::finalize()
    {        
        m_renderImageProvider = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderImage::setRenderImageProvider( const RenderImageProviderInterfacePtr & _renderImageProvider )
    {
        m_renderImageProvider = _renderImageProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderImageProviderInterfacePtr & MockupRenderImage::getRenderImageProvider() const
    {
        return m_renderImageProvider;
    }
    ///////////////////////////////////////////////////////////////////////////
    RenderImageLockedInterfacePtr MockupRenderImage::lock( uint32_t _level, const Rect & _rect, bool _readOnly )
    {
        MENGINE_UNUSED( _level );
        MENGINE_UNUSED( _readOnly );

        MockupRenderImageLockedPtr locked = MockupRenderImageLockedFactoryStorage::createObject( MENGINE_DOCUMENT_FACTORABLE );

        locked->initialize( _rect, m_hwPixelFormat );

        return locked;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MockupRenderImage::unlock( const RenderImageLockedInterfacePtr & _locked, uint32_t _level, bool _successful )
    {
        MENGINE_UNUSED( _locked );
        MENGINE_UNUSED( _level );
        MENGINE_UNUSED( _successful );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MockupRenderImage::getHWWidth() const
    {
        return m_hwWidth;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MockupRenderImage::getHWHeight() const
    {
        return m_hwHeight;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MockupRenderImage::getHWMipmaps() const
    {
        return m_hwMipmaps;
    }

    //////////////////////////////////////////////////////////////////////////
    EPixelFormat MockupRenderImage::getHWPixelFormat() const
    {
        return m_hwPixelFormat;
    }
    //////////////////////////////////////////////////////////////////////////
    float MockupRenderImage::getHWWidthInv() const
    {
        return m_hwWidthInv;
    }
    //////////////////////////////////////////////////////////////////////////
    float MockupRenderImage::getHWHeightInv() const
    {
        return m_hwHeightInv;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MockupRenderImage::getUpscalePow2() const
    {
        return m_upscalePow2;
    }
    //////////////////////////////////////////////////////////////////////////
}
