#	pragma once

#	include "ResourceImage.h"

#	include "Math/vec4.h"

class RenderImageInterface;

namespace Menge
{
	class ResourceImageDefault
		: public ResourceImage
	{
		RESOURCE_DECLARE( ResourceImageDefault )

	public:
		ResourceImageDefault( const std::string & _name );

	public:
		const mt::vec2f & getMaxSize() override;

		size_t getCount() override;

		const mt::vec2f & getSize( size_t _frame ) override;
		const mt::vec2f & getOffset( size_t _image ) override;
		const mt::vec4f & getUV( size_t _frame ) override;		

		RenderImageInterface * getImage( size_t _image ) override;

	public:
		void loader( TiXmlElement * _xml ) override;

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		std::string m_fileImage;

		size_t m_count;
		int	m_filter;

		mt::vec2f m_offset;

		mt::vec2f m_size;
		mt::vec4f m_uv;

		RenderImageInterface *  m_renderImage;
	};
}