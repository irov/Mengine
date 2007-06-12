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

		virtual size_t getCount() = 0;

		virtual const mt::vec2f & getSize( size_t _frame ) = 0;
		virtual const mt::vec2f & getOffset( size_t _image ) = 0;
		virtual const mt::vec4f & getUV( size_t _frame ) = 0;		

		virtual RenderImageInterface * getImage( size_t _image ) = 0;
	};
}