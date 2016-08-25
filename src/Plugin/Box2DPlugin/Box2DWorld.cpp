#	include "Box2DWorld.h"
#	include "Box2DBody.h"
#	include "Box2DJoint.h"
#	include "Box2DScaler.h"

#	include "Interface/NodeInterface.h"
#	include "Interface/StringizeInterface.h"

#	include <algorithm>

//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
	Box2DWorld::Box2DWorld()
        : m_world(nullptr)
		, m_timing(0.f)
		, m_timeStep(1.f / 60.f)
		, m_velocityIterations(10)
		, m_positionIterations(10)
    {
    }
    //////////////////////////////////////////////////////////////////////////
	Box2DWorld::~Box2DWorld()
    {
        m_world = NULL;
    }
    //////////////////////////////////////////////////////////////////////////
	bool Box2DWorld::initialize( const mt::vec2f& _gravity )
    {
		b2Vec2 b2_gravity = Box2DScalerToWorld( _gravity );

		m_world = new b2World( b2_gravity );
		m_world->SetAllowSleeping( true );

		m_world->SetDestructionListener( this );
		m_world->SetContactFilter( this );
		m_world->SetContactListener( this );

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void Box2DWorld::setTimeStep( float _timeStep, uint32_t _velocityIterations, uint32_t _positionIterations )
	{
		m_timeStep = _timeStep;
		m_velocityIterations = _velocityIterations;
		m_positionIterations = _positionIterations;
	}
    //////////////////////////////////////////////////////////////////////////
	void Box2DWorld::finalize()
    {
		m_contacts.clear();

		for( TVectorBodies::iterator
			it = m_deletingBodies.begin(),
			it_end = m_deletingBodies.end();
		it != it_end;
		++it )
		{
			Box2DBody * body = *it;

			body->destroy();
		}

		m_deletingBodies.clear();

		delete m_world;
		m_world = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
	Box2DBody * Box2DWorld::createBody( bool _static, const mt::vec2f& _pos, float _angle, float _linearDamping, float _angularDamping,
        bool _allowSleep, bool _isBullet, bool _fixedRotation )
    {
        b2BodyDef bodyDef;

		bodyDef.position = Box2DScalerToWorld( _pos );
        bodyDef.angle = _angle;
        bodyDef.linearDamping = _linearDamping;
        bodyDef.angularDamping = _angularDamping;
        bodyDef.allowSleep = _allowSleep;
        bodyDef.bullet = _isBullet;
        bodyDef.fixedRotation = _fixedRotation;
		bodyDef.type = _static ? b2_staticBody : b2_dynamicBody;
        
		Box2DBody * node = NODE_SERVICE( m_serviceProvider )
			->createNodeT<Box2DBody *>( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Box2DBody" ) );

		if( node == nullptr )
		{
			return nullptr;
		}

        bodyDef.userData = node;

		b2Body * body = m_world->CreateBody( &bodyDef );

		if( body == nullptr )
		{
			return nullptr;
		}

		node->setWorld( m_world );
		node->setBody( body );

		m_bodies.push_back( node );

		return node;
    }
    //////////////////////////////////////////////////////////////////////////
	void Box2DWorld::update( float _time, float _timing )
    {
		(void)_time;

		for( TVectorBodies::iterator
            it = m_deletingBodies.begin(), 
            it_end = m_deletingBodies.end();
        it != it_end;
        ++it )
        {
            delete (*it);
        }
        m_deletingBodies.clear();

		m_timing += _timing;

		while( m_timing > m_timeStep )
		{
			m_world->Step( m_timeStep, m_velocityIterations, m_positionIterations );

			m_timing -= m_timeStep;
		}

        for( TVectorContact::iterator 
            it = m_contacts.begin(),
            it_end = m_contacts.end(); 
        it != it_end; 
        ++it )
        {
			b2Contact * contact = *it;

			b2Body * body1 = contact->GetFixtureA()->GetBody();
			b2Body * body2 = contact->GetFixtureB()->GetBody();

			if( !body1 || !body2 )
			{
				continue;
			}

            Box2DBody * mBody1 = static_cast<Box2DBody*>( body1->GetUserData() );
            Box2DBody * mBody2 = static_cast<Box2DBody*>( body2->GetUserData() );

			mBody1->onCollide( mBody2, contact );
			mBody2->onCollide( mBody1, contact );
        }
		
        m_contacts.clear();

		for( TVectorBodies::iterator
			it = m_bodies.begin(),
			it_end = m_bodies.end();
		it != it_end;
		++it )
		{
			Box2DBody * body = *it;

			body->invalidateLocalMatrix();
		}
    }
    //////////////////////////////////////////////////////////////////////////
	void Box2DWorld::destroyBody( Box2DBody * _body )
    {
		m_deletingBodies.push_back( _body );
    }
    //////////////////////////////////////////////////////////////////////////
	void Box2DWorld::SayGoodbye( b2Joint* _joint )
	{
		(void)_joint;
	}
	void Box2DWorld::SayGoodbye( b2Fixture* _fixture )
	{
		(void)_fixture;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Box2DWorld::ShouldCollide( b2Fixture* _fixtureA, b2Fixture* _fixtureB )
	{
		(void)_fixtureA;
		(void)_fixtureB;

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
	void Box2DWorld::BeginContact( b2Contact* _contact )
    {
		m_contacts.push_back( _contact );
    }
    //////////////////////////////////////////////////////////////////////////
	void Box2DWorld::EndContact( b2Contact* _contact )
    {
        TVectorContact::iterator it_find 
            = std::find( m_contacts.begin(), m_contacts.end(), _contact );

        if( it_find != m_contacts.end() )
        {
            m_contacts.erase( it_find );
        }
    }
    //////////////////////////////////////////////////////////////////////////
	void Box2DWorld::PreSolve( b2Contact* _contact, const b2Manifold* _oldManifold )
    {
        (void)_contact;
        (void)_oldManifold;
    }
    //////////////////////////////////////////////////////////////////////////
	void Box2DWorld::PostSolve( b2Contact* _contact, const b2ContactImpulse* _impulse )
    {
        (void)_contact;
        (void)_impulse;
    }
    //////////////////////////////////////////////////////////////////////////
	void Box2DWorld::destroyJoint( Box2DJoint * _joint )
    {		
		delete _joint;
    }    
    //////////////////////////////////////////////////////////////////////////
	Box2DJoint * Box2DWorld::createDistanceJoint( Box2DBody* _body1, Box2DBody * _body2, const mt::vec2f& _offsetBody1, const mt::vec2f& _offsetBody2, bool _collideBodies )
    {
        b2DistanceJointDef* jointDef = new b2DistanceJointDef();
		b2Body* body1 = _body1->getBody();
		b2Body* body2 = _body2->getBody();
        jointDef->collideConnected = _collideBodies;

        b2Vec2 offsetBody1 = Box2DScalerToWorld( _offsetBody1 );
		b2Vec2 offsetBody2 = Box2DScalerToWorld( _offsetBody2 );

        b2Vec2 positionBody1 = body1->GetPosition();
        b2Vec2 positionBody2 = body2->GetPosition();

        b2Vec2 anchor1 = positionBody1 + offsetBody1;
        b2Vec2 anchor2 = positionBody2 + offsetBody2;

        jointDef->Initialize( body1, body2, anchor1, anchor2 );

		Box2DJoint* joint = this->createJoint_( jointDef );

		return joint;
    }
    //////////////////////////////////////////////////////////////////////////
	Box2DJoint * Box2DWorld::createHingeJoint( Box2DBody* _body1, Box2DBody* _body2, const mt::vec2f& _offsetBody1, const mt::vec2f& _limits, bool _collideBodies )
    {
        b2RevoluteJointDef* jointDef = new b2RevoluteJointDef();
        b2Body* body1 = _body1->getBody();
        b2Body* body2 = _body2->getBody();
        jointDef->collideConnected = _collideBodies;

		b2Vec2 offsetBody1 = Box2DScalerToWorld( _offsetBody1 );
		b2Vec2 positionBody1 = body1->GetPosition();

        b2Vec2 anchor1 =  positionBody1 + offsetBody1;
        jointDef->enableLimit = false;

		if( _limits.x - _limits.y > 0.01 )
        {
            jointDef->enableLimit = true;
            jointDef->lowerAngle = _limits.x;
            jointDef->upperAngle = _limits.y;
        }

        jointDef->Initialize( body1, body2, anchor1 );

		Box2DJoint* joint = this->createJoint_( jointDef );

        return joint;
    }
    //////////////////////////////////////////////////////////////////////////
	Box2DJoint * Box2DWorld::createPrismaticJoint( Box2DBody* _body1, Box2DBody* _body2
        ,const mt::vec2f& _unitsWorldAxis ,bool _collideConnected 
        ,bool _enableLimit, const mt::vec2f& _translation 
        ,bool _enableMotor ,float _maxMotorForce, float _motorSpeed)
    {
        b2PrismaticJointDef* jointDef = new b2PrismaticJointDef();
        b2Body* body1 = _body1->getBody();
        b2Body* body2 = _body2->getBody();

        b2Vec2 worldAxis = Box2DScalerToWorld( _unitsWorldAxis );

		const b2Vec2 & body1_position = body1->GetPosition();
        
		jointDef->Initialize( body1, body2, body1_position, worldAxis );

        jointDef->enableLimit = _enableLimit; 
        if( jointDef->enableLimit == true )
        {
			jointDef->lowerTranslation = Box2DScalerToWorld( _translation.x );
			jointDef->upperTranslation = Box2DScalerToWorld( _translation.y );
        }

        jointDef->collideConnected = _collideConnected;

        if( _enableMotor == true)
        {
            jointDef->maxMotorForce = _maxMotorForce;
            jointDef->motorSpeed = _motorSpeed;
            jointDef->enableMotor = true;
        }

		Box2DJoint* joint = this->createJoint_( jointDef );

        return joint;
    }
    //////////////////////////////////////////////////////////////////////////
	Box2DJoint * Box2DWorld::createPulleyJoint( Box2DBody* _body1, Box2DBody* _body2
        ,const mt::vec2f& _offsetBody1, const mt::vec2f& _offsetBody2
        ,const mt::vec2f& _offsetGroundBody1, const mt::vec2f& _offsetGroundBody2, float _ratio, bool _collideConnected  )
    {
        b2Body* body1 = _body1->getBody();
        b2Body* body2 = _body2->getBody();

        b2Vec2 centerBody1 = body1->GetWorldCenter();
        b2Vec2 centerBody2 = body2->GetWorldCenter();

        b2Vec2 offsetBody1 = Box2DScalerToWorld( _offsetBody1 );
		b2Vec2 offsetBody2 = Box2DScalerToWorld( _offsetBody2 );
		b2Vec2 groundOffsetBody1 = Box2DScalerToWorld( _offsetGroundBody1 );
		b2Vec2 groundOffsetBody2 = Box2DScalerToWorld( _offsetGroundBody2 );

        b2Vec2 anchor1 = centerBody1 + offsetBody1;
        b2Vec2 anchor2 = centerBody2 + offsetBody2;
        b2Vec2 groundAnchor1 = centerBody1 + groundOffsetBody1;
        b2Vec2 groundAnchor2 = centerBody2 + groundOffsetBody2;

        b2PulleyJointDef* jointDef = new b2PulleyJointDef();
        jointDef->Initialize(body1,  body2,  groundAnchor1,  groundAnchor2,  anchor1, anchor2, _ratio);
        jointDef->collideConnected = _collideConnected;

		Box2DJoint* joint = this->createJoint_( jointDef );

        return joint;
    }
    //////////////////////////////////////////////////////////////////////////
	Box2DJoint * Box2DWorld::createGearJoint( Box2DBody * _body1, Box2DBody * _body2
        , Box2DJoint * _joint1, Box2DJoint * _joint2
        , float _ratio, bool _collideConnected )
    {
        b2Body* body1 = _body1->getBody();
        b2Body* body2 = _body2->getBody();
        b2Joint* joint1 = _joint1->getJoint();
        b2Joint* joint2 = _joint2->getJoint();

        b2GearJointDef * jointDef = new b2GearJointDef(); 
        jointDef->bodyA = body1; 
        jointDef->bodyB = body2; 
        jointDef->joint1 = joint2; 
        jointDef->joint2 = joint1; 
        jointDef->ratio = _ratio;
        jointDef->collideConnected = _collideConnected;

		Box2DJoint* joint = this->createJoint_( jointDef );

		return joint;
    }	
    //////////////////////////////////////////////////////////////////////////
	Box2DJoint * Box2DWorld::createRopeJoint( Box2DBody * _body1, Box2DBody * _body2
        ,const mt::vec2f & _offsetBody1 ,const mt::vec2f & _offsetBody2 ,float _maxlength
        ,bool _collideConnected )
    {
        b2Body* body1 = _body1->getBody();
        b2Body* body2 = _body2->getBody();

        b2Vec2 offsetBody1 = Box2DScalerToWorld( _offsetBody1 );
		b2Vec2 offsetBody2 = Box2DScalerToWorld( _offsetBody2 );

        b2RopeJointDef* jointDef = new b2RopeJointDef();

        jointDef->localAnchorA = offsetBody1;
        jointDef->localAnchorB = offsetBody2;
        jointDef->collideConnected = _collideConnected;
		jointDef->maxLength = Box2DScalerToWorld( _maxlength );
        jointDef->bodyA = body1;
        jointDef->bodyB = body2;

		Box2DJoint* joint = this->createJoint_( jointDef );

		return joint;
    }	
    //////////////////////////////////////////////////////////////////////////
	Box2DJoint * Box2DWorld::createWheelJoint( Box2DBody * _body1, Box2DBody * _body2
        ,const mt::vec2f & _offsetBody1 ,const mt::vec2f & _offsetBody2 
        ,const mt::vec2f & _localAxis1 ,float _frequencyHz ,float _dampingRatio ,bool _collideConnected
        ,float _maxMotorTorque ,float _motorSpeed )
    {
        (void)_offsetBody2;

        b2Body* body1 = _body1->getBody();
        b2Body* body2 = _body2->getBody();

		b2Vec2 localAxis = Box2DScalerToWorld( _localAxis1 );
        
        b2WheelJointDef * jointDef = new b2WheelJointDef();

        b2Vec2 offsetBody1 = Box2DScalerToWorld( _offsetBody1 );
        
		b2Vec2 positionBody1 = body1->GetPosition();
        b2Vec2 anchor1 =  offsetBody1;
        
        jointDef->Initialize(body1, body2, anchor1, localAxis);
        jointDef->frequencyHz = _frequencyHz;
        jointDef->dampingRatio = _dampingRatio;
        jointDef->collideConnected = _collideConnected;

        jointDef->enableMotor = false;
        if( _motorSpeed != 0.f)
        {
            jointDef->maxMotorTorque = _maxMotorTorque; 
            jointDef->motorSpeed = _motorSpeed; 
            jointDef->enableMotor = true;
        }

		Box2DJoint* joint = this->createJoint_( jointDef );

		return joint;
    }	
	//////////////////////////////////////////////////////////////////////////
	Box2DJoint * Box2DWorld::createJoint_( const b2JointDef * _jointDef )
	{ 
		Box2DJoint * join = new Box2DJoint;

		if( join->initialize( m_world, _jointDef ) == false )
		{
			return nullptr;
		}

		return join;
	}
}