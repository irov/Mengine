#	include "Box2DPhysicBody.h"

//////////////////////////////////////////////////////////////////////////
Box2DPhysicBody::Box2DPhysicBody( b2World* _world )
: m_world(_world)
, m_listener(0)
, m_body(0)
, m_boundingBox( mt::vec2f(0,0), mt::vec2f(0,0))
{
}
//////////////////////////////////////////////////////////////////////////
Box2DPhysicBody::~Box2DPhysicBody()
{
	if( m_world && m_body )
	{
		m_world->DestroyBody( m_body );
	}
}
//////////////////////////////////////////////////////////////////////////
bool Box2DPhysicBody::initialize( const b2BodyDef& _bodyDef )
{
	if( m_world == 0 )
	{
		return false;
	}

	m_body = m_world->CreateBody( &_bodyDef );
	static b2Vec2 pos(0.0f, 0.0f);
	pos = m_body->GetPosition();
	printf(  "Box2DPhysicBody::initialize  %f  %f \n",pos.x,pos.y );

	if( m_body == 0 )
	{
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
const mt::box2f& Box2DPhysicBody::getBoundingBox()
{
	_updateBoundingBox();
	return m_boundingBox;
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::_updateBoundingBox()
{
	const b2Transform& xf = m_body->GetTransform();
	b2AABB aabb;
	aabb.lowerBound = b2Vec2(1000,1000);
	aabb.upperBound = b2Vec2(-1000,-1000); 
	for (b2Fixture* fixture = m_body->GetFixtureList(); fixture; fixture = fixture->GetNext())
	{
		aabb.Combine(aabb, fixture->GetAABB(0));
		int x =1;
	}
	m_boundingBox.minimum.x = aabb.lowerBound.x * Menge::oneDivPhysicsScaler;
	m_boundingBox.minimum.y = aabb.lowerBound.y * Menge::oneDivPhysicsScaler;
	m_boundingBox.maximum.y = aabb.upperBound.x * Menge::oneDivPhysicsScaler;
	m_boundingBox.maximum.y = aabb.upperBound.y * Menge::oneDivPhysicsScaler;
#ifndef PHYSIC_DEBUG
	return;
#endif
	printf(" minimum :  (x :%4.2f , y: %4.2f) maximum (x: %4.2f ,y: %4.2f) \n" ,m_boundingBox.minimum.x , m_boundingBox.minimum.y , m_boundingBox.maximum.x , m_boundingBox.maximum.y);
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::addShapeConvex(std::size_t _pointsNum, const float* _convex,
									 float _density, float _friction, float _restitution, bool _isSensor,
									 unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex )
{
	//float area = 0.0f;
	b2PolygonShape shape;
	shape.m_vertexCount = _pointsNum;

	for( unsigned int i = 0; i != _pointsNum; ++i )
	{
		float point1 = _convex[ 2*i ] * Menge::physicsScaler;
		float point2 = _convex[ 2*i + 1 ] * Menge::physicsScaler;

		shape.m_vertices[i].Set( point1, point2 );
	}

	b2FixtureDef fd;
	
	fd.shape = &shape;
	fd.density = _density;
	fd.friction = _friction;
	fd.filter.categoryBits = _categoryBits;
	fd.filter.maskBits =_collisionMask;
	fd.restitution = _restitution;
	fd.filter.groupIndex = _groupIndex;
	fd.isSensor = _isSensor;
	
	//density as second argument?
	m_body->CreateFixture( &fd );
	
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::addShapeCircle(float _radius, const mt::vec2f& _localPos,
									 float _density, float _friction, float _restitution, bool _isSensor,
									 unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex )
{
	
	b2CircleShape shape;
	shape.m_radius =  _radius * Menge::physicsScaler;
	shape.m_p.Set( _localPos[0] * Menge::physicsScaler, _localPos[1] * Menge::physicsScaler );
	
	b2FixtureDef fd;
	fd.shape = &shape;
	fd.density = _density;
	fd.friction = _friction;
	fd.restitution = _restitution;
	fd.filter.maskBits =_collisionMask;
	fd.filter.categoryBits = _categoryBits;
	fd.filter.groupIndex = _groupIndex;
	fd.isSensor = _isSensor;
	
	m_body->CreateFixture( &fd );
	
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::addShapeBox(float _width, float _height, const mt::vec2f& _localPos, float _angle,
								  float _density, float _friction, float _restitution, bool _isSensor,
								  unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex )
{
	
	b2PolygonShape shape;
	float boxWidth = _width * 0.5f * Menge::physicsScaler;
	float boxHeight = _height * 0.5f * Menge::physicsScaler;
	shape.SetAsBox(
		boxWidth
		, boxHeight
		, b2Vec2( _localPos[0] * Menge::physicsScaler, _localPos[1] * Menge::physicsScaler )
		, _angle );

	b2FixtureDef fd;
	
	fd.shape = &shape;
	fd.density = _density;
	fd.friction = _friction;
	fd.restitution = _restitution;
	//fd.filter.maskBits =_collisionMask;
	//fd.filter.categoryBits = _categoryBits;
	//fd.filter.groupIndex = _groupIndex;
	fd.isSensor = _isSensor;
	
	m_body->CreateFixture( &fd );
	
}
//////////////////////////////////////////////////////////////////////////
mt::vec2f& Box2DPhysicBody::getPosition()
{
	static b2Vec2 pos(0.0f, 0.0f);
	pos = m_body->GetPosition();
	m_position.x = pos.x * Menge::oneDivPhysicsScaler;
	m_position.y = pos.y * Menge::oneDivPhysicsScaler;
	return m_position;
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::setPosition( float _x, float _y )
{
	b2Vec2 position( _x * Menge::physicsScaler, _y * Menge::physicsScaler );
	float angle = m_body->GetAngle();
	m_body->SetTransform( position, angle );
}
//////////////////////////////////////////////////////////////////////////
 mt::vec2f& Box2DPhysicBody::getOrientation()
{
	float angle = m_body->GetTransform().q.GetAngle();
	m_direction.x = -1*cosf(angle);
	m_direction.y = sinf(angle);
	return m_direction;
}
//////////////////////////////////////////////////////////////////////////
float Box2DPhysicBody::getMass() const
{
	return m_body->GetMass();
}
//////////////////////////////////////////////////////////////////////////
float Box2DPhysicBody::getInertia() const
{
	return m_body->GetInertia();
}
//////////////////////////////////////////////////////////////////////////
float Box2DPhysicBody::getAngle()
{
	return m_body->GetAngle();
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::setOrientation( float _angle )
{
	m_body->SetTransform( m_body->GetPosition(), _angle );
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::setLinearVelocity( float _x, float _y )
{
	b2Vec2 velocity( _x * Menge::physicsScaler, _y * Menge::physicsScaler );
	return m_body->SetLinearVelocity( velocity );
}
//////////////////////////////////////////////////////////////////////////
mt::vec2f& Box2DPhysicBody::getLinearVelocity()
{
	static b2Vec2 velocity(0.0f, 0.0f);
	velocity = m_body->GetLinearVelocity();
		
	m_velocity.x = velocity.x * Menge::oneDivPhysicsScaler;
	m_velocity.y = velocity.y * Menge::oneDivPhysicsScaler;
	return m_velocity;
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::setAngularVelocity( float _w )
{
	m_body->SetAngularVelocity( _w );
}
//////////////////////////////////////////////////////////////////////////
float Box2DPhysicBody::getAngularVelocity() const
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
	m_body->ApplyLinearImpulse( impulse, point );
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::setBodyListener( Menge::PhysicBody2DListener* _listener )
{
	m_listener = _listener;
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::_collide( b2Body* _otherBody, b2Contact* _contact )
{
	if( !m_listener ) return;

	Box2DPhysicBody* _otherObj = static_cast<Box2DPhysicBody*> ( _otherBody->GetUserData() );
	if( _otherObj->getUserData() == 0 )
	{
		return;
	}
	
	b2Vec2 contact_position = _contact->GetManifold()->localPoint;

	contact_position.x *= Menge::oneDivPhysicsScaler;
	contact_position.y *= Menge::oneDivPhysicsScaler;

	b2Vec2 normal = _contact->GetManifold()->localNormal;
	m_listener->onCollide( _otherObj, contact_position.x, contact_position.y, normal.x, normal.y );

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
void Box2DPhysicBody::setLinearDumping( float _dumping )
{
	m_body->SetLinearDamping( _dumping );
} 
//////////////////////////////////////////////////////////////////////////
float Box2DPhysicBody::getLinearDumping() const
{
	return m_body->GetLinearDamping();
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::setAngularDumping( float _dumping )
{
	m_body->SetAngularDamping( _dumping );
}
//////////////////////////////////////////////////////////////////////////
float Box2DPhysicBody::getAngularDumping() const
{
	return m_body->GetAngularDamping();
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::setFixedRotation( bool _rotation )
{
	m_body->SetFixedRotation( _rotation );
}
//////////////////////////////////////////////////////////////////////////
bool Box2DPhysicBody::getFixedRotation() const
{
	return m_body->IsFixedRotation();
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::setIsBullet( bool _isBullet )
{
	m_body->SetBullet( _isBullet );
}
//////////////////////////////////////////////////////////////////////////
bool Box2DPhysicBody::getIsBullet() const
{
	return m_body->IsBullet();
}
//////////////////////////////////////////////////////////////////////////
bool Box2DPhysicBody::isFrozen() const
{
	//return false;
	return !(m_body->IsActive());
}
//////////////////////////////////////////////////////////////////////////
bool Box2DPhysicBody::isSleeping() const
{
	return m_body->IsSleepingAllowed();
	
}
//////////////////////////////////////////////////////////////////////////
bool Box2DPhysicBody::isStatic() const
{
	return (m_body->GetType() == b2_staticBody);
}
//////////////////////////////////////////////////////////////////////////
b2Body* Box2DPhysicBody::getBody()
{
	return m_body;
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::sleep()
{
	m_body->SetSleepingAllowed(true);
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::wakeUp()
{
	m_body->SetSleepingAllowed(false);
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::applyTorque( float _torque )
{
	m_body->ApplyTorque( _torque );
}
////////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::update()
{
	if( m_listener )
	{
		m_listener->onUpdate();
	}
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::updateFilterData( Menge::uint16 _categoryBits, Menge::uint16 _collisionMask, signed short _groupIndex )
{
	b2Filter filter;
	filter.categoryBits = _categoryBits;
	filter.maskBits = _collisionMask;
	filter.groupIndex = _groupIndex;
		
	b2Fixture* fixture = m_body->GetFixtureList();
	while( fixture != 0 )
	{
		fixture->SetFilterData( filter );
		//may be not need : m_world->Refilter( shape );
		fixture = fixture->GetNext();
	}
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicBody::dump()
{
	b2Vec2 position = m_body->GetPosition();
	float32 angle = m_body->GetAngle();
	int type = m_body->GetType();
	/*if(position.y < -0.1f)
	{
		printf("Error body going to the center of the earth  x : %4.2f y : %4.2f  angle : %4.2f\n", position.x, position.y, angle);
	}*/
	printf("x : %4.2f y : %4.2f  angle : %4.2f type %i ", position.x, position.y, angle , type);
	const b2Transform& xf = m_body->GetTransform();
	for (b2Fixture* fixture = m_body->GetFixtureList(); fixture; fixture = fixture->GetNext())
	{
		printf (" Fixture type %i Shape type %i \n", fixture->GetType(), fixture->GetShape()->GetType() );
		
		b2PolygonShape* poly = (b2PolygonShape*)fixture->GetShape();
		int32 vertexCount = poly->m_vertexCount;
		b2Assert(vertexCount <= b2_maxPolygonVertices);
		b2Vec2 vertices[b2_maxPolygonVertices];

		for (int32 i = 0; i < vertexCount; ++i)
		{
			vertices[i] = b2Mul(xf, poly->m_vertices[i]);
			printf(" vertice %i x %4.2f y %4.2f \n",i,vertices[i].x, vertices[i].y );
		}
	}
	
	_updateBoundingBox();
	printf("Bounding BOX minimum :  (x :%4.2f , y: %4.2f) maximum (x: %4.2f ,y: %4.2f) \n" ,m_boundingBox.minimum.x , m_boundingBox.minimum.y , m_boundingBox.maximum.x , m_boundingBox.maximum.y);
}
//////////////////////////////////////////////////////////////////////////
