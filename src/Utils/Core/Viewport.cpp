#	include "Viewport.h"

#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Viewport::Viewport()
		: begin(0.f, 0.f)
		, end(0.f, 0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Viewport::Viewport( const Viewport & _viewport )
		: begin(_viewport.begin)
		, end(_viewport.end)
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	Viewport::Viewport( const mt::vec2f & _begin, const mt::vec2f & _end )
	: begin(_begin)
	, end(_end)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Viewport::Viewport( float _beginX, float _beginY, float _endX, float _endY )
		: begin(_beginX, _beginY)
		, end(_endX, _endY)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Viewport::initialize( const mt::vec2f & _begin, const mt::vec2f & _end )
	{
		begin = _begin;
		end = _end;
	}
	//////////////////////////////////////////////////////////////////////////
	void Viewport::parallax( const mt::vec2f & _factor )
	{
		mt::vec2f size = end - begin;
		begin.x *= _factor.x;
		begin.y *= _factor.y;
		end.x = begin.x + size.x;
		end.y = begin.y + size.y;
	}
	//////////////////////////////////////////////////////////////////////////
	void Viewport::scale( const mt::vec2f & _value )
	{
		begin *= _value;
		end *= _value;
	}
	//////////////////////////////////////////////////////////////////////////
	void Viewport::inflate( const mt::vec2f & _value )
	{
		mt::vec2f half_size = (end - begin) * 0.5f;
		mt::vec2f center = (end + begin) * 0.5f;
		begin = center - half_size * _value;
		end = center + half_size * _value;
	}
	//////////////////////////////////////////////////////////////////////////
	float Viewport::getWidth() const
	{
		return end.x - begin.x;
	}
	//////////////////////////////////////////////////////////////////////////
	float Viewport::getHeight() const
	{
		return end.y - begin.y;
	}
	//////////////////////////////////////////////////////////////////////////
	void Viewport::calcSize( mt::vec2f & _size ) const
	{
		_size = end - begin;
	}
	//////////////////////////////////////////////////////////////////////////
	void Viewport::getCenter( mt::vec2f & _point ) const
	{
		_point = (begin + end) * 0.5f;
	}
	//////////////////////////////////////////////////////////////////////////
	void Viewport::centerize( const mt::vec2f & _center )
	{
		begin -= _center;
		end -= _center;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Viewport::empty() const
	{
		float width = this->getWidth();
		float height = this->getHeight();

		if( width > 0.f && height > 0.f )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Viewport::equalViewport( const Viewport & _vp ) const
	{
		if( begin != _vp.begin )
		{
			return false;
		}

		if( end != _vp.end )
		{
			return false;
		}

		return true;
	}
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
	bool Viewport::testRectangle( const mt::vec2f & _min, const mt::vec2f & _max ) const
	{
		return mt::is_intersect( _min, _max, begin, end );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Viewport::testBBox( const mt::box2f& _bbox ) const
	{
		return this->testRectangle( _bbox.minimum, _bbox.maximum );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Viewport::existRectangle( const mt::vec2f & _min, const mt::vec2f & _max ) const
	{
		return mt::is_exist( _min, _max, begin, end );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Viewport::existBBox( const mt::box2f& _bbox ) const
	{
		return this->existRectangle( _bbox.minimum, _bbox.maximum );
	}
	//////////////////////////////////////////////////////////////////////////
	void Viewport::toBBox( mt::box2f & _box ) const
	{
		mt::set_box_from_min_max( _box, begin, end );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Viewport::intersectBBox( const mt::box2f& _bbox ) const
	{
		bool result = mt::is_intersect( _bbox.minimum, _bbox.maximum, this->begin, this->end );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	float Viewport::getIntersectionSquareBBox( const mt::box2f& _bbox ) const
	{
		if( mt::is_intersect( _bbox.minimum, _bbox.maximum, this->begin, this->end ) == false )
		{
			return 0.f;
		}

		float sx[4] = {_bbox.minimum.x, _bbox.maximum.x, this->begin.x, this->end.x};
		std::sort( sx, sx + 4 );

		float sy[4] = {_bbox.minimum.y, _bbox.maximum.y, this->begin.y, this->end.y};
		std::sort( sy, sy + 4 );

		float s = (sx[2] - sx[1])*(sy[2] - sy[1]);

		return s;
	}
	//////////////////////////////////////////////////////////////////////////
	float Viewport::getSquare() const
	{
		float s = (this->end.x - this->begin.x) * (this->end.y - this->begin.y);

		return s;
	}
}
