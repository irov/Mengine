#	pragma once

#	include "CollisionMotor.h"

#	include "Kernel/Node.h"

namespace Menge
{
	class CollisionTesterInterface;

	class CollisionObject
		: public Node
	{
	public:
		CollisionObject();
		~CollisionObject();

	public:
		void setRadius( float _radius );
		float getRadius() const;

		void setMass( float _mass );
		float getMass() const;

		void setMotor( CollisionMotor * _motor );
		CollisionMotor * getMotor() const;

	public:
		void setDead( bool _dead );
		bool getDead() const;

	public:
		void setGhost( bool _ghost );
		bool getGhost() const;

	public:
		void update( float _timing );

	protected:
		float m_radius;
		float m_mass;

		CollisionMotor * m_motor;

		bool m_ghost;
		bool m_dead;
	};	
}
