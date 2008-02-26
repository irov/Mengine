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
	return &(m_body->GetOriginPosition().x);
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
void Box2DPhysicBody::setAngularVelocity( float w )
{
	m_body->SetAngularVelocity( w );
}
//////////////////////////////////////////////////////////////////////////
float Box2DPhysicBody::getAngularVelocity()
{
	return m_body->GetAngularVelocity();
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::applyForce( float forceX, float forceY, float pointX, float pointY )
{
	m_body->ApplyForce( b2Vec2( forceX, forceY ), b2Vec2( pointX, pointY ) );
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
		m_listener->onCollide( _otherObj, manifolds[i].points[0].position.x, manifolds[i].points[0].position.y,
								manifolds[i].normal.x, manifolds[i].normal.y );
	}
}
