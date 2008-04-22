 #	pragma once

#	include "Holder.h"
#	include "Math/vec2.h"
#	include "Math/polygon.h"

class PhysicSystem2DInterface;
class PhysicBody2DInterface;
class PhysicJoint2DInterface;

namespace Menge
{

	class RigidBody2D;

	class PhysicEngine2D
	{
	public:
		PhysicEngine2D(  PhysicSystem2DInterface * _interface );
		~PhysicEngine2D();

		void createScene( const mt::vec2f& _upperLeft, const mt::vec2f& _lowerRight, const mt::vec2f& _gravity, bool _doSleep = true );
		void destroyScene();

			
		PhysicBody2DInterface* createDynamicBody( const mt::vec2f& _pos, float _angle, float _linearDamping, float _angularDamping,
													bool _allowSleep, bool _isBullet, bool _fixedRotation );
		PhysicBody2DInterface* createStaticBody( const mt::vec2f& _pos, float _angle );

		void destroyPhysicBody( PhysicBody2DInterface* _bodyInterface );

		PhysicJoint2DInterface*  createDistanceJoint( RigidBody2D* _body1, RigidBody2D* _body2, const mt::vec2f& _offsetBody1, const mt::vec2f& _offsetBody2, bool _collideBodies );
		PhysicJoint2DInterface*  createHingeJoint( RigidBody2D* _body1, RigidBody2D* _body2, const mt::vec2f& _offsetBody1, const mt::vec2f& _limits, bool _collideBodies );
		void destroyJoint( PhysicJoint2DInterface* _joint );

		void update( float _timing );

	private:
		PhysicSystem2DInterface* m_interface;
		float m_timing;
		float m_timeStep;
		int m_iterating;
	};
}	// namespace Menge