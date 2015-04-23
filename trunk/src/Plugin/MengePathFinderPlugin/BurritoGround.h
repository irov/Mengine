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
		void translate( const mt::vec3f & _translate );

	public:
		bool check_collision( float _timing, const mt::vec3f & _position, float _radius, const mt::vec3f & _velocity, float & _collisionTiming, mt::vec3f & _newVelocity ) const;

	protected:		
		mt::planef m_plane;

		PyObject * m_cb;
	};
}