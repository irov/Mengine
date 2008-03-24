
#	include "Box2DPhysicSystem.h"
#	include "Box2DPhysicBody.h"

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
	worldAABB.minVertex.Set( _upperLeft[0], _upperLeft[1] );
	worldAABB.maxVertex.Set( _lowerRight[0], _lowerRight[1] );
	b2Vec2 gravity( _gravity[0], _gravity[1] );

	m_world = new b2World( worldAABB, gravity, _doSleep );
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
	m_world->Step( _timing, _iterations );
	//m_world->Step( 0.0f, 1 );
	m_world->m_broadPhase->Validate();

	for( b2Contact* c = m_world->GetContactList(); c; c = c->GetNext() )
	{
		for( int i = 0; i < c->GetManifoldCount(); i++ )
		{
			static_cast<Box2DPhysicBody*>( c->GetShape1()->GetBody()->m_userData )->_collide( c->GetShape2()->GetBody(), c );
			static_cast<Box2DPhysicBody*>( c->GetShape2()->GetBody()->m_userData )->_collide( c->GetShape1()->GetBody(), c );
		}
	}
}
//////////////////////////////////////////////////////////////////////////
PhysicBody2DInterface* Box2DPhysicSystem::createPhysicBodyBox( float _width, float _heigth, 
															  float _posX, float _posY, float _angle,
															  float _density, float _friction, float _restitution,
															  float _shapeX, float _shapeY, float _shapeAngle,
															  unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex )
{
	b2BoxDef shape;
	shape.extents.Set( _width / 2.0f , _heigth / 2.0f );
	shape.density = _density;
	shape.friction = _friction;
	shape.restitution = _restitution;
	shape.localPosition.Set( _shapeX, _shapeY );
	shape.localRotation = _shapeAngle;
	shape.maskBits = _collisionMask;
	shape.categoryBits = _categoryBits;
	shape.groupIndex = _groupIndex;
	
	b2BodyDef bodyDef;
	bodyDef.AddShape( &shape );
	bodyDef.position.Set( _posX, _posY );
	bodyDef.rotation = _angle;
	
	return static_cast<PhysicBody2DInterface*>( new Box2DPhysicBody( m_world, bodyDef ) );
}
//////////////////////////////////////////////////////////////////////////
PhysicBody2DInterface* Box2DPhysicSystem::createPhysicBodyCircle( float _radius, 
																 float _posX, float _posY, float _angle,
																 float _density, float _friction, float _restitution,
																 float _shapeX, float _shapeY,
																 unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex )
{
	b2CircleDef shape;
	shape.radius = _radius;
	shape.density = _density;
	shape.friction = _friction;
	shape.restitution = _restitution;
	shape.localPosition.Set( _shapeX, _shapeY );
	shape.maskBits = _collisionMask;
	shape.categoryBits = _categoryBits;
	shape.groupIndex = _groupIndex;
	
	b2BodyDef bodyDef;
	bodyDef.AddShape( &shape );
	bodyDef.position.Set( _posX, _posY );
	bodyDef.rotation = _angle;

	return static_cast<PhysicBody2DInterface*>( new Box2DPhysicBody( m_world, bodyDef ) );
}
//////////////////////////////////////////////////////////////////////////
PhysicBody2DInterface* Box2DPhysicSystem::createPhysicBodyConvex( int _pointsNum, const float* _convex,
																	float _posX, float _posY, float _angle,
																	float _density, float _friction, float _restitution,
																	float _shapeX, float _shapeY, float _shapeAngle,
																	unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex )
{
	b2PolyDef shape;
	shape.vertexCount = _pointsNum;
	for( int i = 0; i < _pointsNum; i++ )
	{
		shape.vertices[i].Set( _convex[ 2*i ], _convex[ 2*i + 1 ] );
	}
	shape.density = _density;
	shape.friction = _friction;
	shape.restitution = _restitution;
	shape.localPosition.Set( _shapeX, _shapeY );
	shape.localRotation = _shapeAngle;
	shape.maskBits = _collisionMask;
	shape.categoryBits = _categoryBits;
	shape.groupIndex = _groupIndex;

	b2BodyDef bodyDef;
	bodyDef.AddShape( &shape );
	bodyDef.position.Set( _posX, _posY );
	bodyDef.rotation = _angle;
	//bodyDef.allowSleep = false;
	bodyDef.linearDamping = 0.02f;

	return static_cast<PhysicBody2DInterface*>( new Box2DPhysicBody( m_world, bodyDef ) );
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicSystem::destroyBody( PhysicBody2DInterface* _body )
{
	delete static_cast<Box2DPhysicBody*>( _body );
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicSystem::createDistanceJoint( PhysicBody2DInterface* _body1, PhysicBody2DInterface* _body2, const float* _offsetBody1, const float* _offsetBody2, bool _collideBodies )
{
	b2DistanceJointDef jointDef;
	jointDef.body1 = static_cast<Box2DPhysicBody*>( _body1 )->getBody();
	jointDef.body2 = static_cast<Box2DPhysicBody*>( _body2 )->getBody();
	jointDef.collideConnected = _collideBodies;
	b2Vec2 anchor1 = jointDef.body1->GetCenterPosition() + b2Vec2( _offsetBody1[0], _offsetBody1[1] );
	b2Vec2 anchor2 = jointDef.body2->GetCenterPosition() + b2Vec2( _offsetBody2[0], _offsetBody2[1] );
	jointDef.anchorPoint1 = anchor1;
	jointDef.anchorPoint2 = anchor2;

	m_world->CreateJoint( &jointDef );
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicSystem::createHingeJoint( PhysicBody2DInterface* _body1, PhysicBody2DInterface* _body2, const float* _offsetBody1, bool _collideBodies )
{
	b2RevoluteJointDef jointDef;
	jointDef.body1 = static_cast<Box2DPhysicBody*>( _body1 )->getBody();
	jointDef.body2 = static_cast<Box2DPhysicBody*>( _body2 )->getBody();
	jointDef.collideConnected = _collideBodies;
	b2Vec2 anchor1 = jointDef.body1->GetCenterPosition() + b2Vec2( _offsetBody1[0], _offsetBody1[1] );
	jointDef.anchorPoint = anchor1;
	
	m_world->CreateJoint( &jointDef );
}
//////////////////////////////////////////////////////////////////////////