#include "MockupRenderTargetTexture.h"

#include "Kernel/TextureHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MockupRenderTargetTexture::MockupRenderTargetTexture()
        : m_width( 0 )
        , m_height( 0 )
        , m_hwChannels( 0 )
        , m_hwPixelFormat( PF_UNKNOWN )
        , m_hwWidth( 0 )
        , m_hwHeight( 0 )
        , m_hwWidthInv( 0.f )
        , m_hwHeightInv( 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MockupRenderTargetTexture::~MockupRenderTargetTexture()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MockupRenderTargetTexture::initialize( uint32_t _width, uint32_t _height, uint32_t _channels, EPixelFormat _format )
    {
        m_width = _width;
        m_height = _height;
        m_hwWidth = Helper::getTexturePow2( _width );
        m_hwHeight = Helper::getTexturePow2( _height );

        m_hwWidthInv = 1.f / (float)m_hwWidth;
        m_hwHeightInv = 1.f / (float)m_hwHeight;

        m_hwChannels = _channels;
        m_hwPixelFormat = _format;

        if( this->_initialize() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MockupRenderTargetTexture::_initialize()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderTargetTexture::finalize()
    {
        this->_finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderTargetTexture::_finalize()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MockupRenderTargetTexture::getHWMipmaps() const
    {
        return 0U;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MockupRenderTargetTexture::getHWWidth() const
    {
        return m_hwWidth;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MockupRenderTargetTexture::getHWHeight() const
    {
        return m_hwHeight;
    }
    //////////////////////////////////////////////////////////////////////////
    EPixelFormat MockupRenderTargetTexture::getHWPixelFormat() const
    {
        return m_hwPixelFormat;
    }
    //////////////////////////////////////////////////////////////////////////
    float MockupRenderTargetTexture::getHWWidthInv() const
    {
        return m_hwWidthInv;
    }
    //////////////////////////////////////////////////////////////////////////
    float MockupRenderTargetTexture::getHWHeightInv() const
    {
        return m_hwHeightInv;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderTargetTexture::calcViewport( const mt::vec2f & _size, Viewport * const _viewport ) const
    {
        float uv_width = _size.x * m_hwWidthInv;
        float uv_height = _size.y * m_hwHeightInv;

        _viewport->begin = mt::vec2f( 0.f, 0.f );
        _viewport->end = mt::vec2f( uv_width, uv_height );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MockupRenderTargetTexture::begin() const
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderTargetTexture::end() const
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool MockupRenderTargetTexture::getData( void * const _buffer, size_t _pitch ) const
    {
        MENGINE_UNUSED( _buffer );
        MENGINE_UNUSED( _pitch );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}
