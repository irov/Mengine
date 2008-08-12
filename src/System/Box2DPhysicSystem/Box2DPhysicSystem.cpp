
#	include "Box2DPhysicSystem.h"
#	include "Box2DPhysicBody.h"
#	include "Box2DPhysicJoint.h"
#	include <algorithm>

#	define MAX_CONTACTS_NUM 1024
//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem( Menge::PhysicSystem2DInterface **_system )
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
void releaseInterfaceSystem( Menge::PhysicSystem2DInterface *_system )
{
	delete static_cast<Box2DPhysicSystem*>( _system );
}
//////////////////////////////////////////////////////////////////////////
Box2DPhysicSystem::Box2DPhysicSystem()
: m_world( 0 )
, m_mouseJoint( 0 )
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
	worldAABB.lowerBound.Set( _upperLeft[0] * Menge::physicsScaler, _upperLeft[1] * Menge::physicsScaler );
	worldAABB.upperBound.Set( _lowerRight[0] * Menge::physicsScaler, _lowerRight[1] * Menge::physicsScaler );
	b2Vec2 gravity( _gravity[0], _gravity[1] );

	m_world = new b2World( worldAABB, gravity, _doSleep );
	m_world->SetContactListener( this );

}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicSystem::destroyWorld()
{
	delete m_world;
	m_world = 0;
	m_contacts.clear();
	m_deletingBodies.clear();
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicSystem::update( float _timing, int _velocityIterations, int _positionIterations )
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

		Box2DPhysicBody* mBody1 = static_cast<Box2DPhysicBody*>( body1->GetUserData() );
		Box2DPhysicBody* mBody2 = static_cast<Box2DPhysicBody*>( body2->GetUserData() );
		/*if( !body1 || !body2 )
		{
			continue;
		}*/
		mBody1->_collide( body2, &(*it) );
		mBody2->_collide( body1, &(*it) );
	}
	m_contacts.clear();

	for( TBodyVector::iterator it = m_deletingBodies.begin(), it_end = m_deletingBodies.end();
		it != it_end;
		it++ )
	{
		delete (*it);
	}
	m_deletingBodies.clear();

	m_world->Step( _timing, _velocityIterations, _positionIterations );

	//m_world->m_broadPhase->Validate();


}
//////////////////////////////////////////////////////////////////////////
Menge::PhysicBody2DInterface* Box2DPhysicSystem::createDynamicBody( const float* _pos, float _angle, float _linearDamping, float _angularDamping,
														   bool _allowSleep, bool _isBullet, bool _fixedRotation )
{
	b2BodyDef bodyDef;

	bodyDef.position.Set( _pos[0] * Menge::physicsScaler, _pos[1] * Menge::physicsScaler );

	bodyDef.angle = _angle;
	bodyDef.linearDamping = _linearDamping;
	bodyDef.angularDamping = _angularDamping;
	bodyDef.allowSleep = _allowSleep;
	bodyDef.isBullet = _isBullet;
	bodyDef.fixedRotation = _fixedRotation;
	
	Box2DPhysicBody* body = new Box2DPhysicBody( m_world, false );
	bodyDef.userData = body;

	body->initialize( bodyDef );

	return static_cast<Menge::PhysicBody2DInterface*>( body );
}
//////////////////////////////////////////////////////////////////////////
Menge::PhysicBody2DInterface* Box2DPhysicSystem::createStaticBody( const float* _pos, float _angle )
{
	b2BodyDef bodyDef;
	bodyDef.position.Set( _pos[0] * Menge::physicsScaler, _pos[1] * Menge::physicsScaler );
	bodyDef.angle = _angle;

	Box2DPhysicBody* body = new Box2DPhysicBody( m_world, true );
	bodyDef.userData = body;
	body->initialize( bodyDef );

	return static_cast<Menge::PhysicBody2DInterface*>( body );
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicSystem::destroyBody( Menge::PhysicBody2DInterface* _body )
{
	//delete static_cast<Box2DPhysicBody*>( _body );
	Box2DPhysicBody* body = static_cast<Box2DPhysicBody*>( _body );
	body->setUserData( 0 );
	body->setBodyListener( 0 );
	m_deletingBodies.push_back( body );
}
//////////////////////////////////////////////////////////////////////////
Menge::PhysicJoint2DInterface* Box2DPhysicSystem::createDistanceJoint( Menge::PhysicBody2DInterface* _body1, Menge::PhysicBody2DInterface* _body2, const float* _offsetBody1, const float* _offsetBody2, bool _collideBodies )
{
	b2DistanceJointDef* jointDef = new b2DistanceJointDef();
	b2Body* body1 = static_cast<Box2DPhysicBody*>( _body1 )->getBody();
	b2Body* body2 = static_cast<Box2DPhysicBody*>( _body2 )->getBody();
	jointDef->collideConnected = _collideBodies;

	b2Vec2 offsetBody1( _offsetBody1[0] * Menge::physicsScaler, _offsetBody1[1] * Menge::physicsScaler);
	b2Vec2 offsetBody2( _offsetBody2[0] * Menge::physicsScaler, _offsetBody2[1] * Menge::physicsScaler);

	b2Vec2 positionBody1 = body1->GetPosition();
	b2Vec2 positionBody2 = body2->GetPosition();

	b2Vec2 anchor1 = positionBody1 + offsetBody1;
	b2Vec2 anchor2 = positionBody2 + offsetBody2;

	jointDef->Initialize( body1, body2, anchor1, anchor2 );

	Box2DPhysicJoint* joint = new Box2DPhysicJoint( m_world, NULL, false );

	//if( m_world->m_lock )
	//{
	//	m_jointDefList.push_back( std::make_pair( static_cast<b2JointDef*>( jointDef ), joint ) );
	//}
	//else
	//{
		_createJoint( jointDef, joint );
		delete jointDef;
	//}
	return static_cast<Menge::PhysicJoint2DInterface*>( joint );
}
//////////////////////////////////////////////////////////////////////////
Menge::PhysicJoint2DInterface* Box2DPhysicSystem::createHingeJoint( Menge::PhysicBody2DInterface* _body1, Menge::PhysicBody2DInterface* _body2, const float* _offsetBody1, const float* _limits, bool _collideBodies )
{
	b2RevoluteJointDef* jointDef = new b2RevoluteJointDef();
	b2Body* body1 = static_cast<Box2DPhysicBody*>( _body1 )->getBody();
	b2Body* body2 = static_cast<Box2DPhysicBody*>( _body2 )->getBody();
	jointDef->collideConnected = _collideBodies;
	

	b2Vec2 offsetBody1( _offsetBody1[0] * Menge::physicsScaler, _offsetBody1[1] * Menge::physicsScaler);
	b2Vec2 positionBody1 = body1->GetPosition();
	
	b2Vec2 anchor1 = positionBody1 + offsetBody1;

	if( _limits[0] != _limits[1] )
	{
		jointDef->enableLimit = true;
		jointDef->lowerAngle = _limits[0];
		jointDef->upperAngle = _limits[1];
	}
	jointDef->Initialize( body1, body2, anchor1 );
	
	Box2DPhysicJoint* joint = new Box2DPhysicJoint( m_world, NULL, false );
	//if( m_world->m_lock )
	//{
	//	m_jointDefList.push_back( std::make_pair( static_cast<b2JointDef*>( jointDef ), joint ) );
	//}
	//else
	//{
		_createJoint( jointDef, joint );
		delete jointDef;
	//}

	return static_cast<Menge::PhysicJoint2DInterface*>( joint );
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicSystem::destroyJoint( Menge::PhysicJoint2DInterface* _joint )
{
	Box2DPhysicJoint* joint = static_cast<Box2DPhysicJoint*>( _joint );
	if( joint->isMouse() )
	{
		m_mouseJoint = NULL;
	}
	delete joint;
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicSystem::Add( const b2ContactPoint* point )
{
	m_contacts.push_back( *point );

	/*b2Body* body1 = point->shape1->GetBody();
	b2Body* body2 = point->shape2->GetBody();

	static_cast<Box2DPhysicBody*>( body1->GetUserData() )->_collide( body2, point );
	static_cast<Box2DPhysicBody*>( body2->GetUserData() )->_collide( body1, point );*/
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicSystem::Persist( const b2ContactPoint* point )
{
	// Nothing to hold this time
	m_contacts.push_back( *point );
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicSystem::Remove( const b2ContactPoint* point )
{
	// Nothing to hold this time
	//m_contacts.erase( std::remove( m_contacts.begin(), m_contacts.end(), *point ) );
	/*class RemoveIfPred
	{
		const b2ContactPoint* m_point;
	public:
		RemoveIfPred( const b2ContactPoint* _point )
			: m_point( _point )
		{
		}
		bool operator()( const b2ContactPoint& _point )
		{
			if( m_point->id.key == _point.id.key )
			{
				return true;
			}
			return false;
		}
	};
	TContactPointList::iterator it_remove = std::remove_if( m_contacts.begin(), m_contacts.end(), RemoveIfPred(point) );
	if( it_remove != m_contacts.end() )
	{
		m_contacts.erase( it_remove );
	}*/
	/*for( TContactPointList::iterator it = m_contacts.begin(), it_end = m_contacts.end();
		it != it_end;
		it++ )
	{
		if( point->id == it->id )
	}*/
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicSystem::_createJoint( b2JointDef* _jointDef, Box2DPhysicJoint* _joint )
{
	b2Joint* joint = m_world->CreateJoint( _jointDef );
	if( _joint->isMouse() )
	{
		m_mouseJoint = static_cast<b2MouseJoint*>( joint );
	}
	_joint->_setJoint( joint );
}
//////////////////////////////////////////////////////////////////////////
Menge::PhysicJoint2DInterface* Box2DPhysicSystem::createMouseJoint( Menge::PhysicBody2DInterface* _body, int _x, int _y )
{
	b2MouseJointDef* jointDef = new b2MouseJointDef();
	b2Body* body = static_cast<Box2DPhysicBody*>( _body )->getBody();
	jointDef->collideConnected = false;
	jointDef->body1 = m_world->GetGroundBody();
	jointDef->body2 = body;
	jointDef->maxForce = 1000.0f * body->GetMass();
	jointDef->target = b2Vec2( _x * Menge::physicsScaler, _y * Menge::physicsScaler );

	Box2DPhysicJoint* joint = new Box2DPhysicJoint( m_world, NULL, true );

	_createJoint( jointDef, joint );
	delete jointDef;

	return static_cast<Menge::PhysicJoint2DInterface*>( joint );
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicSystem::onMouseMove( int x, int y )
{
	if( m_mouseJoint )
	{
		m_mouseJoint->SetTarget( b2Vec2( x * Menge::physicsScaler, y * Menge::physicsScaler) );
	}
}
//////////////////////////////////////////////////////////////////////////