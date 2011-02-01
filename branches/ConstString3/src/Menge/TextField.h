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

	//! TextField - ����� ��� ��������� ������. 

	/*! xml - ���� ����� ��������� ���������:
	* <Node Name = "���_����" Type = "TextField">
	*  <Font Name = "���_������_������"/>
	*  <Text Value = "������������_�����"/>
	*  <Color Value = "����"/>
	*  <Height Value = "������ �����"/>
	*  <CenterAlign Value = "1/0"/>
	* </Node>
	*/

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

		bool getCenterAlign() const;
		void setCenterAlign( bool _centerAlign );

		bool getRightAlign() const;
		void setRightAlign( bool _rightAlign );

		bool getLeftAlign() const;
		void setLeftAlign( bool _leftAlign );

		float getCharOffset() const;
		void setCharOffset( float _offset );

		void setTextByKey( const ConstString& _key );
		const ConstString & getTextKey() const;

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
		void updateVertices();
		void invalidateVertices();

		inline TVectorVertex2D & getOutlineVertices();
		inline TVectorVertex2D & getTextVertices();

	private:
		ResourceFont * m_resourceFont;
		ConstString m_resourceFontName;
		ConstString m_key;

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

		bool m_solid;

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
