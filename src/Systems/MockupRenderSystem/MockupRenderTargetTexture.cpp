#include "MockupRenderTargetTexture.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MockupRenderTargetTexture::MockupRenderTargetTexture()
        : m_width( 0 )
        , m_height( 0 )
        , m_channels( 0 )
        , m_format( PF_UNKNOWN )
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
        m_channels = _channels;
        m_format = _format;

        m_hwWidth = _width;
        m_hwHeight = _height;

        m_hwWidthInv = 1.f / (float)m_hwWidth;
        m_hwHeightInv = 1.f / (float)m_hwHeight;

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
    uint32_t MockupRenderTargetTexture::getWidth() const
    {
        return m_width;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MockupRenderTargetTexture::getHeight() const
    {
        return m_height;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MockupRenderTargetTexture::getChannels() const
    {
        return m_channels;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MockupRenderTargetTexture::getDepth() const
    {
        return 1U;
    }
    //////////////////////////////////////////////////////////////////////////
    EPixelFormat MockupRenderTargetTexture::getPixelFormat() const
    {
        return m_format;
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
    bool MockupRenderTargetTexture::getData( void * _buffer, size_t _pitch )
    {
        MENGINE_UNUSED( _buffer );
        MENGINE_UNUSED( _pitch );

        return false;
    }
}
