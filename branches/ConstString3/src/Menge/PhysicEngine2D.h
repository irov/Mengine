// #	pragma once
//
//#	include "Config/Typedef.h"
//#	include "Core/Holder.h"
//#	include "Math/vec2.h"
//
//#	include <vector>
//
//namespace Menge
//{
//	class PhysicSystem2DInterface;
//	class PhysicBody2DInterface;
//	class PhysicJoint2DInterface;
//
//	class RigidBody2D;
//
//	class PhysicEngine2D
//		: public Holder<PhysicEngine2D>
//	{
//	public:
//		PhysicEngine2D();
//		~PhysicEngine2D();
//
//		bool initialize();
//
//		bool isWorldCreate() const;
//
//		void createWorld( const mt::vec2f& _upperLeft, const mt::vec2f& _lowerRight, const mt::vec2f& _gravity, bool _doSleep = true );
//		void destroyWorld();
//
//			
//		PhysicBody2DInterface* createBody( const mt::vec2f& _pos, float _angle, float _linearDamping, float _angularDamping,
//													bool _allowSleep, bool _isBullet, bool _fixedRotation );
//
//		void destroyPhysicBody( PhysicBody2DInterface* _bodyInterface );
//
//		PhysicJoint2DInterface*  createDistanceJoint( RigidBody2D* _body1, RigidBody2D* _body2, const mt::vec2f& _offsetBody1, const mt::vec2f& _offsetBody2, bool _collideBodies );
//		PhysicJoint2DInterface*  createHingeJoint( RigidBody2D* _body1, RigidBody2D* _body2, const mt::vec2f& _offsetBody1, const mt::vec2f& _limits, bool _collideBodies );
//		PhysicJoint2DInterface*  createMouseJoint( RigidBody2D* _body, int _x, int _y  );
//		void destroyJoint( PhysicJoint2DInterface* _joint );
//
//		void update( float _timing );
//
//		void onMouseMove( const mt::vec2f & _delta );
//
//		const mt::vec2f& getGravity() const;
//
//		float getPhase() const;
//
//	private:
//		PhysicSystem2DInterface* m_interface;
//		float m_timing;
//		float m_timeStep;
//		int m_iterating;
//		mt::vec2f m_gravity;
//		float m_phase;
//		bool m_inProcess;
//
//		typedef std::vector<PhysicBody2DInterface*> TVectorBodies;
//		TVectorBodies m_bodies;
//	};
//}	// namespace Menge
