#	include "Box2DPhysicBody.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Box2DPhysicBody::Box2DPhysicBody()
		: m_body(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Box2DPhysicBody::initialize( b2Body * _body )
	{
		m_body = _body;

		m_body->SetUserData( this );
	}
	//////////////////////////////////////////////////////////////////////////
	void Box2DPhysicBody::setTransform( const mt::vec2f & _pos, float _angle )
	{
		m_body->SetTransform( b2Vec2(_pos.x, _pos.y), _angle );
	}
	//////////////////////////////////////////////////////////////////////////
	void Box2DPhysicBody::setLinearVelocity( const mt::vec2f & _linearVelocity )
	{
		m_body->SetLinearVelocity( b2Vec2(_linearVelocity.x, _linearVelocity.y) );
	}
	//////////////////////////////////////////////////////////////////////////
	void Box2DPhysicBody::setAngularVelocity( float _angularVelocity )
	{
		m_body->SetAngularVelocity( _angularVelocity );
	}
	//////////////////////////////////////////////////////////////////////////
	void Box2DPhysicBody::setLinearDamping( float _linearDamping )
	{
		m_body->SetLinearDamping( _linearDamping );
	}
	//////////////////////////////////////////////////////////////////////////
	void Box2DPhysicBody::setAngularDamping( float _angularDamping )
	{
		m_body->SetAngularDamping( _angularDamping );
	}
	//////////////////////////////////////////////////////////////////////////
	void Box2DPhysicBody::setAllowSleep( bool _allowSleep )
	{
		m_body->SetSleepingAllowed( _allowSleep );
	}
	//////////////////////////////////////////////////////////////////////////
	void Box2DPhysicBody::setAwake( bool _awake )
	{
		m_body->SetAwake( _awake );
	}
	//////////////////////////////////////////////////////////////////////////
	void Box2DPhysicBody::setFixedRotation( bool _fixedRotation )
	{
		m_body->SetFixedRotation( _fixedRotation );
	}
	//////////////////////////////////////////////////////////////////////////
	void Box2DPhysicBody::setBullet( bool _bullet )
	{
		m_body->SetBullet( _bullet );
	}
	//////////////////////////////////////////////////////////////////////////
	void Box2DPhysicBody::setActive( bool _active )
	{
		m_body->SetActive( _active );
	}
	//////////////////////////////////////////////////////////////////////////
	void createConvex( PhysicShapeConvex * _convex, PhysicFixture * _fixture, bool isSensor )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void createCircle( PhysicShapeCircle * _convex, PhysicFixture * _fixture, bool isSensor )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void createBox( PhysicShapeBox * _convex, PhysicFixture * _fixture, bool isSensor )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void createEdge( PhysicShapeEdge * _convex, PhysicFixture * _fixture, bool isSensor )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void createChain( PhysicShapeChain * _convex, PhysicFixture * _fixture, bool isSensor )
	{

	}
}