# pragma once

#	include "Config/Typedef.h"

#	include "Node.h"
#	include "Core/ColourValue.h"
#	include <vector>

#	include "ValueInterpolator.h"
#	include "TextLine.h"

#	include "Math/vec4.h"

#	include "Vertex.h"

#	include "Core/ConstString.h"

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

	enum ETextFieldHorizontAlign
	{
		ETFHA_NONE = 0,
		ETFHA_CENTER = 1,
		ETFHA_LEFT = 2,
		ETFHA_RIGHT = 3
	};

	enum ETextFieldVerticalAlign
	{
		ETFVA_NONE = 0,
		ETFVA_CENTER = 1,
	};

	class TextField
		: public Node
	{
	public:
		TextField();
		~TextField();

	public:
		void setMaxLen( float _len );
		void setText( const String& _text );
		void setHeight( float _height );
		float getHeight() const;
		const String& getText() const;
		void setOutlineColor( const ColourValue& _color );
		const ColourValue& getOutlineColor() const;

		void enableOutline( bool _value );
		bool isOutline() const;

		const mt::vec2f& getLength();

		void setLineOffset( float _offset );
		float getLineOffset() const;

		void setResourceFont( const ConstString& _resName );
		const ConstString & getResourceFont() const;

		void setNoneAlign();
		bool isNoneAlign() const;

		void setCenterAlign();
		bool isCenterAlign() const;

		void setRightAlign();
		bool isRightAlign() const;

		void setLeftAlign();
		bool isLeftAlign() const;

		void setVerticalNoneAlign();
		bool isVerticalNoneAlign() const;

		void setVerticalCenterAlign();
		bool isVerticalCenterAlign() const;

		float getCharOffset() const;
		void setCharOffset( float _offset );

		void setTextByKey( const ConstString& _key );
		const ConstString & getTextKey() const;

		void setTextByKeyFormat( const ConstString& _key, const String & _format, std::size_t _number );

	protected:
		void loader( BinParser * _parser ) override;

	protected:
		void _render( Camera2D * _camera ) override;

	protected:	
		bool _activate() override;
		void _deactivate() override;
		bool _compile() override;
		void _release() override;

		void _invalidateWorldMatrix() override;
		void _updateBoundingBox( mt::box2f & _boundingBox ) override;
		void _invalidateColor() override;

	private:
		void updateAlignOffset_( TextLine & _line, mt::vec2f & _offset );

	private:
		void updateVertices();
		void invalidateVertices();

		inline TVectorVertex2D & getOutlineVertices();
		inline TVectorVertex2D & getTextVertices();

	private:
		ResourceFont * m_resourceFont;
		ConstString m_resourceFontName;
		ConstString m_key;

		ColourValue m_outlineColor;

		String m_text;
		String m_format;
		std::size_t m_number;

		float m_height;		
		
		mt::vec2f m_length;

		ETextFieldHorizontAlign m_horizontAlign;
		ETextFieldVerticalAlign m_verticalAlign;

		float m_maxWidth;
		float m_charOffset;
	
		bool m_outline;

		float m_lineOffset;

		typedef std::list<TextLine> TListTextLine;
		TListTextLine m_lines;

		void updateVertexData_( const ColourValue & _color, TVectorVertex2D& _vertexData );
		void createFormattedMessage_( const String& _text );
		void splitLine(const std::string& str);

		const Material * m_materialText;
		const Material * m_materialOutline;

		TVectorVertex2D m_vertexDataText;
		TVectorVertex2D m_vertexDataOutline;
		bool m_invalidateVertices;
	};
	//////////////////////////////////////////////////////////////////////////
	inline TVectorVertex2D & TextField::getOutlineVertices()
	{
		if( m_invalidateVertices == true )
		{
			updateVertices();
		}

		return m_vertexDataOutline;
	}
	//////////////////////////////////////////////////////////////////////////
	inline TVectorVertex2D & TextField::getTextVertices()
	{
		if( m_invalidateVertices == true )
		{
			updateVertices();
		}

		return m_vertexDataText;
	}
}
