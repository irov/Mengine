#include "ShapeQuadFlex.h"

#include "Kernel/Surface.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ShapeQuadFlex::ShapeQuadFlex()
        : m_customSize( -1.f, -1.f )
        , m_percentVisibility( 0.f, 0.f, 1.f, 1.f )
        , m_textureUVOffset( 0.f, 0.f )
        , m_textureUVScale( 1.f, 1.f )
        , m_centerAlign( false )
        , m_flipX( false )
        , m_flipY( false )
        , m_solid( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ShapeQuadFlex::~ShapeQuadFlex()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeQuadFlex::setCustomSize( const mt::vec2f & _customSize )
    {
        if( m_customSize == _customSize )
        {
            return;
        }

        m_customSize = _customSize;

        this->invalidateVerticesLocal();
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeQuadFlex::removeCustomSize()
    {
        this->setCustomSize( mt::vec2f( -1.f, -1.f ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ShapeQuadFlex::hasCustomSize() const
    {
        return m_customSize.x >= 0.f || m_customSize.y >= 0.f;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & ShapeQuadFlex::getCustomSize() const
    {
        return m_customSize;
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeQuadFlex::setCenterAlign( bool _centerAlign )
    {
        if( m_centerAlign == _centerAlign )
        {
            return;
        }

        m_centerAlign = _centerAlign;

        this->invalidateVerticesLocal();
    }
    //////////////////////////////////////////////////////////////////////////
    bool ShapeQuadFlex::getCenterAlign() const
    {
        return m_centerAlign;
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeQuadFlex::setFlipX( bool _flipX )
    {
        if( m_flipX == _flipX )
        {
            return;
        }

        m_flipX = _flipX;

        this->invalidateVerticesLocal();
    }
    //////////////////////////////////////////////////////////////////////////
    bool ShapeQuadFlex::getFlipX() const
    {
        return m_flipX;
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeQuadFlex::setFlipY( bool _flipY )
    {
        if( m_flipY == _flipY )
        {
            return;
        }

        m_flipY = _flipY;

        this->invalidateVerticesLocal();
    }
    //////////////////////////////////////////////////////////////////////////
    bool ShapeQuadFlex::getFlipY() const
    {
        return m_flipY;
    }
    ///////////////////////////////////////////////////////////////////////////
    void ShapeQuadFlex::setPercentVisibility( const mt::vec4f & _percent )
    {
        if( m_percentVisibility == _percent )
        {
            return;
        }

        m_percentVisibility = _percent;

        this->invalidateVerticesLocal();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec4f & ShapeQuadFlex::getPercentVisibility() const
    {
        return m_percentVisibility;
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeQuadFlex::setTextureUVOffset( const mt::vec2f & _offset )
    {
        if( m_textureUVOffset == _offset )
        {
            return;
        }

        m_textureUVOffset = _offset;

        this->invalidateVerticesLocal();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & ShapeQuadFlex::getTextureUVOffset() const
    {
        return m_textureUVOffset;
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeQuadFlex::setTextureUVScale( const mt::vec2f & _scale )
    {
        if( m_textureUVScale == _scale )
        {
            return;
        }

        m_textureUVScale = _scale;

        this->invalidateVerticesLocal();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & ShapeQuadFlex::getTextureUVScale() const
    {
        return m_textureUVScale;
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeQuadFlex::updateVerticesLocal() const
    {
        mt::vec2f maxSize;
        mt::vec2f size;
        mt::vec2f offset;

        if( this->hasCustomSize() == false )
        {
            maxSize = m_surface->getMaxSize();
            size = m_surface->getSize();
            offset = m_surface->getOffset();
        }
        else
        {
            maxSize = m_customSize;
            size = m_customSize;
            offset = mt::vec2f( 0.f, 0.f );
        }

        const mt::vec2f & anchor = m_surface->getAnchor();

        mt::vec2f total_offset = offset - anchor * maxSize;

        mt::vec4f percent_size = m_percentVisibility * size;

        mt::vec2f visOffset( percent_size.x, percent_size.y );

        if( m_centerAlign == true )
        {
            mt::vec2f alignOffset = -size * 0.5f;

            visOffset += alignOffset;
        }

        if( m_flipX == true )
        {
            visOffset.x += maxSize.x - (percent_size.z + total_offset.x);
        }
        else
        {
            visOffset.x += total_offset.x;
        }

        if( m_flipY == true )
        {
            visOffset.y += maxSize.y - (percent_size.w + total_offset.y);
        }
        else
        {
            visOffset.y += total_offset.y;
        }

        m_verticesLocal[0].x = visOffset.x + 0.f;
        m_verticesLocal[0].y = visOffset.y + 0.f;

        m_verticesLocal[1].x = visOffset.x + percent_size.z;
        m_verticesLocal[1].y = visOffset.y + 0.f;

        m_verticesLocal[2].x = visOffset.x + percent_size.z;
        m_verticesLocal[2].y = visOffset.y + percent_size.w;

        m_verticesLocal[3].x = visOffset.x + 0.f;
        m_verticesLocal[3].y = visOffset.y + percent_size.w;

        mt::uv4f uv_percentVisibility;
        uv4_from_mask( &uv_percentVisibility, m_percentVisibility );

        if( m_flipX == true )
        {
            mt::uv4_swap_u( &uv_percentVisibility );
        }

        if( m_flipY == true )
        {
            mt::uv4_swap_v( &uv_percentVisibility );
        }

        uint32_t uvCount = m_surface->getUVCount();

        for( uint32_t i = 0; i != uvCount; ++i )
        {
            for( uint32_t v = 0; v != 4; ++v )
            {
                const mt::vec2f & uv_correct = uv_percentVisibility[v];

                mt::vec2f uv_total = uv_correct * m_textureUVScale + m_textureUVOffset;

                m_surface->correctUV( i, uv_total, m_verticesWM[v].uv + i );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
}