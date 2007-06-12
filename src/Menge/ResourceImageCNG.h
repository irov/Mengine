#	pragma once

#	include "ResourceImage.h"

#	include <vector>

namespace Menge
{
	class ResourceImageCNG
		: public ResourceImage
	{
		RESOURCE_DECLARE( ResourceImageCNG )

	public:
		ResourceImageCNG( const std::string & _name );

	public:
		const mt::vec2f & getMaxSize();
		size_t getCount();

		const mt::vec2f & getSize(  size_t _image );
		const mt::vec2f & getOffset( size_t _image );
		const mt::vec4f & getUV( size_t _image );
		
		RenderImageInterface * getImage( size_t _image );

	public:
		void loader( TiXmlElement *xml );

	protected:
		bool _compile();
		void _release();

	protected:
		std::string m_fileMNG;

		int m_numX;
		int m_numY;

		mt::vec2f m_size;
		mt::vec2f m_offset;

	private:
		std::vector<mt::vec4f>	m_uvs;
		RenderImageInterface *  m_renderImage;
	};
}