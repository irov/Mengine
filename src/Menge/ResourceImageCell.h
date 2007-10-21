#	pragma once

#	include "ResourceImage.h"

#	include <vector>

namespace Menge
{
	class ResourceImageCell
		: public ResourceImage
	{
		RESOURCE_DECLARE( ResourceImageCell )

	public:
		ResourceImageCell( const std::string & _name );

	public:
		size_t getCount() override;

		const mt::vec2f & getMaxSize( size_t _frame ) override;
		const mt::vec2f & getSize( size_t _frame ) override;
		const mt::vec2f & getOffset( size_t _frame ) override;
		const mt::vec4f & getUV( size_t _frame ) override;		

		RenderImageInterface * getImage( size_t _frame ) override;

	public:
		void loader( TiXmlElement * _xml ) override;

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		std::string m_fileImage;

		size_t m_numX;
		size_t m_numY;

		mt::vec2f m_offset;
		std::vector<mt::vec4f>	m_uvs;

		ImageFrame m_imageFrame;
	};
}