#	pragma once

#	include "ResourceImageDefault.h"

#	include <vector>

namespace Menge
{
	class ResourceImageSet
		: public ResourceImageDefault
	{
		RESOURCE_DECLARE( ResourceImageSet )

	public:
		ResourceImageSet( const std::string & _name );

	public:
		size_t getCount() override;

		const mt::vec2f & getSize( size_t _frame ) override;
		const mt::vec4f & getUV( size_t _image ) override;

	public:
		void loader( TiXmlElement * _xml ) override;

	protected:
		bool _compile() override;

	protected:

		std::vector<mt::vec4f>	m_uvs;
	};
}