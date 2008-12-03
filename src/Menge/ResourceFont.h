#	pragma once

#	include "ResourceReference.h"

#	include "Math/vec4.h"
#	include <vector>
#	include <map>

namespace Menge
{
	class RenderImageInterface;
	class DataStreamInterface;

	//! ResourceFont - ������-����, ������� �������� �����������, ������ uv - ��������� � ������������� ������������������. 

    /*! xml - ���� ����� ��������� ���������:
	* <Resource Name = "���_�������" Type = "ResourceFont" >
	*	<File Path = "���_�����"/>
	* </Resource>
	*/

	class ResourceFont
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceFont )

	public:
		//! �����������.
		/*!
		\param _name ��� �������.
		*/
		ResourceFont( const ResourceFactoryParam & _params );

	public:
		//! ���������� uv - ���������� �����.
		/*!
		\param _id ������ �����
		\return uv - ���������� �����
		*/
		const mt::vec4f & getUV( unsigned int _id ) const;

		//! ���������� ��������� ������ � ������ �����.
		/*!
		\param _id ������ �����
		\return ����������� ������������������ �����
		*/
		float getCharRatio( unsigned int _id ) const;

		//! ���������� ����������� ��� ������.
		/*!
		\return �����������
		*/
		const RenderImageInterface * getImage() const;

		//! ���������� ������ - ����������� ��� ������.
		/*!
		\return �����������
		*/
		const RenderImageInterface * getOutlineImage() const;
		float getInitSize();

	public:
		void loader( XmlElement * _xml ) override;

	protected:
		bool _compile() override;
		void _release() override;

	private:

		void setFontdefPath_( const String& _path );
		void setImagePath_( const String& _path );
		void setOutlineImagePath_( const String& _path );

		void setGlyph( unsigned int _id, float _u1, float _v1, float _u2, float _v2 );
		String getFontDir( const String& _fontName );

		bool parseFontdef( DataStreamInterface * _stream );
		bool parseAttribute( const String& name, const String& params );

		void loaderFontdef_( XmlElement* _xml );
		void addGlyph_( const String& _glyph, const String& _rect, const String& _offset, int _width );
	private:

		struct Glyph
		{
			Glyph( unsigned int _id, const mt::vec4f& _rect, float _ratio )
				: id( _id )
				, rect( _rect )
				, ratio( _ratio )
			{}
			unsigned int id;
			mt::vec4f rect;
			float ratio;
		};

		typedef std::map<unsigned int, Glyph> TMapGlyph;
		TMapGlyph	m_glyphs;

		String m_fontdefFile;
		String m_imageFile;
		String m_outlineImageFile;

		float m_whsRatio;
		float m_initSize;
		float m_textureRatio;

		RenderImageInterface * m_image;
		RenderImageInterface * m_outline;
	};
}