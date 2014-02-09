#	pragma once

#	include "CollisionMotor.h"

#	include "Math/vec3.h"

namespace Menge
{
	class CollisionMotorPosition
		: public CollisionMotor
	{
	public:
		CollisionMotorPosition();
		~CollisionMotorPosition();

	public:
		void setPosition( const mt::vec3f & _position, float _angle );
		
	public:
		void update( float _timing, CollisionObject * _object ) override;

	protected:
		mt::vec3f m_position;
		float m_angle;
	};	
}
