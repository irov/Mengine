#	include "Viewport.h"

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
	void Viewport::getSize( mt::vec2f & _size ) const
	{
		float w = this->getWidth();
		float h = this->getHeight();

		_size.x = w;
		_size.y = h;
	}
}
