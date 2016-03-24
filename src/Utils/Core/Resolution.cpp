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
	Resolution::Resolution( uint32_t _width, uint32_t _height )
		: m_width(_width)
		, m_height(_height)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Resolution::setWidth( uint32_t _width )
	{
		m_width = _width;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t Resolution::getWidth() const
	{
		return m_width;
	}
	//////////////////////////////////////////////////////////////////////////
	void Resolution::setHeight( uint32_t _height )
	{
		m_height = _height;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t Resolution::getHeight() const
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
	void Resolution::calcInvSize( mt::vec2f & _size ) const
	{
		_size.x = 1.f / float( m_width );
		_size.y = 1.f / float( m_height );
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
