#	pragma once

#	include "CollisionMotor.h"

#	include "Math/vec3.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	class CollisionMotorPosition
		: public CollisionMotor
	{
	public:
		CollisionMotorPosition();
		~CollisionMotorPosition();

	public:
		void setCallback( PyObject * _cb );

	public:
		void setPositionAngle( const mt::vec3f & _position, float _angle );
		void setPositionLookAt( const mt::vec3f & _position, const mt::vec3f & _lookAt );

	public:
		void update( float _timing, CollisionObject * _object ) override;

	protected:
		mt::vec3f m_position;
		float m_angle;

		PyObject * m_cb;
	};	
}
