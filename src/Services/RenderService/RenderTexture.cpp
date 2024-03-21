#include "RenderTexture.h"

#include "Kernel/Logger.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/TextureHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    RenderTexture::RenderTexture()
        : m_id( 0 )
        , m_width( 0 )
        , m_height( 0 )
        , m_widthInv( 0.f )
        , m_heightInv( 0.f )
        , m_pow2( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTexture::~RenderTexture()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTexture::initialize( UniqueId _id, const RenderImageInterfacePtr & _image, uint32_t _width, uint32_t _height )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _image );

        MENGINE_ASSERTION_FATAL( _width != 0, "invalid width == 0" );
        MENGINE_ASSERTION_FATAL( _height != 0, "invalid height == 0" );

        m_id = _id;

        m_image = _image;

        m_width = _width;
        m_height = _height;

        m_widthInv = 1.f / (float)m_width;
        m_heightInv = 1.f / (float)m_height;

        uint32_t HWWidth = m_image->getHWWidth();
        uint32_t HWHeight = m_image->getHWHeight();

        float u = float( m_width ) / float( HWWidth );
        float v = float( m_height ) / float( HWHeight );

        mt::uv4_from_mask( &m_uv, mt::vec4f( 0.f, 0.f, u, v ) );

        m_pow2 = Helper::isTexturePow2( m_width ) == true && Helper::isTexturePow2( m_height ) == true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTexture::release()
    {
        m_image = nullptr;
        m_content = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderImageInterfacePtr & RenderTexture::getImage() const
    {
        return m_image;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t RenderTexture::getId() const
    {
        return m_id;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTexture::setContent( const ContentInterfacePtr & _content )
    {
        m_content = _content;
    }
    //////////////////////////////////////////////////////////////////////////
    const ContentInterfacePtr & RenderTexture::getContent() const
    {
        return m_content;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t RenderTexture::getWidth() const
    {
        return m_width;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t RenderTexture::getHeight() const
    {
        return m_height;
    }
    //////////////////////////////////////////////////////////////////////////
    float RenderTexture::getWidthInv() const
    {
        return m_widthInv;
    }
    //////////////////////////////////////////////////////////////////////////
    float RenderTexture::getHeightInv() const
    {
        return m_heightInv;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::uv4f & RenderTexture::getUV() const
    {
        return m_uv;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderTexture::isPow2() const
    {
        return m_pow2;
    }
    //////////////////////////////////////////////////////////////////////////
}
