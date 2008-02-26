#	include "Box2DPhysicBody.h"

//////////////////////////////////////////////////////////////////////////
Box2DPhysicBody::Box2DPhysicBody( b2World* _world, const b2BodyDef& _bodyDef )
: m_world( _world )
{
	m_body = m_world->CreateBody( &_bodyDef );
}
//////////////////////////////////////////////////////////////////////////
Box2DPhysicBody::~Box2DPhysicBody()
{
	m_world->DestroyBody( m_body );
}
//////////////////////////////////////////////////////////////////////////
const float* Box2DPhysicBody::getPosition()
{
	return &(m_body->GetWorldPoint( b2Vec2( 0.0f, 0.0f ) ).x);
}
//////////////////////////////////////////////////////////////////////////
const float* Box2DPhysicBody::getOrientation()
{
	return &(m_body->GetRotationMatrix().col1.x);
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