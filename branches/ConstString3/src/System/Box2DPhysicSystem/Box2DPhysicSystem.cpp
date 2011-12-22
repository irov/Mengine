
#	include "Box2DPhysicSystem.h"
#	include "Box2DPhysicBody.h"
#	include "Box2DPhysicJoint.h"
#	include <algorithm>
#	define MAX_CONTACTS_NUM 1024






class Box2fDebugDraw : public b2Draw
{
public:
	int GetFlags()
	{
		return b2Draw::e_shapeBit;
	}

	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
	{
		
		/*Camera2D * camera = Player::get()
			->getRenderCamera2D();

		const MaterialGroup * mg_debug = RenderEngine::get()->getMaterialGroup( CONST_STRING(SolidSprite) );

		Material debugMaterial = mg_debug->getMaterial( TAM_CLAMP, TAM_CLAMP );



		RenderEngine::get()
			->renderObject2D( &debugMaterial, NULL, NULL, 0, &(vertices[0]), vertices.size(), LPT_LINE );
		return;*/
	}
	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
	{
		return;
	}
	void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
	{
		return;
	}
	void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
	{
		return;
	}
	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
	{
		return;
	}
	void DrawTransform(const b2Transform& xf)
	{
		return;
	}
	void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color)
	{
		return;
	}
	void DrawString(int x, int y, const char* string, ...)
	{
		return;
	}

	void DrawAABB(b2AABB* aabb, const b2Color& color)
	{
		return;
	}
};






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
		destroyWorld();
	}
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicSystem::createWorld( const mt::vec2f& _upperLeft, const mt::vec2f& _lowerRight, const mt::vec2f& _gravity, bool _doSleep )
{
	if( m_world )
	{
		return;
	}

	//deprecated
	//b2AABB worldAABB;
	//worldAABB.lowerBound.Set( _upperLeft[0] * Menge::physicsScaler, _upperLeft[1] * Menge::physicsScaler );
	//worldAABB.upperBound.Set( _lowerRight[0] * Menge::physicsScaler, _lowerRight[1] * Menge::physicsScaler );
	b2Vec2 gravity( _gravity[0], _gravity[1] );
	//b2Vec2 gravity( 0 , 10 );
	m_world = new b2World( gravity );
	m_world->SetAllowSleeping( true );
	m_world->SetContactListener( this );
	
	//createGround();
	
	//b2BodyDef bd;
	//bd.position.Set(0.0f, -10.0f);
	//b2PolygonShape groundBox; 
	//groundBox.SetAsBox( 50.0f, 10.0f );
	//m_ground->CreateFixture( &groundBox, 0.0f );
}
void Box2DPhysicSystem::createGround()
{
	mt::vec2f pos (0.0f , 0.0f );
	mt::vec2f posbox (0.0f, 0.0f );
	Menge::PhysicBody2DInterface * groundBody = createBody(Menge::bodyTypeStatic, pos, 0, 0, 0, true, false, 0 );
	groundBody->addShapeBox(100.0f ,10.0f ,posbox ,0.0f , 0 ,0 ,0 ,0 ,0xFFFF ,1 ,0);
	m_ground = (static_cast<Box2DPhysicBody*> (groundBody))->getBody(); 
}

