#	pragma once

#	include "Kernel/Node.h"

#	include "Core/ColourValue.h"
#	include "Core/ValueInterpolator.h"

#	include "../Interface/RenderSystemInterface.h"

#	include "math/mat3.h"
#	include "math/vec4.h"
#	include "math/mat4.h"

#	include <vector>

namespace Menge
{
	class ResourceImage;
	
	struct RenderMaterial;
	struct RenderMaterialGroup;
	
	class Sprite
		: public Node
	{
	public:
		Sprite();
		~Sprite();

	public:
		void setImageResourceName( const ConstString& _name );
		const ConstString& getImageResourceName() const;

    public:
        ResourceImage * getImageResource() const;
        
    public:
		void setFlipX( bool _flipX );
		bool getFlipX() const;

		void setFlipY( bool _flipY );		
		bool getFlipY() const;

		void setPercentVisibility( const mt::vec4f& _percent );
		const mt::vec4f& getPercentVisibility() const;

		void disableTextureColor( bool _disable );

		void setTextureMatrixOffset( const mt::vec2f& _offset );	// hack hack

        void setTextureUVOffset( const mt::vec2f & _offset );
        const mt::vec2f & getTextureUVOffset() const;

        void setTextureUVScale( const mt::vec2f & _scale );
        const mt::vec2f & getTextureUVScale() const;
	
		void setBlendAdd( bool _value );
		bool isBlendAdd() const;

		void setSpriteSize( const mt::vec2f& _size );

	protected:
		bool _compile() override;
		void _release() override;

		void _render( RenderCameraInterface * _camera ) override;

		void _invalidateWorldMatrix() override;

		void _updateBoundingBox( mt::box2f & _boundingBox ) override;
		void _invalidateColor() override;

    protected:
        void invalidateVertices_( unsigned char _invalidate = 0xFF );
		void updateVertices_();

    protected:
        void invalidateVerticesWM_();
        void updateVerticesWM_();

    protected:
        const Vertex2D * getVerticesWM_();
        
	protected:
		void updateMaterial_();

	protected:
		bool compileResource_();

	protected:
		ResourceImage * m_resource;

		ConstString m_resourceName;

        mt::vec2f m_spriteSize;
		bool m_isCustomSize;

		bool m_flipX;
		bool m_flipY;

		bool m_blendAdd;
		
		bool m_solid;

		mt::vec4f m_percentVisibility;

		const RenderMaterialGroup * m_materialGroup;
		const RenderMaterial * m_material;

		bool m_disableTextureColor;
		size_t m_texturesNum;
		mt::vec2f m_textureMatrixOffset;

        mt::vec2f m_textureUVOffset;
        mt::vec2f m_textureUVScale;

		const RenderTextureInterface * m_textures[2];

        mt::vec3f m_verticesLocal[4];
        unsigned char m_invalidateVerticesLocal;

        Vertex2D m_verticesWM[4];
        bool m_invalidateVerticesWM;
	};
    //////////////////////////////////////////////////////////////////////////
    inline const Vertex2D * Sprite::getVerticesWM_()
    {
        if( m_invalidateVerticesWM == true )
        {
            this->updateVerticesWM_();
        }

        return m_verticesWM;
    }
}
