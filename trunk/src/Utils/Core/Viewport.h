#	pragma once

#	include "Math/box2.h"

namespace Menge
{
	class Viewport
	{
	public:
		Viewport();
		Viewport( const mt::vec2f & _begin, const mt::vec2f & _end );
		Viewport( float _beginX, float _beginY, float _endX, float _endY );

	public:
		bool testPoint( const mt::vec2f & _point ) const;
		bool testBBox( const mt::box2f& _bbox ) const;
		bool testRectangle( const mt::vec2f & _min,  const mt::vec2f & _max ) const;

	public:
		void parallax( const mt::vec2f & _factor );

	public:
		float getWidth() const;
		float getHeight() const;

	public:
		mt::vec2f begin;
		mt::vec2f end;
	};
}
