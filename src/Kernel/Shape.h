#   pragma once

#   include "Kernel/Node.h"

#   include "Interface/RenderSystemInterface.h"

#	include "math/mat3.h"
#	include "math/vec4.h"
#	include "math/mat4.h"

namespace Menge
{
    class Shape
        : public Node
    {
    public:
        Shape();
        ~Shape();

    public:
        void setTextureWrapX( bool _wrap );
        bool getTextureWrapX() const;

        void setTextureWrapY( bool _wrap );
        bool getTextureWrapY() const;

        void setSize( const mt::vec2f & _size );
        const mt::vec2f & getSize() const;

        void setMaxSize( const mt::vec2f & _size );
        const mt::vec2f & getMaxSize() const;

        void setUVRotate( bool _rotate );
        bool getUVRotate() const;

        void setUV( const mt::vec4f & _uv );
        const mt::vec4f & getUV() const;

        void setUV2( const mt::vec4f & _uv );
        const mt::vec4f & getUV2() const;

    public:
        void setCenterAlign( bool _centerAlign );
        bool getCenterAlign() const;

        void setFlipX( bool _flipX );
        bool getFlipX() const;

        void setFlipY( bool _flipY );
        bool getFlipY() const;

        void setPercentVisibility( const mt::vec4f& _percent );
        const mt::vec4f& getPercentVisibility() const;

        void setTextureUVOffset( const mt::vec2f & _offset );
        const mt::vec2f & getTextureUVOffset() const;

        void setTextureUVScale( const mt::vec2f & _scale );
        const mt::vec2f & getTextureUVScale() const;

    protected:
        void _invalidateColor() override;
        void _invalidateWorldMatrix() override;

    protected:
        void invalidateVertices();
        void updateVertices();

    protected:
        void invalidateVerticesWM();
        void updateVerticesWM();

    protected:
        void invalidateVerticesColor();
        void updateVerticesColor();

    protected:
        const RenderVertex2D * getVerticesWM();

    private:
        bool m_textureWrapX;
        bool m_textureWrapY;

        mt::vec2f m_size;
        mt::vec2f m_maxSize;

        bool m_uvRotate;
        mt::vec4f m_uv;
        mt::vec4f m_uv2;

        bool m_centerAlign;
        bool m_flipX;
        bool m_flipY;
        
        mt::vec4f m_percentVisibility;
        
        mt::vec2f m_textureUVOffset;
        mt::vec2f m_textureUVScale;

        mt::vec3f m_verticesLocal[4];
        bool m_invalidateVerticesLocal;

        RenderVertex2D m_verticesWM[4];
        bool m_invalidateVerticesWM;

        bool m_invalidateVerticesColor;
    };
    //////////////////////////////////////////////////////////////////////////
    inline const RenderVertex2D * Shape::getVerticesWM()
    {
        if( m_invalidateVerticesWM == true )
        {
            this->updateVerticesWM();
        }

        if( m_invalidateVerticesColor == true )
        {
            this->updateVerticesColor();
        }

        return m_verticesWM;
    }
}