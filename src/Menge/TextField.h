#	pragma once

#	include "SceneNode2D.h"

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
		void setColor( unsigned int _color );

		//! ���������� ������ ������.
		/*!
		\return ������ ������
		*/
		float getHeight() const;

		//! ���������� ���� ������.
		/*!
		\return ���� ������
		*/
		unsigned int getColor() const;

		//! ���������� �����.
		/*!
		\return �����
		*/
		const std::string & getText() const;

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

	private:
		ResourceFont * m_resourceFont;

		unsigned int m_color;
		float		m_height;
		std::string m_text;
		std::string m_resourceFontName;
		mt::vec2f	m_length;

		bool m_centerAlign;
		mt::vec2f m_alignOffset;
		
		void updateAlign_();
	};
}