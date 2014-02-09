#	pragma once

namespace Menge
{
	class CollisionObject;

	class CollisionMotor
	{
	public:
		CollisionMotor();
		~CollisionMotor();

	public:
		void setLinearSpeed( float _speed );
		float getLinearSpeed() const;

		void setAngularSpeed( float _speed );
		float getAngularSpeed() const;

	public:
		virtual void update( float _timing, CollisionObject * _object ) = 0;

	protected:
		float m_linearSpeed;
		float m_angularSpeed;
	};	
}
