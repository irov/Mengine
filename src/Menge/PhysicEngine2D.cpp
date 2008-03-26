
#	include "PhysicEngine2D.h"
#	include "Interface/PhysicSystem2DInterface.h"
#	include "RigidBody2D.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PhysicEngine2D::PhysicEngine2D( PhysicSystem2DInterface * _interface )
		: m_interface( _interface )
	{
		Holder<PhysicEngine2D>::keep( this );

		//createScene( mt::vec2f( 0.0f, 0.0f ), mt::vec2f(2048.0f, 2048.0f), mt::vec2f(0.0f, 50.0f), true );
	}
	//////////////////////////////////////////////////////////////////////////
	PhysicEngine2D::~PhysicEngine2D()
	{
		//destroyScene();
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicEngine2D::createScene( const mt::vec2f& _upperLeft, const mt::vec2f& _lowerRight, const mt::vec2f& _gravity, bool _doSleep /* = true  */)
	{
		m_interface->createWorld( &_upperLeft.x, &_lowerRight.x, &_gravity.x, _doSleep );
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicEngine2D::destroyScene()
	{
		m_interface->destroyWorld();
	}
	//////////////////////////////////////////////////////////////////////////
	PhysicBody2DInterface* PhysicEngine2D::createDynamicBody( const mt::vec2f& _pos, float _angle, float _linearDamping, float _angularDamping,
															 bool _allowSleep, bool _isBullet, bool _fixedRotation )
	{
		return m_interface->createDynamicBody( _pos.m, _angle, _linearDamping, _angularDamping, _allowSleep, _isBullet, _fixedRotation );
	}
	//////////////////////////////////////////////////////////////////////////
	PhysicBody2DInterface* PhysicEngine2D::createStaticBody( const mt::vec2f& _pos, float _angle )
	{
		return m_interface->createStaticBody( _pos.m, _angle );
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicEngine2D::destroyPhysicBody( PhysicBody2DInterface* _bodyInterface )
	{
		m_interface->destroyBody( _bodyInterface );
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicEngine2D::update( float _timing, int _iterations )
	{
		m_interface->update( _timing, _iterations );
	}
	//////////////////////////////////////////////////////////////////////////
	PhysicJoint2DInterface* PhysicEngine2D::createDistanceJoint( RigidBody2D* _body1, RigidBody2D* _body2, const mt::vec2f& _offsetBody1, const mt::vec2f& _offsetBody2, bool _collideBodies )
	{
		return m_interface->createDistanceJoint( _body1->getInterface(), _body2->getInterface(), _offsetBody1.m, _offsetBody2.m, _collideBodies );
	}
	//////////////////////////////////////////////////////////////////////////
	PhysicJoint2DInterface* PhysicEngine2D::createHingeJoint( RigidBody2D* _body1, RigidBody2D* _body2, const mt::vec2f& _offsetBody1, bool _collideBodies )
	{
		return m_interface->createHingeJoint( _body1->getInterface(), _body2->getInterface(), _offsetBody1.m, _collideBodies );
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicEngine2D::destroyJoint( PhysicJoint2DInterface* _joint )
	{
		m_interface->destroyJoint( _joint );
	}
	//////////////////////////////////////////////////////////////////////////
} // namespace Menge