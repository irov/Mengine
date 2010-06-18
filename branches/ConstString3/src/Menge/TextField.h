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

		const mt::vec2f& getLength();

		void setLineOffset( float _offset );
		float getLineOffset() const;

		void setResource( const ConstString& _resName );
		const ConstString & getResource() const;

		bool getCenterAlign() const;
		void setCenterAlign( bool _centerAlign );

		bool getRightAlign() const;
		void setRightAlign( bool _rightAlign );

		bool getLeftAlign() const;
		void setLeftAlign( bool _leftAlign );

		float getCharOffset() const;
		void setCharOffset( float _offset );

		void setTextByKey( const ConstString& _key );
		const std::string & getTextKey() const;

	public:
		void loader( XmlElement * _xml ) override;
		void _render( Camera2D * _camera ) override;

	protected:	
		bool _activate() override;
		void _deactivate() override;
		bool _compile() override;
		void _release() override;

		void _setListener( PyObject * _listener ) override;
		void _invalidateWorldMatrix() override;
		void _updateBoundingBox( mt::box2f & _boundingBox ) override;
		void _invalidateColor() override;

	private:
		void updateVertices();
		void invalidateVertices();

		inline TVertex2DVector & getOutlineVertices();
		inline TVertex2DVector & getTextVertices();

	private:
		ResourceFont * m_resource;
		ConstString m_resourcename;
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
	//////////////////////////////////////////////////////////////////////////
	inline TVertex2DVector & TextField::getOutlineVertices()
	{
		if( m_invalidateVertices == true )
		{
			updateVertices();
		}

		return m_vertexDataOutline;
	}
	//////////////////////////////////////////////////////////////////////////
	inline TVertex2DVector & TextField::getTextVertices()
	{
		if( m_invalidateVertices == true )
		{
			updateVertices();
		}

		return m_vertexDataText;
	}
}
