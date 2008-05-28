#	pragma once

#	include "ResourceImage.h"

#	include <vector>

namespace Menge
{
	//! ResourceImageCell - ������-����, ������� ������ ����������� �����������. 

	/*! xml - ���� ����� ��������� ���������:
	* <Resource Name = "���_�������" Type = "ResourceImageCell" >
	*	<File Path = "���_�����"/>
	*   <Cell X = "���������� �������� �� �����������"/>
	*   <Cell Y = "���������� �������� �� ���������"/>
	* </Resource>
	*/

	class ResourceImageCell
		: public ResourceImage
	{
		RESOURCE_DECLARE( ResourceImageCell )

	public:
		ResourceImageCell( const ResourceFactoryParam & _name );

	public:
		unsigned int getCount() const override;

		const mt::vec2f & getMaxSize( unsigned int _frame ) const override;
		const mt::vec2f & getSize( unsigned int _frame ) const override;
		const mt::vec2f & getOffset( unsigned int _frame ) const override;
		const mt::vec4f & getUV( unsigned int _frame ) const override;		
		bool isAlpha( unsigned int _frame ) const;

		const RenderImageInterface * getImage( unsigned int _frame ) const override;

	public:
		void loader( XmlElement * _xml ) override;

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		unsigned int m_numX;
		unsigned int m_numY;

		typedef std::vector<mt::vec4f> TVectorUV;
		TVectorUV m_uvs;

		ImageFrame m_imageFrame;

	private:

		struct ImageDesc
		{
			std::string fileName;
			mt::vec4f uv;
			mt::vec2f offset;
			mt::vec2f maxSize;
			mt::vec2f size;
			bool isAlpha;
		};

		ImageDesc m_imageDesc;
	};
}