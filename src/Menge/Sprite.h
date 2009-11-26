#	pragma once

#	include "../Interface/RenderSystemInterface.h"
#	include "Node.h"
#	include "Core/ColourValue.h"
#	include "ValueInterpolator.h"

#	include "Vertex.h"

#	include "math/mat3.h"
#	include "math/vec4.h"

#	include <vector>

namespace Menge
{
	class ResourceImage;
	struct Material;
	class Texture;

	//! Sprite - Спрайт. В качестве ресурса принимает ResourceImage, поэтому может содержать (в зависимости от типа ресурса, например ResourceImageDefault или ResourceImageSet) N изображений. Это необходимо для того, что бы под одно описание анимации можно было подгонять различные изображения и наоборот. Поэтому Animation наследуется от Sprite и через Sprite::getImageIndex получает текущий индекс изображения.

	/*! xml - файл имеет следующую структуру:
	*	<Node Name = "имя_ноды" Type = "Sprite">
	*		<ImageMap Name = "имя_ресурса" />
	*		<ImageIndex Value = "индекс_картинки" />
	*		<CenterAlign Value = "1/0" />
	*		<Scale Value = "value" />
	*	</Node>
	*/

	class Sprite
		: public Node
	{
		FACTORABLE_DECLARE(Sprite);
	public:
		//! Конструктор.
		/*!
		*/
		Sprite();

		//! Деструктор.
		/*!
		*/
		virtual ~Sprite();

	public:
		//! Установка индекса в списке изображений.
		/*!
		\param _index индекс.
		*/
		virtual void setImageIndex( std::size_t _index );

		//! Возвращает текущий индекс в списке изображений
		/*!
		\return индекс
		*/
		virtual std::size_t getImageIndex() const;

		//! Установка ресурса.
		/*!
		\param _name имя ресурса.
		*/
		virtual void setImageResource( const String& _name );
		void setImageAlpha( const String& _name );

		std::size_t getImageCount() const;

		virtual mt::vec2f getImageSize();
		virtual bool getCenterAlign();

		void setCenterAlign( bool _centerAlign );
		//! Возвращает имя ресурса
		/*!
		\return имя ресурса
		*/
		virtual const String& getImageResource() const;

		//! Инвертирование спрайта по оси x, если _x = true, иначе по оси y.
		/*!
		\param _x ось.
		*/
		virtual void flip( bool _x );

		//! Установка процента видимости спрайта по краям. 0.0 - все видно. 1.0 - не видно.
		/*!
		\param _percentX - коэффициент видимости по x, y слева.
		\param _percentY - коэффициент видимости по ширине, высоте справа.
		*/
		virtual void setPercentVisibility( const mt::vec2f & _percentX, const mt::vec2f & _percentY );

		void setPercentVisibilityToCb( float _time, const mt::vec2f& _percentX, const mt::vec2f& _percentY, PyObject* _cb );
		void setPercentVisibilityToStop();

		const mt::vec4f& getPercentVisibility() const;
		void setPercentVisibilityVec4f( const mt::vec4f& _percent );

		void disableTextureColor( bool _disable );

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
		void updateVertices();
		void invalidateVertices();

	private:
		void updateSprite_();
		void updateDimensions_( ResourceImage * _resource, bool _wrap );

	protected:
		ResourceImage * m_resource;
		String m_resourceName;	
		String m_alphaImageName;
		ResourceImage* m_alphaImage;

		std::size_t m_currentImageIndex;

		bool m_centerAlign;
		bool m_flipX;
		bool m_flipY;

		mt::vec2f m_alignOffset;
		mt::vec2f m_size;
		mt::vec2f m_offset;

		mt::vec4f m_percent;

		bool m_invalidateVertices;

		EBlendFactor m_blendSrc;
		EBlendFactor m_blendDest;

		Material* m_material;
		Vertex2D m_vertices[4];
		bool m_disableTextureColor;
		Texture* m_textures[2];
		int m_texturesNum;
	};
}
