#	pragma once

#	include "../Interface/RenderSystemInterface.h"
#	include "Node.h"
#	include "ColourValue.h"
#	include "ValueInterpolator.h"

#	include "math/mat3.h"
#	include "math/vec4.h"

#	include <vector>

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
		: public Node
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
		virtual void setImageIndex( unsigned int _index );

		//! Возвращает текущий индекс в списке изображений
		/*!
		\return индекс
		*/
		virtual unsigned int getImageIndex() const;

		//! Установка ресурса.
		/*!
		\param _name имя ресурса.
		*/
		virtual void setImageResource( const std::string & _name );

		virtual mt::vec2f getImageSize();
		virtual bool getCenterAlign();

		//! Возвращает имя ресурса
		/*!
		\return имя ресурса
		*/
		virtual const std::string & getImageResource() const;

		//! Установка скейла по осям.
		/*!
		\param _scale скейл.
		*/
		virtual void setScale( const mt::vec2f& _scale );

		//! Возвращает скейл.
		/*!
		\return скейл
		*/
		virtual const mt::vec2f& getScale() const;

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

		//! Установка времени _time, в течении которого будет изменятся цвет спрайта к цвету _color.
		/*!
		\param _color - результирующий цвет
		\param _time - время, в течении которого будет изменятся цвет
		*/
		virtual void colorTo( const ColourValue & _color, float _time );

		void setAlpha( float _alpha );
		void alphaTo( float _alpha, float _time );
		//! Установка цвета спрайта.
		/*!
		\param _color значение цвета
		*/
		void setColor( const ColourValue & _color );

		//! Возвращает цвет спрайта.
		/*!
		\return цвет
		*/

		const ColourValue & getColor() const;

	public:
		void loader( XmlElement * _xml ) override;

	protected:
		void _render( const Viewport & _viewport, bool _enableDebug ) override;

		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

		void _update( float _timing ) override;
		void _setListener() override;

	private:
		void updateAlign_();
		void updateVisibility_();
		void updateFlip_();
		void updateAlphaBlend_();
			private:
				void updateSprite_();

	protected:


		ResourceImage * m_resource;
		std::string m_resourcename;

		unsigned int m_currentImageIndex;

		bool m_centerAlign;
		bool m_flipX;
		bool m_flipY;
		//bool m_changingColor;

		//float m_changingColorTime;

		mt::vec2f m_scale;
		mt::vec2f m_alignOffset;
		mt::vec2f m_size;
		mt::vec2f m_offset;

		mt::vec4f m_percent;
		mt::vec4f m_uv;


		ColourValue m_color;
		ValueInterpolator<ColourValue> m_colorTo;
		//Color m_newColor;

		EBlendFactor m_blendSrc;
		EBlendFactor m_blendDest;
		//PyObject * m_listener;
		//virtual void _onSetListener();
	};
}
