#	include "Box2DPhysicBody.h"

//////////////////////////////////////////////////////////////////////////
Box2DPhysicBody::Box2DPhysicBody( b2World* _world, const b2BodyDef& _bodyDef )
: m_world( _world )
, m_listener( NULL )
{
	m_body = m_world->CreateBody( &_bodyDef );
	m_body->m_userData = this;
}
//////////////////////////////////////////////////////////////////////////
Box2DPhysicBody::~Box2DPhysicBody()
{
	m_world->DestroyBody( m_body );
}
//////////////////////////////////////////////////////////////////////////
const float* Box2DPhysicBody::getPosition() const
{
	static b2Vec2 s_pos;
	s_pos = m_body->GetOriginPosition();
	return &s_pos.x;
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::setPosition( float _x, float _y )
{
	m_body->SetOriginPosition( b2Vec2( _x, _y ), m_body->GetRotation() );
}
//////////////////////////////////////////////////////////////////////////
const float* Box2DPhysicBody::getOrientation()
{
	return &(m_body->GetRotationMatrix().col1.x);
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::setOrientation( float _angle )
{
	m_body->SetOriginPosition( m_body->GetOriginPosition(), _angle );
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::setLinearVelocity( float _x, float _y )
{
	return m_body->SetLinearVelocity( b2Vec2( _x, _y ) );
}
//////////////////////////////////////////////////////////////////////////
const float* Box2DPhysicBody::getLinearVelocity()
{
	return &(m_body->GetLinearVelocity().x);
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::setAngularVelocity( float _w )
{
	m_body->SetAngularVelocity( _w );
}
//////////////////////////////////////////////////////////////////////////
float Box2DPhysicBody::getAngularVelocity()
{
	return m_body->GetAngularVelocity();
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::applyForce( float _forceX, float _forceY, float _pointX, float _pointY )
{
	m_body->ApplyForce( b2Vec2( _forceX, _forceY ), b2Vec2( _pointX, _pointY ) );
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::applyImpulse( float _impulseX, float _impulseY, float _pointX, float _pointY )
{
	m_body->ApplyImpulse( b2Vec2( _impulseX, _impulseY ), b2Vec2( _pointX, _pointY ) );
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::setCollisionListener( PhysicBody2DCollisionListener* _listener )
{
	m_listener = _listener;
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::_collide( b2Body* _otherBody, b2Contact* _contact )
{
	if( !m_listener ) return;

	Box2DPhysicBody* _otherObj = static_cast<Box2DPhysicBody*> ( _otherBody->m_userData );
	b2Manifold* manifolds = _contact->GetManifolds();
	for( int i = 0; i < _contact->GetManifoldCount(); i++ )
	{
		/*for( int j = 0; j < manifolds[i].pointCount; j++ )
		{
			m_listener->onCollide( _otherObj, manifolds[i].points[j].position.x, manifolds[i].points[j].position.y,
									manifolds[i].normal.x, manifolds[i].normal.y );
		}*/
		m_listener->onCollide( _otherObj, manifolds[i].points[0].position.x, manifolds[i].points[0].position.y,
			manifolds[i].normal.x, manifolds[i].normal.y );

	}
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::setUserData( void* _data )
{
	m_userData = _data;
}
//////////////////////////////////////////////////////////////////////////
void* Box2DPhysicBody::getUserData() const
{
	return m_userData;
}
//////////////////////////////////////////////////////////////////////////
bool Box2DPhysicBody::isFrozen() const
{
	return m_body->IsFrozen();
}
//////////////////////////////////////////////////////////////////////////
bool Box2DPhysicBody::isSleeping() const
{
	return m_body->IsSleeping();
}
//////////////////////////////////////////////////////////////////////////
bool Box2DPhysicBody::isStatic() const
{
	return m_body->IsStatic();
}
//////////////////////////////////////////////////////////////////////////
b2Body* Box2DPhysicBody::getBody()
{
	return m_body;
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::wakeUp()
{
	m_body->WakeUp();
}