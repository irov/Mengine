#   pragma once

#   include "Interface/RenderSystemInterface.h"

#   include "Kernel/Node.h"
#	include "Kernel/Materialable.h"
#	include "Kernel/ResourceImage.h"

#   include "Core/Polygon.h"

#	include "math/mat3.h"
#	include "math/vec4.h"
#	include "math/mat4.h"

namespace Menge
{
    class Shape
        : public Node
		, public Materialable
    {
    public:
        Shape();
        ~Shape();

	public:
		void setResourceImage( ResourceImage * _resourceImage );
		ResourceImage * getResourceImage() const;
		
	public:
		void setCustomSize( const mt::vec2f & _customSize );
		void removeCustomSize();
		bool hasCustomSize() const;
		const mt::vec2f & getCustomSize() const;

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
		bool _compile() override;
		void _release() override;

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
		ResourceHolder<ResourceImage> m_resourceImage;

		mt::vec2f m_customSize;

        mt::vec4f m_percentVisibility;
       
        mt::vec2f m_textureUVOffset;
        mt::vec2f m_textureUVScale;

        mutable mt::vec3f m_verticesLocal[4];
        mutable RenderVertex2D m_verticesWM[4];

		bool m_centerAlign;
		bool m_hasCustomSize;

		bool m_flipX;
		bool m_flipY;

		bool m_solid;

		mutable bool m_invalidateVerticesLocal;
		mutable bool m_invalidateVerticesWM;
		mutable bool m_invalidateVerticesColor;
    };
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