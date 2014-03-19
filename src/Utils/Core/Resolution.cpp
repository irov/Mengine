#	include "Resolution.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Resolution::Resolution()
		: m_width(0)
		, m_height(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Resolution::Resolution( size_t _width, size_t _height )
		: m_width(_width)
		, m_height(_height)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Resolution::setWidth( size_t _width )
	{
		m_width = _width;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Resolution::getWidth() const
	{
		return m_width;
	}
	//////////////////////////////////////////////////////////////////////////
	void Resolution::setHeight( size_t _height )
	{
		m_height = _height;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Resolution::getHeight() const
	{
		return m_height;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Resolution::operator == ( const Resolution & _resolution ) const
	{
		return m_width == _resolution.m_width && m_height == _resolution.m_height;
	}
	//////////////////////////////////////////////////////////////////////////
	float Resolution::getAspectRatio() const
	{
		float drx = float(m_width);
		float dry = float(m_height);
		float aspect = drx / dry;
		return aspect;
	}
	//////////////////////////////////////////////////////////////////////////
	void Resolution::calcSize( mt::vec2f & _size ) const
	{
		_size.x = float(m_width);
		_size.y = float(m_height);
	}
	//////////////////////////////////////////////////////////////////////////
	void Resolution::calcScale( const Resolution & _resolution, mt::vec2f & _scale ) const
	{
		mt::vec2f self_size;
		this->calcSize( self_size );

		mt::vec2f other_size;
		_resolution.calcSize( other_size );

		_scale = self_size / other_size;
	}
}
