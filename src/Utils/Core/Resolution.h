#	pragma once

#	include <stdint.h>

#	include "Math/vec2.h"

namespace Menge
{
	class Resolution
	{
	public:
		Resolution();
		Resolution( uint32_t _width, uint32_t _height );

	public:
		void setWidth( uint32_t _width );
		uint32_t getWidth() const;

		void setHeight( uint32_t _height );
		uint32_t getHeight() const;

		float getAspectRatio() const;

		void calcSize( mt::vec2f & _size ) const;
		void calcScale( const Resolution & _resolution, mt::vec2f & _scale ) const;
		
	public:
		bool operator == ( const Resolution & _resolution ) const;

	protected:
		uint32_t m_width;
		uint32_t m_height;
	};
}
