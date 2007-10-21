#	pragma once

#	include "ResourceImage.h"

namespace Menge
{
	class ResourceImageDefault
		: public ResourceImage
	{
		RESOURCE_DECLARE( ResourceImageDefault )

	public:
		ResourceImageDefault( const std::string & _name );

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
		std::vector<std::string> m_filesImages;

		mt::vec2f m_offset;
		mt::vec4f m_uv;

		size_t m_filter;
		std::vector<ImageFrame>  m_frames;
	};
}