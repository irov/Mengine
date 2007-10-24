#	include "Viewport.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Viewport::Viewport()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Viewport::Viewport( const mt::vec2f & _begin, const mt::vec2f & _end )
	: begin( _begin )
	, end( _end )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Viewport::testPoint( const mt::vec2f & _point ) const
	{
		if( begin.x > _point.x ) return false;
		if( begin.y > _point.y ) return false;
		if( end.x < _point.x ) return false;
		if( end.y < _point.y ) return false;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Viewport::testRectangle( const mt::vec2f & _min, const mt::vec2f & _max ) const
	{
		if ( _max.x < begin.x || _min.x > end.x ) return false;
		if ( _max.y < begin.y || _min.y > end.y ) return false;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}