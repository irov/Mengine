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

		mt::vec2f getScale( const Resolution & _resolution ) const;

	public:
		bool operator == ( const Resolution & _resolution ) const;

	protected:
		size_t m_width;
		size_t m_height;
	};
}
