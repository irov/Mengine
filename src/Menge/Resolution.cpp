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
	Resolution::Resolution( std::size_t _width, std::size_t _height )
		: m_width(_width)
		, m_height(_height)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Resolution::setWidth( std::size_t _width )
	{
		m_width = _width;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t Resolution::getWidth() const
	{
		return m_width;
	}
	//////////////////////////////////////////////////////////////////////////
	void Resolution::setHeight( std::size_t _height )
	{
		m_height = _height;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t Resolution::getHeight() const
	{
		return m_height;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t & Resolution::operator [] ( int i )
	{
		return m_size[i];
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t Resolution::operator [] ( int i ) const
	{
		return m_size[i];
	}
	//////////////////////////////////////////////////////////////////////////
	float Resolution::getRatio() const
	{
		float drx = float(m_width);
		float dry = float(m_height);
		float aspect = drx / dry;
		return aspect;
	}
	//////////////////////////////////////////////////////////////////////////
}