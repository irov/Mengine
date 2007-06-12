#	pragma once

#	include "ResourceImage.h"

#	include <vector>

namespace Menge
{
	class ResourceImageMNG
		: public ResourceImage
	{
		RESOURCE_DECLARE( ResourceImageMNG )

	private:

		struct Image
		{
			mt::vec2f offset;
			mt::vec2f size;
			RenderImageInterface * renderImage;
		};

	public:
		ResourceImageMNG( const std::string & _name );

	public:
		const mt::vec2f & getMaxSize();
		size_t getCount();

		const mt::vec2f & getSize( size_t _image );
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

		mt::vec2f m_size;
		mt::vec4f m_uv;

		std::vector<Image>	m_images;
	};
}