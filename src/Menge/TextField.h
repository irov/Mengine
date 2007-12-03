#	pragma once

#	include "SceneNode2D.h"
#	include "Color.h"

namespace Menge
{
	class ResourceFont;

	//! TextField - ����� ��� ��������� ������. 

    /*! xml - ���� ����� ��������� ���������:
	 *	<Node Name = "���_����" Type = "TextField">
     *      <Font Name = "���_������_������"/>
     *	    <Text Value = "������������_�����"/>
	 *		<Color Value = "����"/>
	 *		<Height Value = "������ �����"/>
	 *		<CenterAlign Value = "1/0"/>
	 *	</Node>
	*/

	class TextField
		: public SceneNode2D
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
		void setColor( const Color& _color );

		//! ���������� ���� ������.
		/*!
		\return ���� ������
		*/
		unsigned int getColor() const;

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