//////////////////////////////////////////////////////////////////////////
Menge::PhysicBody2DInterface* Box2DPhysicSystem::createBody(const int bodyType ,const mt::vec2f& _pos, float _angle, float _linearDamping, float _angularDamping,
	bool _allowSleep, bool _isBullet, bool _fixedRotation )
{
	if (m_world == 0)
	{
		printf("Box2DPhysicSystem World not initialized");
		return 0;
	}
	
	b2BodyType box2DType;
	
	if( bodyType == Menge::bodyTypeDynamic )
	{
		box2DType = b2_dynamicBody;
	}
	else if( bodyType == Menge::bodyTypeStatic )
	{
		box2DType = b2_staticBody;
	}
	else
	{
		printf("Box2DPhysicSystem  Bad body Type");
		return 0;
	}

	b2BodyDef bodyDef;

	bodyDef.position.Set( _pos[0] * Menge::physicsScaler, _pos[1] * Menge::physicsScaler );

	
	bodyDef.angle = _angle;
	bodyDef.linearDamping = _linearDamping;
	bodyDef.angularDamping = _angularDamping;
	bodyDef.allowSleep = _allowSleep;
	bodyDef.bullet = _isBullet;
	bodyDef.fixedRotation = _fixedRotation;
	bodyDef.type = box2DType;
	Box2DPhysicBody* body = new Box2DPhysicBody( m_world );
	bodyDef.userData = body;

	body->initialize( bodyDef );

	return static_cast<Menge::PhysicBody2DInterface*>( body );
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicSystem::update( float _timing, int _velocityIterations, int _positionIterations )
{
	if( !m_world ) return;
	
	for( TBodyVector::iterator it = m_deletingBodies.begin(), it_end = m_deletingBodies.end();
		it != it_end;
		it++ )
	{
		delete (*it);
	}
	m_deletingBodies.clear();
	/*
	static int x = 0;
	x++;
	if ( (x % 2) == 0) 
		return;
	*/
	// Prepare for simulation. Typically we use a time step of 1/60 of a
	// second (60Hz) and 10 iterations. This provides a high quality simulation
	// in most game scenarios.
	float32 timeStep = 1.0f / 60.0f;
	int32 velocityIterations = 10;
	int32 positionIterations = 8;
	//uint32 flags = 0;
	//Box2fDebugDraw* debugDraw = new  Box2fDebugDraw();
	
	//flags +=  b2Draw::e_shapeBit;
	//flags +=  b2Draw::e_jointBit;
	//flags +=  b2Draw::e_aabbBit;
	//flags +=  b2Draw::e_pairBit;
	//debugDraw->SetFlags(flags);
	//flags +=  b2Draw::e_centerOfMassBit;
	//m_world->SetDebugDraw(debugDraw);
	m_world->SetContinuousPhysics( true );
	m_world->Step( timeStep, velocityIterations , positionIterations );
	//m_world->Step( _timing, _velocityIterations , _positionIterations );
	//m_world->DrawDebugData();

	
#ifndef PHYSIC_DEBUG
	return;
#endif
	b2Body* node = m_world->GetBodyList(); 
	
	int i =0;
	while (node) 
	{ 
		
		b2Body* body = node; 
		b2Vec2 position = body->GetPosition();
		float32 angle = body->GetAngle();
		int type = body->GetType();
		if(position.y < -0.1f)
		{
			printf("Error body going to the center of the earth %i x : %4.2f y : %4.2f  angle : %4.2f\n", i, position.x, position.y, angle);
		}
		printf("body %i  x : %4.2f y : %4.2f  angle : %4.2f type %i \n", i, position.x, position.y, angle , type);
		const b2Transform& xf = node->GetTransform();
		for (b2Fixture* fixture = node->GetFixtureList(); fixture; fixture = fixture->GetNext())
		{
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
		

		
		/*
		while( fixture )
		{
			b2AABB box;
			b2Transform b2trans;
			fixture->GetShape()->ComputeAABB(&box, b2trans,0);
			printf("boundings lower x:%f y:%f  upper x:%f y:%f \n",box.lowerBound.x,box.lowerBound.y,box.upperBound.x, box.upperBound.y );
			fixture = fixture->GetNext();
		}
		*/
		node = node->GetNext(); 
		i++;
	} 
	return;
	int size = m_contacts.size();
	b2Contact* contact;
	for( TVectorContact::iterator it = m_contacts.begin(),
		it_end = m_contacts.end(); it != it_end; it++ )
	{
		contact = *it;
		b2Body* body1 = contact->GetFixtureA()->GetBody();
		b2Body* body2 = contact->GetFixtureB()->GetBody();

		Box2DPhysicBody* mBody1 = static_cast<Box2DPhysicBody*>( body1->GetUserData() );
		Box2DPhysicBody* mBody2 = static_cast<Box2DPhysicBody*>( body2->GetUserData() );
		if( !body1 || !body2 )
		{
			continue;
		}
		mBody1->_collide( body2, contact );
		mBody2->_collide( body1, contact );
	}
	m_contacts.clear();
	//m_world->m_broadPhase->Validate();


}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicSystem::destroyWorld()
{
	m_contacts.clear();

	for( TBodyVector::iterator 
		it = m_deletingBodies.begin(), 
		it_end = m_deletingBodies.end();
	it != it_end;
	it++ )
	{
		delete (*it);
	}
	
	m_deletingBodies.clear();

	delete m_world;
	m_world = 0;
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicSystem::destroyBody( Menge::PhysicBody2DInterface* _body )
{
	//delete static_cast<Box2DPhysicBody*>( _body );
	if( _body == 0 ) return;
	Box2DPhysicBody* body = static_cast<Box2DPhysicBody*>( _body );
	body->setUserData( 0 );
	body->setBodyListener( 0 );
	m_deletingBodies.push_back( body );
}
//////////////////////////////////////////////////////////////////////////
//This code may be wrong. because Box2D change collssion interface
void Box2DPhysicSystem::BeginContact(b2Contact* _contact)
{
	TVectorContact::iterator it_find 
		= std::find_if( m_contacts.begin(), m_contacts.end(), FindContact( _contact ) );
	if( it_find == m_contacts.end() )
	{
		m_contacts.push_back( _contact );
	}
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicSystem::EndContact(b2Contact* _contact)
{
	TVectorContact::iterator it_find 
		= std::find_if( m_contacts.begin(), m_contacts.end(), FindContact( _contact ) );
	if( it_find != m_contacts.end() )
	{
		m_contacts.erase( it_find );
	}
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicSystem::PreSolve(b2Contact* _contact, const b2Manifold* _oldManifold)
{

}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicSystem::PostSolve(b2Contact* _contact, const b2ContactImpulse* _impulse)
{

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
/////////////////////////////////////////////////////////////////////////
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
Menge::PhysicJoint2DInterface* Box2DPhysicSystem::createDistanceJoint( Menge::PhysicBody2DInterface* _body1, Menge::PhysicBody2DInterface* _body2,  const mt::vec2f& _offsetBody1,  const mt::vec2f& _offsetBody2, bool _collideBodies )
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
Menge::PhysicJoint2DInterface* Box2DPhysicSystem::createHingeJoint( Menge::PhysicBody2DInterface* _body1, Menge::PhysicBody2DInterface* _body2, const mt::vec2f& _offsetBody1, const mt::vec2f& _limits, bool _collideBodies )
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
Menge::PhysicJoint2DInterface * Box2DPhysicSystem::createPrismaticJoint( Menge::PhysicBody2DInterface* _body1, Menge::PhysicBody2DInterface* _body2
																		,const mt::vec2f& _worldAxis ,const mt::vec2f& _translation ,bool _enableLimit 
																		,bool _enableMotor ,float _maxMotorForce, float _motorSpeed)
{
	b2PrismaticJointDef* jointDef = new b2PrismaticJointDef();
	b2Body* body1 = static_cast<Box2DPhysicBody*>( _body1 )->getBody();
	b2Body* body2 = static_cast<Box2DPhysicBody*>( _body2 )->getBody();
			
	b2Vec2 worldAxis( _worldAxis[0], _worldAxis[1] ); 
	jointDef->Initialize(body1, body2, body1->GetWorldCenter(),worldAxis ); 
	jointDef->lowerTranslation = _translation[0]; 
	jointDef->upperTranslation = _translation[1]; 
	jointDef->enableLimit = _enableLimit; 
	
	if( _enableMotor == true)
	{
		jointDef->maxMotorForce = _maxMotorForce; 
		jointDef->motorSpeed = _motorSpeed; 
		jointDef->enableMotor = true;
	}
	
	Box2DPhysicJoint* joint = new Box2DPhysicJoint( m_world, NULL, false );
	
	_createJoint( jointDef, joint );
	delete jointDef;
	return static_cast<Menge::PhysicJoint2DInterface*>( joint );
}
//////////////////////////////////////////////////////////////////////////
Menge::PhysicJoint2DInterface * Box2DPhysicSystem::createPulleyJoint( Menge::PhysicBody2DInterface* _body1, Menge::PhysicBody2DInterface* _body2 
																	,const mt::vec2f& _offsetBody1, const mt::vec2f& _offsetBody2
																	,const mt::vec2f& _offsetGroundBody1, const mt::vec2f& _offsetGroundBody2, float _ratio )
{
	b2Body* body1 = static_cast<Box2DPhysicBody*>( _body1 )->getBody();
	b2Body* body2 = static_cast<Box2DPhysicBody*>( _body2 )->getBody();
	
	b2Vec2 centerBody1 = body1->GetWorldCenter();
	b2Vec2 centerBody2 = body2->GetWorldCenter();
	
	b2Vec2 anchor1( centerBody1.x + (_offsetBody1[0] * Menge::physicsScaler), centerBody1.y + (_offsetBody1[1] * Menge::physicsScaler) );
	b2Vec2 anchor2( centerBody2.x + (_offsetBody2[0] * Menge::physicsScaler), centerBody2.y + (_offsetBody2[1] * Menge::physicsScaler) );
	
	b2Vec2 groundAnchor1( centerBody1.x + (_offsetGroundBody1[0] * Menge::physicsScaler) , centerBody1.y + (_offsetGroundBody1[1] * Menge::physicsScaler) );
	b2Vec2 groundAnchor2( centerBody2.x + (_offsetGroundBody2[0] * Menge::physicsScaler) , centerBody2.y +  (_offsetGroundBody2[1] * Menge::physicsScaler) );
			
	b2PulleyJointDef* jointDef = new b2PulleyJointDef();
	jointDef->Initialize(body1,  body1,  groundAnchor1,  groundAnchor2,  anchor1, anchor2, _ratio);

	Box2DPhysicJoint* joint = new Box2DPhysicJoint( m_world, NULL, false );

	_createJoint( jointDef, joint );
	delete jointDef;
	return static_cast<Menge::PhysicJoint2DInterface*>( joint );
}
//////////////////////////////////////////////////////////////////////////
Menge::PhysicJoint2DInterface * Box2DPhysicSystem::createGearJoint( Menge::PhysicBody2DInterface * _body1, Menge::PhysicBody2DInterface * _body2
																	, Menge::PhysicJoint2DInterface * _joint1, Menge::PhysicJoint2DInterface * _joint2
																	, float _ratio )
{
	b2Body* body1 = static_cast<Box2DPhysicBody*>( _body1 )->getBody();
	b2Body* body2 = static_cast<Box2DPhysicBody*>( _body2 )->getBody();
	b2Joint* joint1 = static_cast<Box2DPhysicJoint*>( _joint1 )->getJoint();
	b2Joint* joint2 = static_cast<Box2DPhysicJoint*>( _joint2 )->getJoint();

	b2GearJointDef * jointDef = new b2GearJointDef(); 
	jointDef->bodyA = body1; 
	jointDef->bodyB = body2; 
	jointDef->joint1 = joint2; 
	jointDef->joint2 = joint1; 
	jointDef->ratio = _ratio;

	Box2DPhysicJoint* joint = new Box2DPhysicJoint( m_world, NULL, false );
	_createJoint( jointDef, joint );
	delete jointDef;
	return static_cast<Menge::PhysicJoint2DInterface*>( joint );
}	
//////////////////////////////////////////////////////////////////////////
Menge::PhysicJoint2DInterface * Box2DPhysicSystem::createRopeJoint( Menge::PhysicBody2DInterface * _body1, Menge::PhysicBody2DInterface * _body2
																	,const mt::vec2f & _offsetBody1 ,const mt::vec2f & _offsetBody2 ,float _maxlength
																	,bool _collideConnected )
{
	b2Body* body1 = static_cast<Box2DPhysicBody*>( _body1 )->getBody();
	b2Body* body2 = static_cast<Box2DPhysicBody*>( _body2 )->getBody();
		
	b2Vec2 offsetBody1( _offsetBody1[0] * Menge::physicsScaler, _offsetBody1[1] * Menge::physicsScaler);
	b2Vec2 offsetBody2( _offsetBody2[0] * Menge::physicsScaler, _offsetBody2[1] * Menge::physicsScaler);

	b2Vec2 positionBody1 = body1->GetPosition();
	b2Vec2 positionBody2 = body2->GetPosition();

	b2Vec2 anchor1 = positionBody1 + offsetBody1;
	b2Vec2 anchor2 = positionBody2 + offsetBody2;
	
	b2RopeJointDef* jointDef = new b2RopeJointDef();

	jointDef->localAnchorA = anchor1;
	jointDef->localAnchorB = anchor2;
	jointDef->collideConnected = _collideConnected;
	jointDef->maxLength = _maxlength;
	jointDef->bodyA = body1;
	jointDef->bodyB = body2;
	
	Box2DPhysicJoint* joint = new Box2DPhysicJoint( m_world, NULL, false );
	_createJoint( jointDef, joint );
	delete jointDef;
	return static_cast<Menge::PhysicJoint2DInterface*>( joint );
}	

//////////////////////////////////////////////////////////////////////////
Menge::PhysicJoint2DInterface * Box2DPhysicSystem::createWheelJoint( Menge::PhysicBody2DInterface * _body1 ,Menge::PhysicBody2DInterface * _body2
																	,const mt::vec2f & _offsetBody1 ,const mt::vec2f & _offsetBody2 
																	,const mt::vec2f & _localAxis1 ,float _frequencyHz ,float _dampingRatio 
																	,bool _enableMotor ,float _maxMotorTorque ,float _motorSpeed )
{
	b2Body* body1 = static_cast<Box2DPhysicBody*>( _body1 )->getBody();
	b2Body* body2 = static_cast<Box2DPhysicBody*>( _body2 )->getBody();
		 
	b2Vec2 localAxis ( _localAxis1[0] * Menge::physicsScaler, _localAxis1[1] * Menge::physicsScaler);
	
	b2WheelJointDef * jointDef = new b2WheelJointDef();
	
	b2Vec2 offsetBody1( _offsetBody1[0] * Menge::physicsScaler, _offsetBody1[1] * Menge::physicsScaler);
	//b2Vec2 offsetBody2( _offsetBody2[0] * Menge::physicsScaler, _offsetBody2[1] * Menge::physicsScaler);
	b2Vec2 positionBody1 = body1->GetPosition();
	//b2Vec2 positionBody2 = body2->GetPosition();
	b2Vec2 anchor1 = positionBody1 + offsetBody1;
	//b2Vec2 anchor2 = positionBody2 + offsetBody2;
	
	jointDef->Initialize(body1, body2, anchor1, localAxis);
	jointDef->motorSpeed = 0.0f;
	jointDef->maxMotorTorque = 20.0f;
	jointDef->frequencyHz = _frequencyHz;
	jointDef->dampingRatio = _dampingRatio;
	
	if( _enableMotor == true)
	{
		jointDef->maxMotorTorque = _maxMotorTorque; 
		jointDef->motorSpeed = _motorSpeed; 
		jointDef->enableMotor = true;
	}
	
	Box2DPhysicJoint* joint = new Box2DPhysicJoint( m_world, NULL, false );
	
	_createJoint( jointDef, joint );
	delete jointDef;
	return static_cast<Menge::PhysicJoint2DInterface*>( joint );
}	
//////////////////////////////////////////////////////////////////////////
Menge::PhysicJoint2DInterface* Box2DPhysicSystem::createMouseJoint( Menge::PhysicBody2DInterface* _body, Menge::PhysicBody2DInterface* _ground, int _x, int _y )
{
	b2MouseJointDef* jointDef = new b2MouseJointDef();
	
	b2Body* body = static_cast<Box2DPhysicBody*>( _body )->getBody();
	b2Body* ground = static_cast<Box2DPhysicBody*>( _ground )->getBody();
	jointDef->collideConnected = false;
	
	jointDef->bodyA = ground;
	jointDef->bodyB = body;
	
	jointDef->maxForce = 1000.0f * body->GetMass();
	jointDef->target = b2Vec2( _x * Menge::physicsScaler, _y * Menge::physicsScaler );
	jointDef->target = b2Vec2( 6,2 );
	Box2DPhysicJoint* joint = new Box2DPhysicJoint( m_world, NULL, true );
	
	_createJoint( jointDef, joint );
	delete jointDef;
	return static_cast<Menge::PhysicJoint2DInterface*>( joint );
}
//////////////////////////////////////////////////////////////////////////
void Box2DPhysicSystem::onMouseMove( float x, float y )
{
	if( m_mouseJoint )
	{
		//m_mouseJoint->SetTarget( b2Vec2( x * Menge::physicsScaler, (1024.0 - y) * Menge::physicsScaler) );
		//m_mouseJoint->SetTarget( b2Vec2( 10 * Menge::physicsScaler, -20 * Menge::physicsScaler) );
		//m_mouseJoint->SetTarget( b2Vec2( 0, 0) );
	}
}
//////////////////////////////////////////////////////////////////////////
