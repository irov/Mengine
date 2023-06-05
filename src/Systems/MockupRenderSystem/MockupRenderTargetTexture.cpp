#include "MockupRenderTargetTexture.h"

#include "Kernel/TextureHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MockupRenderTargetTexture::MockupRenderTargetTexture()
        : m_hwWidth( 0 )
        , m_hwHeight( 0 )
        , m_hwPixelFormat( PF_UNKNOWN )
        , m_hwWidthInv( 0.f )
        , m_hwHeightInv( 0.f )
        , m_upscalePow2( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MockupRenderTargetTexture::~MockupRenderTargetTexture()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MockupRenderTargetTexture::initialize( uint32_t _width, uint32_t _height, EPixelFormat _format )
    {
        m_hwWidth = Helper::getTexturePow2( _width );
        m_hwHeight = Helper::getTexturePow2( _height );

        m_hwWidthInv = 1.f / (float)m_hwWidth;
        m_hwHeightInv = 1.f / (float)m_hwHeight;

        m_hwPixelFormat = _format;

        if( this->_initialize() == false )
        {
            return false;
        }

        float u = float( _width ) / float( m_hwWidth );
        float v = float( _height ) / float( m_hwHeight );

        mt::uv4_from_mask( &m_uv, mt::vec4f( 0.f, 0.f, u, v ) );

        m_upscalePow2 = m_hwWidth != _width || m_hwHeight != _height;

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
    bool MockupRenderTargetTexture::getUpscalePow2() const
    {
        return m_upscalePow2;
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
    const mt::uv4f & MockupRenderTargetTexture::getUV() const
    {
        return m_uv;
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
