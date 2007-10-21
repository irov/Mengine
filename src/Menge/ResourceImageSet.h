#	pragma once

#	include "ResourceImageCell.h"

#	include <vector>

namespace Menge
{
	class ResourceImageSet
		: public ResourceImageCell
	{
		RESOURCE_DECLARE( ResourceImageSet )

	public:
		ResourceImageSet( const std::string & _name );

	public:
		const mt::vec2f & getMaxSize( size_t _frame ) override;
		const mt::vec2f & getSize( size_t _frame ) override;

	public:
		void loader( TiXmlElement * _xml ) override;

	protected:
		bool _compile() override;

	public:
		std::vector<mt::vec2f> m_sizes;
	};
}