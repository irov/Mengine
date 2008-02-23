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