#	pragma once

#	include <cstddef>

#	include "Math/vec2.h"

namespace Menge
{
	class Resolution
	{
	public:
		Resolution();
		Resolution( size_t _width, size_t _height );

	public:
		void setWidth( size_t _width );
		size_t getWidth() const;

		void setHeight( size_t _height );
		size_t getHeight() const;

		float getAspectRatio() const;

		void calcSize( mt::vec2f & _size ) const;
		void calcScale( const Resolution & _resolution, mt::vec2f & _scale ) const;
		
	public:
		bool operator == ( const Resolution & _resolution ) const;

	protected:
		size_t m_width;
		size_t m_height;
	};
}
