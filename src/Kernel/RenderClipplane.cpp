#include "RenderClipplane.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	RenderClipplane::RenderClipplane()
		: m_count( 0 )
		, m_invalidateClipplanes(true)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderClipplane::_activate()
	{
		if( Node::_activate() == false )
		{
			return true;
		}

		this->invalidateClipplanes_();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderClipplane::setPlanes( uint32_t _count, const mt::planef * _planes )
	{
		m_count = _count;

		for( uint32_t i = 0; i != _count; ++i )
		{
			m_planes[i] = _planes[i];
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderClipplane::setViewport( const Viewport & _viewport )
	{ 
		m_count = 4;

		mt::vec3f v0( _viewport.begin.x, _viewport.begin.y, 0.f);
		mt::vec3f v0z( _viewport.begin.x, _viewport.begin.y, -10.f );

		mt::vec3f v1( _viewport.end.x, _viewport.begin.y, 0.f );

		mt::vec3f v2( _viewport.end.x, _viewport.end.y, 0.f );
		mt::vec3f v2z( _viewport.end.x, _viewport.end.y, -10.f );
		
		mt::vec3f v3( _viewport.begin.x, _viewport.end.y, 0.f );

		mt::planef p0;
		mt::set_plane_by_triangle( p0, v3, v0, v0z );
		
		mt::planef p1;
		mt::set_plane_by_triangle( p1, v0, v1, v0z );

		mt::planef p2;
		mt::set_plane_by_triangle( p2, v1, v2, v2z );

		mt::planef p3;
		mt::set_plane_by_triangle( p3, v2, v3, v2z );

		m_planes[0] = p0;
		m_planes[1] = p1;
		m_planes[2] = p2;
		m_planes[3] = p3;

		this->invalidateClipplanes_();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderClipplane::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();

		this->invalidateClipplanes_();
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderClipplane::updateClipplanes_() const
	{
		m_invalidateClipplanes = false;

		const mt::mat4f & wm = this->getWorldMatrix();

		for( uint32_t i = 0; i != m_count; ++i )
		{
			mul_plane_mat4( m_planesWM[i], m_planes[i], wm );
		}
	}	
}
