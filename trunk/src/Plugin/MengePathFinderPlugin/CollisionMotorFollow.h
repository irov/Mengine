#	pragma once

#	include "CollisionMotor.h"

namespace Menge
{
	class CollisionMotorFollow
		: public CollisionMotor
	{
	public:
		CollisionMotorFollow();
		~CollisionMotorFollow();

	public:
		void setTarget( CollisionObject * _target );
		CollisionObject * getTarget() const;

		void setMinimalDistance( float _distance );
		float getMinimalDistance() const;

	public:
		void update( float _timing, CollisionObject * _object ) override;

	protected:
		CollisionObject * m_target;

		float m_minimalDistance;
	};	
}
