#pragma once

#include "math/plane.h"

#include "pybind/object.hpp"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class BurritoGround
	{
	public:
		BurritoGround();
		~BurritoGround();

	public:
		void initialize( const mt::planef & _plane, const pybind::object & _cb );

	public:
		bool check_collision( float _timing, const mt::vec3f & _position, float _radius, const mt::vec3f & _velocity, float & _collisionTiming ) const;

	protected:		
		mt::planef m_plane;

		pybind::object m_cb;
	};
}