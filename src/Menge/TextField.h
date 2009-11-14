# pragma once

#	include "Config/Typedef.h"

#	include "Node.h"
#	include "Core/ColourValue.h"
#	include <vector>

#	include "ValueInterpolator.h"
#	include "TextLine.h"

#	include "Math/vec4.h"
#	include "Vertex.h"

namespace Menge
{
	class ResourceFont;
	class ResourceImage;

	struct Material;

	//! TextField - класс для отрисовки шрифта. 

	/*! xml - файл имеет следующую структуру:
	* <Node Name = "имя_ноды" Type = "TextField">
	*  <Font Name = "имя_ресура_шрифта"/>
	*  <Text Value = "отображаемый_текст"/>
	*  <Color Value = "цвет"/>
	*  <Height Value = "высота глифа"/>
	*  <CenterAlign Value = "1/0"/>
	* </Node>
	*/

	class TextField
		: public Node
	{
		FACTORABLE_DECLARE(TextField);
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
		void setText( const String& _text );

		//! Установка высоты глифов.
		/*!
		\param _height значение высоты
		*/
		void setHeight( float _height );

		//! Возвращает высоту глифов.
		/*!
		\return высота глифов
		*/
		float getHeight() const;

		//! Возвращает текст.
		/*!
		\return текст
		*/
		const String& getText() const;

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

		const mt::vec2f& getLength();

		void setLineOffset( float _offset );
		float getLineOffset() const;

		void setResource( const String& _resName );

		void setOutlineResource( const String& _outlineName );

		bool getCenterAlign();
		void setCenterAlign( bool _centerAlign );

		bool getRightAlign();
		void setRightAlign( bool _rightAlign );

		bool getLeftAlign();
		void setLeftAlign( bool _leftAlign );

		float getCharOffset() const;
		void setCharOffset( float _offset );

		void setTextByKey( const String& _key );

	public:
		void loader( XmlElement * _xml ) override;
		void _render( Camera2D * _camera ) override;

	protected:
	
		bool _activate() override;
		void _deactivate() override;
		bool _compile() override;
		void _release() override;

		void _setListener() override;
		void _invalidateWorldMatrix() override;
		void _updateBoundingBox( mt::box2f & _boundingBox ) override;

	private:
		void updateVertices_();

	private:
		ResourceFont * m_resource;
		String m_resourcename;

		ColourValue m_outlineColor;

		float  m_height;
		String m_text;
		mt::vec2f m_length;

		bool m_centerAlign;
		mt::vec2f m_alignOffset;

		bool m_rightAlign;

		float m_maxWidth;
		float m_charOffset;
	
		bool m_outline;

		float m_lineOffset;

		typedef std::list<TextLine> TListTextLine;
		TListTextLine m_lines;

		void updateVertexData_( const ColourValue & _color, TVertex2DVector& _vertexData );
		void createFormattedMessage_( const String& _text );
		void splitLine(const std::string& str);

		TVertex2DVector m_vertexDataText;
		TVertex2DVector m_vertexDataOutline;
		Material* m_materialText;
		Material* m_materialOutline;
		bool m_invalidateVertices;
	};
}
