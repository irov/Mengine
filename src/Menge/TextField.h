#	pragma once

#	include "SceneNode2D.h"
#	include "Color.h"

namespace Menge
{
	class ResourceFont;

	//! TextField - класс для отрисовки шрифта. 

    /*! xml - файл имеет следующую структуру:
	 *	<Node Name = "имя_ноды" Type = "TextField">
     *      <Font Name = "имя_ресура_шрифта"/>
     *	    <Text Value = "отображаемый_текст"/>
	 *		<Color Value = "цвет"/>
	 *		<Height Value = "высота глифа"/>
	 *		<CenterAlign Value = "1/0"/>
	 *	</Node>
	*/

	class TextField
		: public SceneNode2D
	{
		OBJECT_DECLARE(TextField);
	public:
		//! Конструктор.
		/*!
		*/
		TextField();

		//! Деструктор.
		/*!
		*/
		~TextField();

	public:

		//! Установка отображаемого текста.
		/*!
		\param _text значение текста
		*/
		void setText( const std::string & _text );

		//! Установка высоты глифов.
		/*!
		\param _height значение высоты
		*/
		void setHeight( float _height );

		//! Установка цвета глифов.
		/*!
		\param _color значение цвета
		*/
		void setColor( const Color& _color );

		//! Возвращает цвет глифов.
		/*!
		\return цвет глифов
		*/
		unsigned int getColor() const;

		//! Возвращает высоту глифов.
		/*!
		\return высота глифов
		*/
		float getHeight() const;

		//! Возвращает текст.
		/*!
		\return текст
		*/
		const std::string & getText() const;

		//! Установка времени _time, в течении которого будет изменятся цвет текста к цвету _color.
		/*!
		\param _color - результирующий цвет
		\param _time - время, в течении которого будет изменятся цвет
		*/
		void setColorTime(  const Color& _color, float _time );

	public:
		bool isVisible( const Viewport & _viewPort ) override;
		void loader( TiXmlElement * _xml ) override;

	protected:
		void _render() override;
		void _debugRender() override;

		bool _activate() override;
		void _deactivate() override;
		
		bool _compile() override;
		void _release() override;

		void _update( float _timing ) override;

	private:
		ResourceFont * m_resource;
		std::string	m_resourcename;

		float m_changingColorTime;
		Color m_newColor;

		Color m_color;
		float		m_height;
		std::string m_text;
		mt::vec2f	m_length;

		bool m_centerAlign;
		mt::vec2f m_alignOffset;
		
		void updateAlign_();
	};
}