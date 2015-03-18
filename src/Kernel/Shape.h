#   pragma once

#   include "Kernel/Node.h"

#   include "Interface/RenderSystemInterface.h"

#   include "Core/Polygon.h"

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
		void disableTextureColor( bool _disable );

		void setBlendAdd( bool _value );
		bool isBlendAdd() const;

    public:
        void setTextureWrapX( bool _wrap );
        bool getTextureWrapX() const;

        void setTextureWrapY( bool _wrap );
        bool getTextureWrapY() const;

        void setMaxSize( const mt::vec2f & _size );
        const mt::vec2f & getMaxSize() const;

		void setSize( const mt::vec2f & _size );
		const mt::vec2f & getSize() const;

		void setOffset( const mt::vec2f & _size );
		const mt::vec2f & getOffset() const;

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
        void updateVertices() const;

    protected:
        void invalidateVerticesWM();
        void updateVerticesWM() const;

    protected:
        void invalidateVerticesColor();
        void updateVerticesColor() const;

    protected:
        inline const RenderVertex2D * getVerticesWM() const;

	protected:
		inline void invalidateMaterial();
		inline bool isInvalidateMaterial() const;
		
	protected:
        mt::vec2f m_maxSize;
		mt::vec2f m_size;
		mt::vec2f m_offset;

        mt::vec4f m_uv;
        mt::vec4f m_uv2;

        mt::vec4f m_percentVisibility;

        TVectorRenderVertex2D m_maskPolygonVertex;
        TVectorIndices m_maskPolygonIndices;
        
        mt::vec2f m_textureUVOffset;
        mt::vec2f m_textureUVScale;

        mutable mt::vec3f m_verticesLocal[4];
        mutable RenderVertex2D m_verticesWM[4];

		bool m_textureWrapX;
		bool m_textureWrapY;

		bool m_uvRotate;

		bool m_centerAlign;

		bool m_flipX;
		bool m_flipY;

		bool m_blendAdd;
		bool m_solid;
		bool m_disableTextureColor;

		mutable bool m_invalidateVerticesLocal;
		mutable bool m_invalidateVerticesWM;
		mutable bool m_invalidateVerticesColor;
		mutable bool m_invalidateMaterial;
    };
	//////////////////////////////////////////////////////////////////////////
	inline void Shape::invalidateMaterial()
	{
		m_invalidateMaterial = true;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Shape::isInvalidateMaterial() const
	{
		return m_invalidateMaterial;
	}
    //////////////////////////////////////////////////////////////////////////
    inline const RenderVertex2D * Shape::getVerticesWM() const
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