# pragma once

#	include "Config/Typedef.h"

#	include "Node.h"
#	include "ColourValue.h"
#	include <vector>

#	include "ValueInterpolator.h"
#	include "TextLine.h"

#	include "Math/vec4.h"

namespace Menge
{
	class ResourceFont;
	class ResourceImage;

	class RenderImageInterface;

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
		OBJECT_DECLARE(TextField);
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
		void setText( const std::string & _text );

		//! ��������� ������ ������.
		/*!
		\param _height �������� ������
		*/
		void setHeight( float _height );

		//! ��������� ����� ������.
		/*!
		\param _color �������� �����
		*/
		void setColor( const ColourValue& _color );

		//! ���������� ���� ������.
		/*!
		\return ���� ������
		*/
		const ColourValue& getColor() const;

		//! ���������� ������ ������.
		/*!
		\return ������ ������
		*/
		float getHeight() const;

		//! ���������� �����.
		/*!
		\return �����
		*/
		const std::string & getText() const;

		//! ��������� ������� _time, � ������� �������� ����� ��������� ���� ������ � ����� _color.
		/*!
		\param _color - �������������� ����
		\param _time - �����, � ������� �������� ����� ��������� ����
		*/
		void colorTo( const ColourValue& _color, float _time );
		void setAlpha( float _alpha ) override;
		void alphaTo( float _alpha, float _time ) override;
		void colorToStop() override;

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

		const mt::vec2f& getLength() const;

		int getLineOffset() const;
		void setLineOffset( int _offset );

		void setResource( const String& _resName );

		void setOutlineResource( const String& _outlineName );

		bool getCenterAlign();

		float getCharOffset() const;

	public:
		void loader( XmlElement * _xml ) override;
		void _render( unsigned int _debugMask ) override;

	protected:
		bool _checkVisibility( const Viewport & _viewPort ) override;
		
		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

		void _update( float _timing ) override;

		void _setListener() override;
		void _invalidateWorldMatrix() override;

	private:
		ResourceFont * m_resource;
		std::string m_resourcename;

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
	
		bool m_outline;

		int m_lineOffset;

		std::list<TextLine>  m_lines;

		void renderPass_( ColourValue& _color, const RenderImageInterface * _renderImage );
		void createFormattedMessage_( const std::string& _text );
		void splitLine(const std::string& str);
	};
}
