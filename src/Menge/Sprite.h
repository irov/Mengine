#	pragma once

#	include "SceneNode2D.h"

#	include "math/mat3.h"
#	include "math/vec4.h"

#	include <vector>

class RenderImageInterface;

namespace Menge
{
	class ResourceImage;

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
		: public SceneNode2D
	{
		OBJECT_DECLARE(Sprite);
	public:
		//! Конструктор.
		/*!
		*/
		Sprite();

		//! Деструктор.
		/*!
		*/
		~Sprite();

	public:
		//! Установка индекса в списке изображений.
		/*!
		\param _index индекс.
		*/
		virtual void setImageIndex( size_t _index );

		//! Возвращает текущий индекс в списке изображений
		/*!
		\return индекс
		*/
		virtual size_t getImageIndex() const;

		//! Установка ресурса.
		/*!
		\param _name имя ресурса.
		*/
		virtual void setImageResource( const std::string & _name );

		//! Возвращает имя ресурса
		/*!
		\return имя ресурса
		*/
		virtual const std::string & getImageResource() const;

		//! Установка скейла.
		/*!
		\param _scale скейл.
		*/
		virtual void setScale( float _scale );

		//! Возвращает скейл.
		/*!
		\return скейл
		*/
		virtual float getScale() const;

		virtual void flip( bool _x );

		virtual void setPercentVisibility( const mt::vec2f & _percentX, const mt::vec2f & _percentY );

	public:
		virtual bool isVisible( const Viewport & _viewPort );

		void loader( TiXmlElement * _xml ) override;

	protected:
		void _render() override;
		void _debugRender() override;

		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;
	
	private:
		void updateAlign_();
		void updateVisibility_();

	protected:

		ResourceImage * m_resource;
		std::string m_resourcename;

		size_t m_currentImageIndex;

		bool m_centerAlign;
		bool m_flipX;
		bool m_flipY;

		int	inverse_x;
		int	inverse_y;

		float m_scale;

		mt::vec2f m_alignOffset;
		mt::vec2f m_size;
		mt::vec2f m_offset;

		mt::vec4f m_percent;
		mt::vec4f m_uv;

		unsigned int m_color;
	};
}