#	pragma once

#	include "ResourceImpl.h"

#	include "Math/vec4.h"

class RenderImageInterface;

namespace Menge
{
	class ResourceImage
		: public ResourceImpl
	{
	public:
		ResourceImage( const std::string & _name );

	public:
		virtual const mt::vec2f & getMaxSize() = 0;

		virtual size_t getFrameCount() = 0;

		virtual int getFrameDelay( size_t _frame ) = 0;
		virtual const mt::vec2f & getFrameSize( size_t _frame ) = 0;
		virtual const mt::vec2f & getFrameOffset( size_t _image ) = 0;
		virtual const mt::vec4f & getFrameUV( size_t _frame ) = 0;		

		virtual RenderImageInterface * getFrameImage( size_t _image ) = 0;
	};
}