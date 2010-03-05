#	pragma once

#	include "../Interface/RenderSystemInterface.h"
#	include "Node.h"
#	include "Core/ColourValue.h"
#	include "ValueInterpolator.h"

#	include "FixedVertices.h"

#	include "math/mat3.h"
#	include "math/vec4.h"

#	include <vector>

namespace Menge
{
	class ResourceImage;
	struct Material;
	class Texture;

	//! Sprite - —прайт. ¬ качестве ресурса принимает ResourceImage, поэтому может содержать (в зависимости от типа ресурса, например ResourceImageDefault или ResourceImageSet) N изображений. Ёто необходимо дл€ того, что бы под одно описание анимации можно было подгон€ть различные изображени€ и наоборот. ѕоэтому Animation наследуетс€ от Sprite и через Sprite::getImageIndex получает текущий индекс изображени€.

	/*! xml - файл имеет следующую структуру:
	*	<Node Name = "им€_ноды" Type = "Sprite">
	*		<ImageMap Name = "им€_ресурса" />
	*		<ImageIndex Value = "индекс_картинки" />
	*		<CenterAlign Value = "1/0" />
	*		<Scale Value = "value" />
	*	</Node>
	*/

	class Sprite
		: public Node
		, public QuadVertices
	{
	public:
		Sprite();
		~Sprite();

	public:
		void setImageIndex( std::size_t _index );
		std::size_t getImageIndex() const;

		void setImageResource( const String& _name );
		void setImageAlpha( const String& _name );

		std::size_t getImageCount();

		mt::vec2f getImageSize();
		bool getCenterAlign() const;

		void setCenterAlign( bool _centerAlign );

		const String& getImageResource() const;

		void flip( bool _x );

		void setPercentVisibility( const mt::vec2f & _percentX, const mt::vec2f & _percentY );
		void setPercentVisibilityToCb( float _time, const mt::vec2f& _percentX, const mt::vec2f& _percentY, PyObject* _cb );
		void setPercentVisibilityToStop();

		const mt::vec4f& getPercentVisibility() const;

		void disableTextureColor( bool _disable );

		void setBlendSource( EBlendFactor _src );
		void setBlendDest( EBlendFactor _dst );

		void setTextureMatrixOffset( const mt::vec2f& _offset );	// hack hack
		void setAlphaImageIndex( std::size_t _index );

	public:
		void loader( XmlElement * _xml ) override;

	protected:
		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

		void _render( Camera2D * _camera ) override;

		void _invalidateWorldMatrix() override;
		void _setListener() override;

		void _updateBoundingBox( mt::box2f & _boundingBox ) override;
		void _invalidateColor() override;

	protected:
		void _updateVertices( Vertex2D * _vertcies, unsigned char _invalidateVertices ) override;

	protected:
		bool compileResource_();
		void setPercentVisibilityVec4f_( const mt::vec4f& _percent );

	protected:
		ResourceImage * m_resource;
		String m_resourceName;	
		String m_alphaImageName;
		ResourceImage* m_alphaImage;

		std::size_t m_currentImageIndex;
		std::size_t m_currentAlphaImageIndex;

		bool m_centerAlign;
		bool m_flipX;
		bool m_flipY;

		mt::vec4f m_percent;

		EBlendFactor m_blendSrc;
		EBlendFactor m_blendDst;

		Material* m_material;
		bool m_disableTextureColor;
		Texture* m_textures[2];
		int m_texturesNum;
		mt::vec2f m_textureMatrixOffset;
	};
}
