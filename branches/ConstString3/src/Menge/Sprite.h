#	pragma once

#	include "Node.h"

#	include "Core/ColourValue.h"
#	include "ValueInterpolator.h"

#	include "FixedVertices.h"

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
		, public QuadVertices
	{
	public:
		Sprite();
		~Sprite();

	public:
		void setImageResource( const ConstString& _name );
		const ConstString& getImageResource() const;

		const mt::vec2f & getImageSize();
		
		void setCenterAlign( bool _centerAlign );
		bool getCenterAlign() const;

		void setFlipX( bool _flipX );
		bool getFlipX() const;

		void setFlipY( bool _flipY );		
		bool getFlipY() const;

		void setPercentVisibility( const mt::vec4f& _percent );
		const mt::vec4f& getPercentVisibility() const;

		void disableTextureColor( bool _disable );

		void setTextureMatrixOffset( const mt::vec2f& _offset );	// hack hack
	
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
		void _updateVertices( Vertex2D * _vertcies, unsigned char _invalidateVertices ) override;

	protected:
		void updateMaterial_();

	protected:
		bool compileResource_();

	protected:
		ResourceImage * m_resource;

		ConstString m_resourceName;

        mt::vec2f m_spriteSize;
		bool m_isCustomSize;

		bool m_centerAlign;
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

		const RenderTextureInterface * m_textures[2];
	};
}
