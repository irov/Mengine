#	pragma once

#	include "ResourceReference.h"

#	include "Math/vec4.h"
#	include <vector>
#	include <map>

namespace Menge
{
	class Texture;
	class DataStreamInterface;
	class ResourceGlyph;

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
		ResourceFont();

	public:
		//! ���������� uv - ���������� �����.
		/*!
		\param _id ������ �����
		\return uv - ���������� �����
		*/
		mt::vec4f getUV( unsigned int _id ) const;

		//! ���������� ��������� ������ � ������ �����.
		/*!
		\param _id ������ �����
		\return ����������� ������������������ �����
		*/
		float getCharRatio( unsigned int _id ) const;

		const mt::vec2f& getOffset( unsigned int _char ) const;
		const mt::vec2f& getSize( unsigned int _char ) const;

		//! ���������� ����������� ��� ������.
		/*!
		\return �����������
		*/
		Texture* getImage();

		//! ���������� ������ - ����������� ��� ������.
		/*!
		\return �����������
		*/
		Texture* getOutlineImage();
		float getInitSize() const;

		const ConstString& getImagePath() const;
		const ConstString& getOutlineImagePath() const;



	public:
		void loader( BinParser * _parser ) override;

	protected:
		bool _compile() override;
		void _release() override;

	private:
		void setImagePath_( const ConstString& _path );
		void setOutlineImagePath_( const ConstString& _path );

	private:
		ConstString m_glyphResource;
		ResourceGlyph * m_glyph;
		
		ConstString m_imageFile;
		ConstString m_imageCodec;

		String m_fontdefPath;

		ConstString m_outlineImageFile;
		ConstString m_outlineImageCodec;

		float m_whsRatio;
		float m_initSize;
		float m_textureRatio;

		Texture* m_image;
		Texture* m_outline;
		mt::vec2f m_imageInvSize;
	};
}
