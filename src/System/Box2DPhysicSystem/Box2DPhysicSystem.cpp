
#	include "Box2DPhysicSystem.h"
#	include "Box2DPhysicBody.h"
#	include "Box2DPhysicJoint.h"
#	include <algorithm>

#	define MAX_CONTACTS_NUM 1024
//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem( PhysicSystem2DInterface **_system )
{
	try
	{
		*_system = new Box2DPhysicSystem();
	}
	catch (...)
	{
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
void releaseInterfaceSystem( PhysicSystem2DInterface *_system )
{
	delete static_cast<Box2DPhysicSystem*>( _system );
}
//////////////////////////////////////////////////////////////////////////
Box2DPhysicSystem::Box2DPhysicSystem()
: m_world( 0 )
{
}
//////////////////////////////////////////////////////////////////////////
Box2DPhysicSystem::~Box2DPhysicSystem()
{
	if( m_world )
	{
		delete m_world;
		m_world = 0;
	}
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicSystem::createWorld( const float* _upperLeft, const float* _lowerRight, const float* _gravity, bool _doSleep )
{
	if( m_world ) return;

	b2AABB worldAABB;
	worldAABB.lowerBound.Set( _upperLeft[0] * physicsScaler, _upperLeft[1] * physicsScaler );
	worldAABB.upperBound.Set( _lowerRight[0] * physicsScaler, _lowerRight[1] * physicsScaler );
	b2Vec2 gravity( _gravity[0], _gravity[1] );

	m_world = new b2World( worldAABB, gravity, _doSleep );
	m_world->SetContactListener( this );
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicSystem::destroyWorld()
{
	delete m_world;
	m_world = 0;
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicSystem::update( float _timing, int _iterations )
{
	if( !m_world ) return;

	/*for( b2Body* body = m_world->GetBodyList(); body; body = body->GetNext() )
	{
		body->WakeUp();
	}*/
	/*for( TJointDefList::iterator it = m_jointDefList.begin(),
		it_end = m_jointDefList.end();
		it != it_end;
	it++ )
	{
		_createJoint( it->first, it->second );
		delete it->first;
	}
	m_jointDefList.clear();*/
	for( TContactPointList::iterator it = m_contacts.begin(),
		it_end = m_contacts.end(); it != it_end; it++ )
	{
		b2Body* body1 = it->shape1->GetBody();
		b2Body* body2 = it->shape2->GetBody();

		static_cast<Box2DPhysicBody*>( body1->GetUserData() )->_collide( body2, &(*it) );
		static_cast<Box2DPhysicBody*>( body2->GetUserData() )->_collide( body1, &(*it) );
	}

	m_contacts.clear();
	m_world->Step( _timing, _iterations );

	//m_world->m_broadPhase->Validate();


}
//////////////////////////////////////////////////////////////////////////
PhysicBody2DInterface* Box2DPhysicSystem::createDynamicBody( const float* _pos, float _angle, float _linearDamping, float _angularDamping,
														   bool _allowSleep, bool _isBullet, bool _fixedRotation )
{
	b2BodyDef bodyDef;

	bodyDef.position.Set( _pos[0] * physicsScaler, _pos[1] * physicsScaler );

	bodyDef.angle = _angle;
	bodyDef.linearDamping = _linearDamping;
	bodyDef.angularDamping = _angularDamping;
	bodyDef.allowSleep = _allowSleep;
	bodyDef.isBullet = _isBullet;
	bodyDef.fixedRotation = _fixedRotation;
	
	Box2DPhysicBody* body = new Box2DPhysicBody( m_world, false );
	bodyDef.userData = body;

	body->initialize( bodyDef );

	return static_cast<PhysicBody2DInterface*>( body );
}
//////////////////////////////////////////////////////////////////////////
PhysicBody2DInterface* Box2DPhysicSystem::createStaticBody( const float* _pos, float _angle )
{
	b2BodyDef bodyDef;
	bodyDef.position.Set( _pos[0] * physicsScaler, _pos[1] * physicsScaler );
	bodyDef.angle = _angle;

	Box2DPhysicBody* body = new Box2DPhysicBody( m_world, true );
	bodyDef.userData = body;
	body->initialize( bodyDef );

	return static_cast<PhysicBody2DInterface*>( body );
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicSystem::destroyBody( PhysicBody2DInterface* _body )
{
	delete static_cast<Box2DPhysicBody*>( _body );
}
//////////////////////////////////////////////////////////////////////////
PhysicJoint2DInterface* Box2DPhysicSystem::createDistanceJoint( PhysicBody2DInterface* _body1, PhysicBody2DInterface* _body2, const float* _offsetBody1, const float* _offsetBody2, bool _collideBodies )
{
	b2DistanceJointDef* jointDef = new b2DistanceJointDef();
	b2Body* body1 = static_cast<Box2DPhysicBody*>( _body1 )->getBody();
	b2Body* body2 = static_cast<Box2DPhysicBody*>( _body2 )->getBody();
	jointDef->collideConnected = _collideBodies;

	b2Vec2 offsetBody1( _offsetBody1[0] * physicsScaler, _offsetBody1[1] * physicsScaler);
	b2Vec2 offsetBody2( _offsetBody2[0] * physicsScaler, _offsetBody2[1] * physicsScaler);

	b2Vec2 positionBody1 = body1->GetPosition();
	b2Vec2 positionBody2 = body2->GetPosition();

	b2Vec2 anchor1 = positionBody1 + offsetBody1;
	b2Vec2 anchor2 = positionBody2 + offsetBody2;

	jointDef->Initialize( body1, body2, anchor1, anchor2 );

	Box2DPhysicJoint* joint = new Box2DPhysicJoint( m_world, NULL );

	//if( m_world->m_lock )
	//{
	//	m_jointDefList.push_back( std::make_pair( static_cast<b2JointDef*>( jointDef ), joint ) );
	//}
	//else
	//{
		_createJoint( jointDef, joint );
		delete jointDef;
	//}
	return static_cast<PhysicJoint2DInterface*>( joint );
}
//////////////////////////////////////////////////////////////////////////
PhysicJoint2DInterface* Box2DPhysicSystem::createHingeJoint( PhysicBody2DInterface* _body1, PhysicBody2DInterface* _body2, const float* _offsetBody1, const float* _limits, bool _collideBodies )
{
	b2RevoluteJointDef* jointDef = new b2RevoluteJointDef();
	b2Body* body1 = static_cast<Box2DPhysicBody*>( _body1 )->getBody();
	b2Body* body2 = static_cast<Box2DPhysicBody*>( _body2 )->getBody();
	jointDef->collideConnected = _collideBodies;
	

	b2Vec2 offsetBody1( _offsetBody1[0] * physicsScaler, _offsetBody1[1] * physicsScaler);
	b2Vec2 positionBody1 = body1->GetPosition();
	
	b2Vec2 anchor1 = positionBody1 + offsetBody1;

	if( _limits[0] != _limits[1] )
	{
		jointDef->enableLimit = true;
		jointDef->lowerAngle = _limits[0];
		jointDef->upperAngle = _limits[1];
	}
	jointDef->Initialize( body1, body2, anchor1 );
	
	Box2DPhysicJoint* joint = new Box2DPhysicJoint( m_world, NULL );
	//if( m_world->m_lock )
	//{
	//	m_jointDefList.push_back( std::make_pair( static_cast<b2JointDef*>( jointDef ), joint ) );
	//}
	//else
	//{
		_createJoint( jointDef, joint );
		delete jointDef;
	//}

	return static_cast<PhysicJoint2DInterface*>( joint );
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicSystem::destroyJoint( PhysicJoint2DInterface* _joint )
{
	Box2DPhysicJoint* joint = static_cast<Box2DPhysicJoint*>( _joint );
	delete joint;
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicSystem::Add( b2ContactPoint* point )
{
	m_contacts.push_back( *point );

	/*b2Body* body1 = point->shape1->GetBody();
	b2Body* body2 = point->shape2->GetBody();

	static_cast<Box2DPhysicBody*>( body1->GetUserData() )->_collide( body2, point );
	static_cast<Box2DPhysicBody*>( body2->GetUserData() )->_collide( body1, point );*/
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicSystem::Persist( b2ContactPoint* point )
{
	// Nothing to hold this time
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicSystem::Remove( b2ContactPoint* point )
{
	// Nothing to hold this time
	//m_contacts.erase( std::remove( m_contacts.begin(), m_contacts.end(), point ) );
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicSystem::_createJoint( b2JointDef* _jointDef, Box2DPhysicJoint* _joint )
{
	b2Joint* joint = m_world->CreateJoint( _jointDef );
	_joint->_setJoint( joint );
}