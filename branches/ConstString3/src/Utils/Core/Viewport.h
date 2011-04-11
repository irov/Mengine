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
		void initialize( const mt::vec2f & _begin, const mt::vec2f & _end );

	public:
		inline bool testPoint( const mt::vec2f & _point ) const;
		inline bool testBBox( const mt::box2f& _bbox ) const;
		inline bool testRectangle( const mt::vec2f & _min,  const mt::vec2f & _max ) const;

		inline bool existBBox( const mt::box2f& _bbox ) const;
		inline bool existRectangle( const mt::vec2f & _min,  const mt::vec2f & _max ) const;

	public:
		void parallax( const mt::vec2f & _factor );

	public:
		float getWidth() const;
		float getHeight() const;

	public:
		mt::vec2f begin;
		mt::vec2f end;
	};
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	bool Viewport::testPoint( const mt::vec2f & _point ) const
	{
		if( begin.x > _point.x ) return false;
		if( begin.y > _point.y ) return false;
		if( end.x <= _point.x ) return false;
		if( end.y <= _point.y ) return false;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Viewport::testRectangle( const mt::vec2f & _min, const mt::vec2f & _max ) const
	{
		return mt::is_intersect( _min, _max, begin, end );
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Viewport::testBBox( const mt::box2f& _bbox ) const
	{
		return testRectangle( _bbox.minimum, _bbox.maximum );
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Viewport::existRectangle( const mt::vec2f & _min,  const mt::vec2f & _max ) const
	{
		return mt::is_exist( _min, _max, begin, end ); 
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Viewport::existBBox( const mt::box2f& _bbox ) const
	{
		return existRectangle( _bbox.minimum, _bbox.maximum ); 
	}
}
