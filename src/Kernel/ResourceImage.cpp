#include "ResourceImage.h"

#include "Interface/RenderTextureInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Assertion.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceImage::ResourceImage()
        : m_maxSize( -1.f, -1.f )
        , m_size( 0.f, 0.f )
        , m_offset( 0.f, 0.f )
        , m_uvImageRotate( false )
        , m_uvAlphaRotate( false )
        , m_hasAlpha( false )
        , m_isPremultiply( false )
        , m_trimAtlas( false )
        , m_isPow2( false )
        , m_isUVImageIdentity( false )
        , m_isUVAlphaIdentity( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceImage::~ResourceImage()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImage::setTexture( const RenderTextureInterfacePtr & _texture )
    {
        m_texture = _texture;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImage::setTextureAlpha( const RenderTextureInterfacePtr & _textureAlpha )
    {
        m_textureAlpha = _textureAlpha;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceImage::_compile()
    {
        MENGINE_ASSERTION_FATAL( m_maxSize.x >= 0.f && m_maxSize.y >= 0.f, "image type '%s' name '%s' not set max size"
            , this->getType().c_str()
            , this->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImage::_release()
    {
        if( m_texture != nullptr )
        {
            m_texture = nullptr;
        }

        if( m_textureAlpha != nullptr )
        {
            m_textureAlpha = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImage::correctUVImage( const mt::vec2f & _in, mt::vec2f * const _out ) const
    {
        if( m_isUVImageIdentity == true )
        {
            *_out = _in;

            return;
        }

        if( m_uvImageRotate == false )
        {
            mt::uv4_quad_point( _out, m_uvTextureImage, _in );
        }
        else
        {
            mt::uv4_quad_point_rotate( _out, m_uvTextureImage, _in );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImage::correctUVAlpha( const mt::vec2f & _in, mt::vec2f * const _out ) const
    {
        if( m_isUVAlphaIdentity == true )
        {
            *_out = _in;
        }

        if( m_uvAlphaRotate == false )
        {
            mt::uv4_quad_point( _out, m_uvTextureAlpha, _in );
        }
        else
        {
            mt::uv4_quad_point_rotate( _out, m_uvTextureImage, _in );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
