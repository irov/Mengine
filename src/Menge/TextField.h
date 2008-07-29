# pragma once

#	include "Config/Typedef.h"

# include "Node.h"
# include "ColourValue.h"
# include <vector>

#	include "ValueInterpolator.h"
#	include "Math/vec4.h"

namespace Menge
{
	class ResourceFont;
	class ResourceImage;

	class RenderImageInterface;

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
		: public Node
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
		void setColor( const ColourValue& _color );

		//! Возвращает цвет глифов.
		/*!
		\return цвет глифов
		*/
		const ColourValue& getColor() const;

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
		void colorTo( const ColourValue& _color, float _time );
		void setAlpha( float _alpha ) override;
		void alphaTo( float _alpha, float _time ) override;
		void colorToStop() override;

		//! Установка обводки цвета глифов.
		/*!
		\param _color значение цвета
		*/
		void setOutlineColor( const ColourValue& _color );

		//! Возвращает цвет обводки глифов.
		/*!
		\return цвет обводки глифов
		*/
		const ColourValue& getOutlineColor() const;

		const mt::vec2f& getLength() const;

		int getLineOffset() const;
		void setLineOffset( int _offset );

		void setResource( const String& _resName );

		void setOutlineResource( const String& _outlineName );

		bool getCenterAlign();

	public:
		void loader( XmlElement * _xml ) override;

		void _render( const Viewport & _viewport, bool _enableDebug ) override;
	protected:
		bool _checkVisibility( const Viewport & _viewPort ) override;
		
		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

		void _update( float _timing ) override;

		void _setListener() override;

		//virtual void _onSetListener();

	private:
		ResourceFont * m_resource;
		std::string m_resourcename;

		//float m_changingColorTime;
		//bool m_changingColor;
		//Color m_newColor;
		//Color m_newOutlineColor;

		ColourValue m_color;
		ColourValue m_outlineColor;
		ValueInterpolator<ColourValue> m_colorTo;
		ValueInterpolator<ColourValue> m_outlineColorTo;

		float  m_height;
		std::string m_text;
		mt::vec2f m_length;

		bool m_centerAlign;
		mt::vec2f m_alignOffset;

		bool m_rightAlign;

		float m_maxWidth;
		float m_charOffset;
		//ResourceImage * m_outlineImage;
		//std::string m_outlineFontName;
		bool m_outline;

		int m_lineOffset;
		//PyObject* m_listener;

		struct CharData
		{
			String::value_type code;
			mt::vec4f uv;
			float ratio;
		};

		typedef std::vector<CharData> TCharsData;

		struct Line
		{
			Line( ResourceFont * _resource, const String & _text, float _len );

			TCharsData charsData;

			String text;
			
			float  length;
		};

		std::list<Line>  m_lines;
		float m_spaceWidth;

		void renderPass_( const ColourValue & _color, const RenderImageInterface * _renderImage, mt::vec4f _uv = mt::vec4f::zero_v4, float k = 0, float h = 0 );
		void createFormattedMessage_( const std::string& _text );
		float getWordWidth_( const std::string & _text ) const;
		void splitLine(const std::string& str);
	};
}
