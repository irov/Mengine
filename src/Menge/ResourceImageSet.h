#	pragma once

#	include "ResourceImageCell.h"

#	include <vector>

namespace Menge
{
	//! ResourceImageSet - ������-����, ������� �������� ������ uv - ��������. 

	/*! xml - ���� ����� ��������� ���������:
	* <Resource Name = "���_�������" Type = "ResourceImageSet" >
	*	<Frame UV = "left;top;right;bottom"/>
	*    ...
	*   <Frame UV = "left;top;right;bottom"/>
	* </Resource>
	*/

	class ResourceImageSet
		: public ResourceImageCell
	{
		RESOURCE_DECLARE( ResourceImageSet )

	public:
		ResourceImageSet( const std::string & _name );

	public:
		void addFrameUV( const mt::vec4f & _uv );

	public:
		const mt::vec2f & getMaxSize( size_t _frame ) const override;
		const mt::vec2f & getSize( size_t _frame ) const override;

	public:
		void loader( XmlElement * _xml ) override;

	protected:
		bool _compile() override;

	public:
		std::vector<mt::vec2f> m_sizes;
	};
}