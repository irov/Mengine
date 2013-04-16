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
	mt::vec2f Resolution::getScale( const Resolution & _resolution ) const
	{
		mt::vec2f scale;

		scale.x = float(m_width) / float(_resolution.getWidth());
		scale.y = float(m_height) / float(_resolution.getHeight());

		return scale;
	}
}
