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
		FACTORABLE_DECLARE(TextField);
	public:
		//! �����������.
		/*!
		*/
		TextField();

		//! ����������.
		/*!
		*/
		~TextField();

	public:

		//! ��������� ����. ����� ������.
		/*!
		\param _len �������� ����. ����� ������
		*/
		void setMaxLen( float _len );

		//! ��������� ������������� ������.
		/*!
		\param _text �������� ������
		*/
		void setText( const String& _text );

		//! ��������� ������ ������.
		/*!
		\param _height �������� ������
		*/
		void setHeight( float _height );

		//! ���������� ������ ������.
		/*!
		\return ������ ������
		*/
		float getHeight() const;

		//! ���������� �����.
		/*!
		\return �����
		*/
		const String& getText() const;

		//! ��������� ������� ����� ������.
		/*!
		\param _color �������� �����
		*/
		void setOutlineColor( const ColourValue& _color );

		//! ���������� ���� ������� ������.
		/*!
		\return ���� ������� ������
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
