#	include "BurritoGround.h"

#	include "Math/ccd.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	BurritoGround::BurritoGround()
		: m_cb(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	BurritoGround::~BurritoGround()
	{
		pybind::decref( m_cb );
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoGround::initialize( const mt::planef & _plane, PyObject * _cb )
	{
		m_plane = _plane;
		m_cb = _cb;

		pybind::incref( m_cb );
	}
	//////////////////////////////////////////////////////////////////////////
	bool BurritoGround::check_collision( const mt::vec3f & _position, float _radius, const mt::vec3f & _velocity, float _timing, float & _collisionTiming, float & _factor ) const
	{
		float potential_collisionTiming;
		mt::vec3f normal_collision;
		if( mt::ccd_sphere_plane( _position, _radius, _velocity, m_plane, potential_collisionTiming, normal_collision ) == false )
		{ 
			return false;
		}

		if( _timing > potential_collisionTiming )
		{
			return false;
		}

		_collisionTiming = potential_collisionTiming - _timing;

		PyObject * py_factor = pybind::ask( m_cb, "()" );

		_factor = pybind::extract<float>( py_factor );

		return true;
	}
}