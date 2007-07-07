#	pragma once

#	include "ResourceImageDefault.h"

#	include <vector>

namespace Menge
{
	class ResourceImageCell
		: public ResourceImageDefault
	{
		RESOURCE_DECLARE( ResourceImageCell )

	public:
		ResourceImageCell( const std::string & _name );

	public:
		size_t getCount() override;

		const mt::vec4f & getUV( size_t _image ) override;

	public:
		void loader( TiXmlElement *xml ) override;

	protected:
		bool _compile() override;

	protected:
		int m_numX;
		int m_numY;

		std::vector<mt::vec4f>	m_uvs;
	};
}