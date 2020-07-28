#include "MockupRenderTargetTexture.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MockupRenderTargetTexture::MockupRenderTargetTexture()
        : m_width( 0 )
        , m_height( 0 )
        , m_hwWidth( 0 )
        , m_hwHeight( 0 )
        , m_hwWidthInv( 0.f )
        , m_hwHeightInv( 0.f )
        , m_hwChannels( 0 )
        , m_hwPixelFormat( PF_UNKNOWN )
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
        m_hwWidth = Helper::getTexturePOW2( _width );
        m_hwHeight = Helper::getTexturePOW2( _height );

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
    uint32_t MockupRenderTargetTexture::getHWChannels() const
    {
        return m_hwChannels;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MockupRenderTargetTexture::getHWDepth() const
    {
        return 1U;
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
    bool MockupRenderTargetTexture::begin()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MockupRenderTargetTexture::end()
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
}
