#	pragma once

#	include "ResourceImage.h"

#	include <vector>

namespace Menge
{
	class ResourceImageMNG
		: public ResourceImage
	{
		struct Frame 
		{
			int		index;
			int		delay;
			mt::vec2f size;
		};

		struct Image
		{
			mt::vec2f offset;
			RenderImageInterface * renderImage;
		};

	public:
		ResourceImageMNG( const std::string & _name );

	public:
		const mt::vec2f & getMaxSize();
		size_t getFrameCount();

		int getFrameDelay( size_t _frame );
		const mt::vec2f & getFrameSize( size_t _image );
		const mt::vec2f & getFrameOffset( size_t _image );
		const mt::vec4f & getFrameUV( size_t _frame );
	
		RenderImageInterface * getFrameImage( size_t _image );

	public:
		void loader( TiXmlElement *xml );

	protected:
		bool _compile();
		void _release();

	protected:
		std::string m_fileMNG;

		mt::vec2f m_size;
		mt::vec4f m_uv;

		std::vector<Frame>	m_frames;
		std::vector<Image>	m_images;
	};
}