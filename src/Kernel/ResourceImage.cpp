#include "ResourceImage.h"

#include "Interface/RenderTextureInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Assertion.h"

#include "Config/StdAlgorithm.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceImage::ResourceImage()
        : m_maxSize( -1.f, -1.f )
        , m_size( 0.f, 0.f )
        , m_offset( 0.f, 0.f )
        , m_hasAlpha( false )
        , m_isPremultiply( false )
        , m_trimAtlas( false )
        , m_isPow2( false )
    {
        StdAlgorithm::fill_n( m_uvRotate, MENGINE_RESOURCEIMAGE_MAX_TEXTURE, false );
        StdAlgorithm::fill_n( m_uvTexture, MENGINE_RESOURCEIMAGE_MAX_TEXTURE, mt::uv4f() );
        StdAlgorithm::fill_n( m_uvIdentity, MENGINE_RESOURCEIMAGE_MAX_TEXTURE, true );
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceImage::~ResourceImage()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImage::setTexture( size_t _index, const RenderTextureInterfacePtr & _texture )
    {
        MENGINE_ASSERTION_FATAL( _index < MENGINE_RESOURCEIMAGE_MAX_TEXTURE, "resource image '%s' index %zu out of range"
            , this->getName().c_str()
            , _index
        );

        m_texture[_index] = _texture;
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
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceImage::correctUV( size_t _index, const mt::vec2f & _in, mt::vec2f * const _out ) const
    {
        MENGINE_ASSERTION_FATAL( _index < MENGINE_RESOURCEIMAGE_MAX_TEXTURE, "resource image '%s' index %zu out of range"
            , this->getName().c_str()
            , _index
        );

        bool identity = this->isUVIdentity( _index );

        if( identity == true )
        {
            *_out = _in;

            return;
        }

        const mt::uv4f & uvTexture = m_uvTexture[_index];

        bool rotate = this->isUVRotate( _index );

        if( rotate == false )
        {
            mt::uv4_quad_point( _out, uvTexture, _in);
        }
        else
        {
            mt::uv4_quad_point_rotate( _out, uvTexture, _in );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
