#	pragma once

#	include "CollisionMotor.h"

#	include "Kernel/Node.h"

namespace Menge
{
	class CollisionTesterInterface;

	class CollisionObject
	{
	public:
		CollisionObject();
		~CollisionObject();

	public:
		void setNode( Node * _node );
		Node * getNode() const;

		void setRadius( float _radius );
		float getRadius() const;

		void setMass( float _mass );
		float getMass() const;

		void setMotor( CollisionMotor * _motor );
		CollisionMotor * getMotor() const;

	public:
		void update( float _timing );

	protected:
		Node * m_node;

		float m_radius;
		float m_mass;

		CollisionMotor * m_motor;
	};	
}
