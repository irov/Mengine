#	include "Box2DPhysicBody.h"

//////////////////////////////////////////////////////////////////////////
Box2DPhysicBody::Box2DPhysicBody( b2World* _world, bool _static )
: m_world( _world )
, m_listener( 0 )
, m_body( 0 )
, m_isStatic( _static )
{
}
//////////////////////////////////////////////////////////////////////////
Box2DPhysicBody::~Box2DPhysicBody()
{
	m_listener = 0;
	m_world->DestroyBody( m_body );
}
//////////////////////////////////////////////////////////////////////////
bool Box2DPhysicBody::initialize( const b2BodyDef& _bodyDef )
{
	/*if( m_isStatic )
	{
		m_body = m_world->CreateStaticBody( &_bodyDef );
	}
	else
	{
		m_body = m_world->CreateDynamicBody( &_bodyDef );
	}*/
	m_body = m_world->CreateBody( &_bodyDef );

	if( m_body == 0 )
	{
		return false;
	}

	m_body->m_listener = this;

	return true;
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::addShapeConvex(unsigned int _pointsNum, const float* _convex,
									 float _density, float _friction, float _restitution, bool _isSensor,
									 unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex )
{
	//float area = 0.0f;
	b2PolygonDef shape;
	shape.vertexCount = _pointsNum;

	for( unsigned int i = 0; i != _pointsNum; ++i )
	{
		float point1 = _convex[ 2*i ] * Menge::physicsScaler;
		float point2 = _convex[ 2*i + 1 ] * Menge::physicsScaler;

		shape.vertices[i].Set( point1, point2 );
	}

	/*for( int i = 0; i< _pointsNum; i++ )
	{
		// calc area
		// Triangle vertices.
		b2Vec2 p1(0.0f, 0.0f);
		b2Vec2 p2 = shape.vertices[i];
		b2Vec2 p3 = i + 1 < _pointsNum ? shape.vertices[i+1] : shape.vertices[0];

		b2Vec2 e1 = p2 - p1;
		b2Vec2 e2 = p3 - p1;

		float32 D = b2Cross(e1, e2);

		float32 triangleArea = 0.5f * D;
		area += triangleArea;

	}*/
	shape.density = _density;
	shape.friction = _friction;
	shape.restitution = _restitution;
	shape.filter.maskBits = _collisionMask;
	shape.filter.categoryBits = _categoryBits;
	shape.filter.groupIndex = _groupIndex;
	shape.isSensor = _isSensor;

	m_body->CreateShape( &shape );
	m_body->SetMassFromShapes();

}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::addShapeCircle(float _radius, const float* _localPos,
									 float _density, float _friction, float _restitution, bool _isSensor,
									 unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex )
{
	b2CircleDef shape;
	shape.radius = _radius * Menge::physicsScaler;
	shape.density = _density;
	shape.friction = _friction;
	shape.restitution = _restitution;
	shape.localPosition.Set( _localPos[0] * Menge::physicsScaler, _localPos[1] * Menge::physicsScaler );
	shape.filter.maskBits = _collisionMask;
	shape.filter.categoryBits = _categoryBits;
	shape.filter.groupIndex = _groupIndex;
	shape.isSensor = _isSensor;

	m_body->CreateShape( &shape );
	m_body->SetMassFromShapes();
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::addShapeBox(float _width, float _height, const float* _localPos, float _angle,
								  float _density, float _friction, float _restitution, bool _isSensor,
								  unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex )
{
	b2PolygonDef shape;
	shape.SetAsBox(
		_width * 0.5f * Menge::physicsScaler
		, _height * 0.5f * Menge::physicsScaler
		, b2Vec2( _localPos[0] * Menge::physicsScaler, _localPos[1] * Menge::physicsScaler )
		, _angle );

	shape.density = _density;
	shape.friction = _friction;
	shape.restitution = _restitution;
	shape.filter.maskBits = _collisionMask;
	shape.filter.categoryBits = _categoryBits;
	shape.filter.groupIndex = _groupIndex;
	shape.isSensor = _isSensor;

	m_body->CreateShape( &shape );
	m_body->SetMassFromShapes();
}
//////////////////////////////////////////////////////////////////////////
const float* Box2DPhysicBody::getPosition() const
{
	static b2Vec2 pos(0.0f, 0.0f);
	pos = m_body->GetPosition();

	pos.x /= Menge::physicsScaler;
	pos.y /= Menge::physicsScaler;

	return &pos.x;
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::setPosition( float _x, float _y )
{
	b2Vec2 position( _x * Menge::physicsScaler, _y * Menge::physicsScaler );
	float angle = m_body->GetAngle();
	m_body->SetXForm( position, angle );
}
//////////////////////////////////////////////////////////////////////////
const float* Box2DPhysicBody::getOrientation()
{
	return &(m_body->GetXForm().R.col1.x);
}
//////////////////////////////////////////////////////////////////////////
float Box2DPhysicBody::getMass() const
{
	return m_body->GetMass();
}
//////////////////////////////////////////////////////////////////////////
float Box2DPhysicBody::getAngle()
{
	return m_body->GetAngle();
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::setOrientation( float _angle )
{
	m_body->SetXForm( m_body->GetPosition(), _angle );
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::setLinearVelocity( float _x, float _y )
{
	b2Vec2 velocity( _x * Menge::physicsScaler, _y * Menge::physicsScaler );
	return m_body->SetLinearVelocity( velocity );
}
//////////////////////////////////////////////////////////////////////////
const float* Box2DPhysicBody::getLinearVelocity()
{
	static b2Vec2 velocity(0.0f, 0.0f);
	velocity = m_body->GetLinearVelocity();

	velocity.x /= Menge::physicsScaler;
	velocity.y /= Menge::physicsScaler;
	
	return &(velocity.x);
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
	b2Vec2 force( _forceX * Menge::physicsScaler, _forceY * Menge::physicsScaler );
	b2Vec2 point( _pointX * Menge::physicsScaler, _pointY * Menge::physicsScaler );

	m_body->ApplyForce( force, point );
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::applyImpulse( float _impulseX, float _impulseY, float _pointX, float _pointY )
{
	b2Vec2 impulse( _impulseX, _impulseY );
	b2Vec2 point( _pointX, _pointY );

	m_body->ApplyImpulse( impulse, point );
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::setBodyListener( Menge::PhysicBody2DListener* _listener )
{
	m_listener = _listener;
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::_collide( b2Body* _otherBody, b2ContactPoint* _contact )
{
	if( !m_listener ) return;

	Box2DPhysicBody* _otherObj = static_cast<Box2DPhysicBody*> ( _otherBody->GetUserData() );
	if( _otherObj->getUserData() == 0 )
	{
		return;
	}
	
	b2Vec2 contact_position = _contact->position;

	contact_position.x /= Menge::physicsScaler;
	contact_position.y /= Menge::physicsScaler;


	m_listener->onCollide( _otherObj, contact_position.x, contact_position.y, _contact->normal.x, _contact->normal.y );

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
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::applyTorque( float _torque )
{
	m_body->ApplyTorque( _torque );
}
////////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::applyForceAndTorque()
{
	if( m_listener )
	{
		m_listener->applyForceAndTorque();
	}
}