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
		ResourceImageCell( const std::string & _name );

	public:
		size_t getCount() const override;

		const mt::vec2f & getMaxSize( size_t _frame ) const override;
		const mt::vec2f & getSize( size_t _frame ) const override;
		const mt::vec2f & getOffset( size_t _frame ) const override;
		const mt::vec4f & getUV( size_t _frame ) const override;		

		const RenderImageInterface * getImage( size_t _frame ) const override;

	public:
		void loader( TiXmlElement * _xml ) override;

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		std::string m_filename;

		size_t m_numX;
		size_t m_numY;

		mt::vec2f m_offset;
		std::vector<mt::vec4f>	m_uvs;

		ImageFrame m_imageFrame;
	};
}