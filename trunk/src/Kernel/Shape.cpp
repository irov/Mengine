#   include "Shape.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    Shape::Shape()
        : m_textureWrapX(false)
        , m_textureWrapY(false)
        , m_size(0.f, 0.f)
        , m_maxSize(0.f, 0.f)
        , m_uvRotate(false)
        , m_uv(0.f, 0.f, 1.f, 1.f)
        , m_uv2(0.f, 0.f, 1.f, 1.f)
        , m_centerAlign(false)
        , m_flipX(false)
        , m_flipY(false)
        , m_percentVisibility(0.f, 0.f, 0.f, 0.f)
        , m_textureUVOffset(0.f, 0.f)
        , m_textureUVScale(1.f, 1.f)
        , m_invalidateVerticesLocal(true)
        , m_invalidateVerticesWM(true)
        , m_invalidateVerticesColor(true)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Shape::~Shape()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::setTextureWrapX( bool _wrap )
    {
        m_textureWrapX = _wrap;

        this->invalidateVertices();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Shape::getTextureWrapX() const
    {
        return m_textureWrapX;
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::setTextureWrapY( bool _wrap )
    {
        m_textureWrapY = _wrap;

        this->invalidateVertices();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Shape::getTextureWrapY() const
    {
        return m_textureWrapY;
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::setSize( const mt::vec2f & _size )
    {
        m_size = _size;

        this->invalidateVertices();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & Shape::getSize() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::setMaxSize( const mt::vec2f & _size )
    {
        m_maxSize = _size;

        this->invalidateVertices();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & Shape::getMaxSize() const
    {
        return m_maxSize;
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::setUVRotate( bool _rotate )
    {
        m_uvRotate = _rotate;

        this->invalidateVertices();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Shape::getUVRotate() const
    {
        return m_uvRotate;
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::setUV( const mt::vec4f & _uv )
    {
        m_uv = _uv;

        this->invalidateVertices();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec4f & Shape::getUV() const
    {
        return m_uv;
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::setUV2( const mt::vec4f & _uv )
    {
        m_uv2 = _uv;

        this->invalidateVertices();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec4f & Shape::getUV2() const
    {
        return m_uv2;
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::setCenterAlign( bool _centerAlign )
    {
        m_centerAlign = _centerAlign;

        this->invalidateVertices();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Shape::getCenterAlign() const
    {
        return m_centerAlign;
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::setFlipX( bool _flipX )
    {
        m_flipX = _flipX;

        this->invalidateVertices();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Shape::getFlipY() const
    {
        return m_flipY;
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::setFlipY( bool _flipY )
    {
        m_flipY = _flipY;

        this->invalidateVertices();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Shape::getFlipX() const
    {
        return m_flipX;
    }
    ///////////////////////////////////////////////////////////////////////////
    void Shape::setPercentVisibility( const mt::vec4f& _percent )
    {
        m_percentVisibility = _percent;

        this->invalidateVertices();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec4f & Shape::getPercentVisibility() const
    {
        return m_percentVisibility;
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::setTextureUVOffset( const mt::vec2f & _offset )
    {
        m_textureUVOffset = _offset;

        this->invalidateVertices();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & Shape::getTextureUVOffset() const
    {
        return m_textureUVOffset;
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::setTextureUVScale( const mt::vec2f & _scale )
    {
        m_textureUVScale = _scale;

        this->invalidateVertices();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & Shape::getTextureUVScale() const
    {
        return m_textureUVScale;
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::_invalidateWorldMatrix()
    {
        Node::_invalidateWorldMatrix();

        this->invalidateVerticesWM();
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::invalidateVertices()
    {
        m_invalidateVerticesLocal = true;

        this->invalidateVerticesWM();
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::invalidateVerticesWM()
    {
        m_invalidateVerticesWM = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::updateVertices()
    {
        m_invalidateVerticesLocal = false;

        mt::vec4f percentPx( m_percentVisibility.x * m_maxSize.x, m_percentVisibility.y * m_maxSize.y,
            m_percentVisibility.z * m_maxSize.x, m_percentVisibility.w * m_maxSize.y );

        percentPx.z -= (m_maxSize.x - m_size.x);
        percentPx.w -= (m_maxSize.y - m_size.y);

        if( m_textureWrapX == false && m_textureWrapY == false )
        {
            if( percentPx.x < 0.f )
            {
                percentPx.x = 0.f;
            }
            else if( percentPx.x > m_size.x )
            {
                percentPx.x = m_size.x;
            }

            if( percentPx.y < 0.f )
            {
                percentPx.y = 0.f;
            }
            else if( percentPx.y > m_size.y )
            {
                percentPx.y = m_size.y;
            }

            if( percentPx.z < 0.f )
            {
                percentPx.z = 0.f;
            }
            else if( percentPx.z > m_size.x )
            {
                percentPx.z = m_size.x;
            }

            if( percentPx.w < 0.f )
            {
                percentPx.w = 0.f;
            }
            else if( percentPx.w > m_size.y )
            {
                percentPx.w = m_size.y;
            }
        }

        mt::vec4f percent( percentPx.x / m_size.x, percentPx.y / m_size.y,
            percentPx.z / m_size.x, percentPx.w / m_size.y );

        if( m_uvRotate == false )
        {
            // RGB(A)
            {
                mt::vec4f uv = m_uv;

                float uvX = uv.z - uv.x;
                float uvY = uv.w - uv.y;

                uv.x += percent.x * uvX;
                uv.y += percent.y * uvY;
                uv.z -= percent.z * uvX;
                uv.w -= percent.w * uvY;

                if( m_flipX == true )
                {
                    std::swap( uv.x, uv.z );
                }

                if( m_flipY == true )
                {
                    std::swap( uv.y, uv.w );
                }

                m_verticesWM[0].uv.x = uv.x * m_textureUVScale.x + m_textureUVOffset.x;
                m_verticesWM[0].uv.y = uv.y * m_textureUVScale.y + m_textureUVOffset.y;
                m_verticesWM[1].uv.x = uv.z * m_textureUVScale.x + m_textureUVOffset.x;
                m_verticesWM[1].uv.y = uv.y * m_textureUVScale.y + m_textureUVOffset.y;
                m_verticesWM[2].uv.x = uv.z * m_textureUVScale.x + m_textureUVOffset.x;
                m_verticesWM[2].uv.y = uv.w * m_textureUVScale.y + m_textureUVOffset.y;
                m_verticesWM[3].uv.x = uv.x * m_textureUVScale.x + m_textureUVOffset.x;
                m_verticesWM[3].uv.y = uv.w * m_textureUVScale.y + m_textureUVOffset.y;
            }

            // Alpha
            //if( m_textures[1] != nullptr )
            {
                mt::vec4f uv = m_uv2;

                float uvX = uv.z - uv.x;
                float uvY = uv.w - uv.y;

                uv.x += percent.x * uvX;
                uv.y += percent.y * uvY;
                uv.z -= percent.z * uvX;
                uv.w -= percent.w * uvY;

                if( m_flipX == true )
                {
                    std::swap( uv.x, uv.z );
                }

                if( m_flipY == true )
                {
                    std::swap( uv.y, uv.w );
                }

                m_verticesWM[0].uv2.x = uv.x * m_textureUVScale.x + m_textureUVOffset.x;
                m_verticesWM[0].uv2.y = uv.y * m_textureUVScale.y + m_textureUVOffset.y;
                m_verticesWM[1].uv2.x = uv.z * m_textureUVScale.x + m_textureUVOffset.x;
                m_verticesWM[1].uv2.y = uv.y * m_textureUVScale.y + m_textureUVOffset.y;
                m_verticesWM[2].uv2.x = uv.z * m_textureUVScale.x + m_textureUVOffset.x;
                m_verticesWM[2].uv2.y = uv.w * m_textureUVScale.y + m_textureUVOffset.y;
                m_verticesWM[3].uv2.x = uv.x * m_textureUVScale.x + m_textureUVOffset.x;
                m_verticesWM[3].uv2.y = uv.w * m_textureUVScale.y + m_textureUVOffset.y;
            }
        }
        else
        {
            // RGB(A)
            {
                mt::vec4f uv = m_uv;

                float uvX = uv.z - uv.x;
                float uvY = uv.w - uv.y;

                uv.x += percent.x * uvX;
                uv.y += percent.y * uvY;
                uv.z -= percent.z * uvX;
                uv.w -= percent.w * uvY;

                if( m_flipX == true )
                {
                    std::swap( uv.x, uv.z );
                }

                if( m_flipY == true )
                {
                    std::swap( uv.y, uv.w );
                }

                m_verticesWM[0].uv.x = uv.z * m_textureUVScale.y + m_textureUVOffset.y;
                m_verticesWM[0].uv.y = uv.y * m_textureUVScale.x + m_textureUVOffset.x;
                m_verticesWM[1].uv.x = uv.z * m_textureUVScale.y + m_textureUVOffset.y;
                m_verticesWM[1].uv.y = uv.w * m_textureUVScale.x + m_textureUVOffset.x;
                m_verticesWM[2].uv.x = uv.x * m_textureUVScale.y + m_textureUVOffset.y;
                m_verticesWM[2].uv.y = uv.w * m_textureUVScale.x + m_textureUVOffset.x;
                m_verticesWM[3].uv.x = uv.x * m_textureUVScale.y + m_textureUVOffset.y;
                m_verticesWM[3].uv.y = uv.y * m_textureUVScale.x + m_textureUVOffset.x;
            }

            // Alpha                    
            {
                mt::vec4f uv = m_uv2;

                float uvX = uv.z - uv.x;
                float uvY = uv.w - uv.y;

                uv.x += percent.y * uvY;
                uv.y += percent.x * uvX;
                uv.z -= percent.w * uvY;
                uv.w -= percent.z * uvX;                        

                if( m_flipX == true )
                {
                    std::swap( uv.y, uv.w );
                }

                if( m_flipY == true )
                {
                    std::swap( uv.x, uv.z );
                }

                m_verticesWM[0].uv2.x = uv.z * m_textureUVScale.y + m_textureUVOffset.y;
                m_verticesWM[0].uv2.y = uv.y * m_textureUVScale.x + m_textureUVOffset.x;
                m_verticesWM[1].uv2.x = uv.z * m_textureUVScale.y + m_textureUVOffset.y;
                m_verticesWM[1].uv2.y = uv.w * m_textureUVScale.x + m_textureUVOffset.x;
                m_verticesWM[2].uv2.x = uv.x * m_textureUVScale.y + m_textureUVOffset.y;
                m_verticesWM[2].uv2.y = uv.w * m_textureUVScale.x + m_textureUVOffset.x;
                m_verticesWM[3].uv2.x = uv.x * m_textureUVScale.y + m_textureUVOffset.y;
                m_verticesWM[3].uv2.y = uv.y * m_textureUVScale.x + m_textureUVOffset.x;
            }
        }

        mt::vec2f visOffset( m_size.x * percent.x, m_size.y * percent.y );

        mt::vec2f size;
        size.x = m_size.x - m_size.x * ( percent.x + percent.z );
        size.y = m_size.y - m_size.y * ( percent.y + percent.w );

        if( m_centerAlign == true )
        {
            mt::vec2f alignOffset = m_maxSize * -0.5f;

            visOffset += alignOffset;
        }

        m_verticesLocal[0].x = visOffset.x + 0.f;
        m_verticesLocal[0].y = visOffset.y + 0.f;
        m_verticesLocal[0].z = 0.f;

        m_verticesLocal[1].x = visOffset.x + size.x;
        m_verticesLocal[1].y = visOffset.y + 0.f;
        m_verticesLocal[1].z = 0.f;

        m_verticesLocal[2].x = visOffset.x + size.x;
        m_verticesLocal[2].y = visOffset.y + size.y;
        m_verticesLocal[2].z = 0.f;

        m_verticesLocal[3].x = visOffset.x + 0.f;
        m_verticesLocal[3].y = visOffset.y + size.y;
        m_verticesLocal[3].z = 0.f;
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::invalidateVerticesColor()
    {
        m_invalidateVerticesColor = true;                
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::updateVerticesColor()
    {
        m_invalidateVerticesColor = false;

        ColourValue color;
        this->calcTotalColor(color);

        uint32_t argb = color.getAsARGB();

		for( RenderVertex2D
			*it = m_verticesWM,
			*it_end = m_verticesWM + 4;
		it != it_end;
		++it )
		{
			RenderVertex2D & vtx = *it;

			vtx.color = argb;
		}
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::_invalidateColor()
    {
        Node::_invalidateColor();

        this->invalidateVerticesColor();
    }
    //////////////////////////////////////////////////////////////////////////
    void Shape::updateVerticesWM()
    {
        m_invalidateVerticesWM = false;

        if( m_invalidateVerticesLocal == true )
        {
            this->updateVertices();
        }

        const mt::mat4f & wm = this->getWorldMatrix();

        for( size_t i = 0; i != 4; ++i )
        {
            const mt::vec3f & pos = m_verticesLocal[i];

            mt::vec3f & wm_pos = m_verticesWM[i].pos;
            mt::mul_v3_m4( wm_pos, pos, wm);
        }
    }
}