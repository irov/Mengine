# pragma once

#	include "Config/Typedef.h"

# include "SceneNode2D.h"
# include "Color.h"
# include <vector>

class RenderImageInterface;

namespace Menge
{
	class ResourceFont;
	class ResourceImage;

	//! TextField - класс для отрисовки шрифта. 

	/*! xml - файл имеет следующую структуру:
	* <Node Name = "имя_ноды" Type = "TextField">
	*      <Font Name = "имя_ресура_шрифта"/>
	*     <Text Value = "отображаемый_текст"/>
	*  <Color Value = "цвет"/>
	*  <Height Value = "высота глифа"/>
	*  <CenterAlign Value = "1/0"/>
	* </Node>
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

		//! Установка макс. длины строки.
		/*!
		\param _len значение макс. длины строки
		*/
		void setMaxLen( float _len );

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
		const Color& getColor() const;

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
		void colorTo( const Color& _color, float _time );
		void setAlpha( float _alpha );
		void alphaTo( float _alpha, float _time );

		//! Установка обводки цвета глифов.
		/*!
		\param _color значение цвета
		*/
		void setOutlineColor( const Color& _color );

		//! Возвращает цвет обводки глифов.
		/*!
		\return цвет обводки глифов
		*/
		const Color& getOutlineColor() const;

		const mt::vec2f& getLength() const;

		int getLineOffset() const;
		void setLineOffset( int _offset );

		void setResource( const String& _resName );

		void setOutlineResource( const String& _outlineName );

	public:
		bool isVisible( const Viewport & _viewPort ) override;
		void loader( XmlElement * _xml ) override;

		virtual void setListener( PyObject* _listener ) override;

	protected:
		void _render( bool _enableDebug ) override;
		void _debugRender() override;

		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

		void _update( float _timing ) override;
		//virtual void _onSetListener();

	private:
		ResourceFont * m_resource;
		std::string m_resourcename;

		float m_changingColorTime;
		bool m_changingColor;
		Color m_newColor;
		Color m_newOutlineColor;

		Color m_color;
		Color m_outlineColor;
		float  m_height;
		std::string m_text;
		mt::vec2f m_length;

		bool m_centerAlign;
		mt::vec2f m_alignOffset;

		bool m_rightAlign;

		float m_maxWidth;
		float m_charOffset;
		ResourceImage * m_outlineImage;
		std::string m_outlineFontName;

		int m_lineOffset;
		//PyObject* m_listener;

		struct Line
		{
			std::string text;
			float  length;
			Line( const std::string& _text, float _len )
				: text(_text), length(_len){};
		};

		std::list<Line>  m_lines;

		void renderPass_( const Color & _color, const RenderImageInterface * _renderImage, mt::vec4f _uv = mt::vec4f::zero_v4, float k = 0, float h = 0 );
		void createFormattedMessage_( const std::string& _text );
		float getWordWidth_( const std::string & _text ) const;
		void splitLine(const std::string& str);
	};
}