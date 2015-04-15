#	pragma once

#	include "Math/plane.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class BurritoGround
	{
	public:
		BurritoGround();
		~BurritoGround();

	public:
		void initialize( const mt::planef & _plane, PyObject * _cb );

	public:
		bool check_collision( const mt::vec3f & _position, float _radius, const mt::vec3f & _velocity, float _timing, float & _collisionTiming, mt::vec2f & _factor ) const;

	protected:		
		mt::planef m_plane;

		PyObject * m_cb;
	};
}