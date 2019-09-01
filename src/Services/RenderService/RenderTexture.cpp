#include "RenderTexture.h"

#include "Kernel/Logger.h"

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
    void RenderTexture::initialize( uint32_t _id
        , const RenderImageInterfacePtr & _image
        , uint32_t _width
        , uint32_t _height )
    {
        m_id = _id;

        m_image = _image;

        m_width = _width;
        m_height = _height;

        m_widthInv = 1.f / (float)m_width;
        m_heightInv = 1.f / (float)m_height;

        m_rect.left = 0;
        m_rect.top = 0;
        m_rect.right = m_width;
        m_rect.bottom = m_height;

        uint32_t HWWidth = m_image->getHWWidth();
        uint32_t HWHeight = m_image->getHWHeight();

        float scaleU = float( m_rect.right ) / float( HWWidth );
        float scaleV = float( m_rect.bottom ) / float( HWHeight );

        mt::uv4_from_mask( m_uv, mt::vec4f( 0.f, 0.f, scaleU, scaleV ) );

        m_pow2 = Helper::isTexturePOW2( m_width ) && Helper::isTexturePOW2( m_height );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTexture::release()
    {
        m_image = nullptr;
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
    void RenderTexture::setFileGroup( const FileGroupInterfacePtr & _fileGroup )
    {
        m_fileGroup = _fileGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTexture::setFilePath( const FilePath & _filePath )
    {
        m_filePath = _filePath;
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
    const Rect & RenderTexture::getRect() const
    {
        return m_rect;
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
