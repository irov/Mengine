#	include "BurritoGround.h"

#	include "Math/ccd.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	BurritoGround::BurritoGround()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	BurritoGround::~BurritoGround()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoGround::initialize( const mt::planef & _plane, const pybind::object & _cb )
	{
		m_plane = _plane;
		m_cb = _cb;
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoGround::translate( const mt::vec3f & _translate )
	{ 
		m_plane.d += _translate.y;
	}
	//////////////////////////////////////////////////////////////////////////
	bool BurritoGround::check_collision( float _timing, const mt::vec3f & _position, float _radius, const mt::vec3f & _velocity, float & _collisionTiming ) const
	{
		float ccd_timing;
		if( mt::ccd_sphere_plane( _position, _radius, _velocity, m_plane, ccd_timing ) == false )
		{ 
			return false;
		}

		if( ccd_timing < 0.f )
		{
			return false;
		}

		if( ccd_timing > _timing )
		{
			return false;
		}

		_collisionTiming = ccd_timing;
		
		m_cb( _velocity );

		return true;
	}
}