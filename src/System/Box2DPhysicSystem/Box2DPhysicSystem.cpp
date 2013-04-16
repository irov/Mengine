
#	include "Box2DPhysicSystem.h"
#	include "Box2DPhysicBody.h"
#	include "Box2DPhysicJoint.h"
#	include "Box2DPhysicScaler.h"
#	include <algorithm>

#	define MAX_CONTACTS_NUM 1024

SERVICE_FACTORY( PhysicSystem2D, Menge::PhysicSystem2DInterface, Menge::Box2DPhysicSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    namespace
    {
        struct FindContact
        {
            FindContact( const b2Contact* _contact )
                : m_contact( _contact )
            {
            }

            bool operator()( const b2Contact* _contact )
            {
                bool result =
                    (m_contact->GetFixtureA() == _contact->GetFixtureA() ) 
                    && (m_contact->GetFixtureB() == _contact->GetFixtureB() );

                return result;
            }

        private:
            const b2Contact* m_contact;
        };
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DPhysicSystem::Box2DPhysicSystem()
        : m_serviceProvider(NULL)
        , m_world(NULL)
        , m_mouseJoint(NULL)
        , m_scaler(NULL)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicSystem::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * Box2DPhysicSystem::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DPhysicSystem::~Box2DPhysicSystem()
    {
        if( m_world )
        {
            this->destroyWorld();
        }

        if( m_scaler )
        {
            delete m_scaler;
        }

        m_world = NULL;
        m_scaler = NULL;
        m_ground = NULL;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DPhysicSystem::initialize()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicSystem::finalize()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicSystem::createWorld( const mt::vec2f& _upperLeft, const mt::vec2f& _lowerRight, const mt::vec2f& _gravity, bool _doSleep )
    {
        (void)_doSleep;

        if( m_world )
        {
            return;
        }
        mt::vec2f size;
        size.x = _lowerRight.x - _upperLeft.x;
        size.y = _lowerRight.y - _upperLeft.y;
        m_scaler = new Box2DPhysicScaler();
        m_scaler->setLimits(size);

        mt::vec2f gravity2f = _gravity;
        m_scaler->scaleVectorToBox2D(gravity2f);
        b2Vec2 gravity( gravity2f[0], gravity2f[1] );

        m_world = new b2World( gravity );
        m_world->SetAllowSleeping( true );
        m_world->SetContactListener( this );

        b2BodyDef bodyDef;
        m_ground = m_world->CreateBody(&bodyDef);
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

        mt::vec2f pos = _pos;
        m_scaler->scalePositionToBox2D(pos);
        bodyDef.position.Set(pos[0], pos[1]);
        bodyDef.angle = _angle;
        bodyDef.linearDamping = _linearDamping;
        bodyDef.angularDamping = _angularDamping;
        bodyDef.allowSleep = _allowSleep;
        bodyDef.bullet = _isBullet;
        bodyDef.fixedRotation = _fixedRotation;
        bodyDef.type = box2DType;
        Box2DPhysicBody* body = new Box2DPhysicBody( m_world );
        bodyDef.userData = body;

        body->initialize( bodyDef, m_scaler );
        return static_cast<Menge::PhysicBody2DInterface*>( body );
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicSystem::update( float _timing, int _velocityIterations, int _positionIterations )
    {
        (void)_timing;
        (void)_velocityIterations;
        (void)_positionIterations;

        if( m_world == NULL ) 
        {
            return;
        }

        for( TBodyVector::iterator 
            it = m_deletingBodies.begin(), 
            it_end = m_deletingBodies.end();
        it != it_end;
        ++it )
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
        int32 velocityIterations = 8;
        int32 positionIterations = 2;
        //uint32 flags = 0;
        //Box2fDebugDraw* debugDraw = new  Box2fDebugDraw();

        //flags +=  b2Draw::e_shapeBit;
        //flags +=  b2Draw::e_jointBit;
        //flags +=  b2Draw::e_aabbBit;
        //flags +=  b2Draw::e_pairBit;
        //debugDraw->SetFlags(flags);
        //flags +=  b2Draw::e_centerOfMassBit;
        //m_world->SetDebugDraw(debugDraw);
        //m_world->SetContinuousPhysics( true );
        m_world->Step( timeStep, velocityIterations , positionIterations );
        //m_world->Step( _timing, _velocityIterations , _positionIterations );
        //m_world->DrawDebugData();


#ifdef PHYSIC_DEBUG
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
            node = node->GetNext(); 
            i++;
        } 
#endif

        b2Contact* contact;
        
        for( TVectorContact::iterator 
            it = m_contacts.begin(),
            it_end = m_contacts.end(); 
        it != it_end; 
        ++it )
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
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicSystem::destroyWorld()
    {
        m_contacts.clear();

        for( TBodyVector::iterator 
            it = m_deletingBodies.begin(), 
            it_end = m_deletingBodies.end();
        it != it_end;
        ++it )
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
        (void)_contact;
        (void)_oldManifold;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicSystem::PostSolve(b2Contact* _contact, const b2ContactImpulse* _impulse)
    {
        (void)_contact;
        (void)_impulse;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicSystem::destroyJoint( Menge::PhysicJoint2DInterface* _joint )
    {
        Box2DPhysicJoint* joint = static_cast<Box2DPhysicJoint*>( _joint );

        if( joint->isMouse() == true )
        {
            m_mouseJoint = NULL;
        }

        delete joint;
    }
    /////////////////////////////////////////////////////////////////////////
    void Box2DPhysicSystem::createJoint_( b2JointDef* _jointDef, Box2DPhysicJoint* _joint )
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

        b2Vec2 offsetBody1( _offsetBody1[0], _offsetBody1[1] );
        b2Vec2 offsetBody2( _offsetBody2[0] , _offsetBody2[1] );

        m_scaler->scaleVectorToBox2D( offsetBody1 );
        m_scaler->scaleVectorToBox2D( offsetBody2 );

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
        createJoint_( jointDef, joint );
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

        b2Vec2 offsetBody1( _offsetBody1[0] ,_offsetBody1[1] );
        m_scaler->scaleVectorToBox2D( offsetBody1 );
        b2Vec2 positionBody1 = body1->GetPosition();

        b2Vec2 anchor1 =  positionBody1 + offsetBody1;
        jointDef->enableLimit = false;
        if( (_limits[0] - _limits[1]) > 0.01 )
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
        createJoint_( jointDef, joint );
        delete jointDef;
        //}

        return static_cast<Menge::PhysicJoint2DInterface*>( joint );
    }
    //////////////////////////////////////////////////////////////////////////
    Menge::PhysicJoint2DInterface * Box2DPhysicSystem::createPrismaticJoint( Menge::PhysicBody2DInterface* _body1, Menge::PhysicBody2DInterface* _body2
        ,const mt::vec2f& _unitsWorldAxis ,bool _collideConnected 
        ,bool _enableLimit, const mt::vec2f& _translation 
        ,bool _enableMotor ,float _maxMotorForce, float _motorSpeed)
    {
        b2PrismaticJointDef* jointDef = new b2PrismaticJointDef();
        b2Body* body1 = static_cast<Box2DPhysicBody*>( _body1 )->getBody();
        b2Body* body2 = static_cast<Box2DPhysicBody*>( _body2 )->getBody();

        int xScale = 1;
        if( m_world->GetGravity().y > 0 )
        {
            xScale = -1;
        }

        b2Vec2 worldAxis( _unitsWorldAxis[0] ,_unitsWorldAxis[1] );
        m_scaler->scaleVectorToBox2D( worldAxis );
        jointDef->Initialize(body1, body2, body1->GetPosition(),worldAxis ); 

        jointDef->enableLimit = _enableLimit; 
        if( jointDef->enableLimit == true )
        {
            jointDef->lowerTranslation = m_scaler->scaleToBox2D( _translation[0] );
            jointDef->upperTranslation = m_scaler->scaleToBox2D( _translation[1] );
        }

        jointDef->collideConnected = _collideConnected;

        if( _enableMotor == true)
        {
            jointDef->maxMotorForce = m_scaler->scaleToBox2D( _maxMotorForce );
            jointDef->motorSpeed = m_scaler->scaleToBox2D( _motorSpeed );
            jointDef->enableMotor = true;
        }

        Box2DPhysicJoint* joint = new Box2DPhysicJoint( m_world, NULL, false );

        createJoint_( jointDef, joint );
        delete jointDef;
        return static_cast<Menge::PhysicJoint2DInterface*>( joint );
    }
    //////////////////////////////////////////////////////////////////////////
    Menge::PhysicJoint2DInterface * Box2DPhysicSystem::createPulleyJoint( Menge::PhysicBody2DInterface* _body1, Menge::PhysicBody2DInterface* _body2 
        ,const mt::vec2f& _offsetBody1, const mt::vec2f& _offsetBody2
        ,const mt::vec2f& _offsetGroundBody1, const mt::vec2f& _offsetGroundBody2, float _ratio, bool _collideConnected  )
    {
        b2Body* body1 = static_cast<Box2DPhysicBody*>( _body1 )->getBody();
        b2Body* body2 = static_cast<Box2DPhysicBody*>( _body2 )->getBody();

        b2Vec2 centerBody1 = body1->GetWorldCenter();
        b2Vec2 centerBody2 = body2->GetWorldCenter();

        b2Vec2 offsetBody1( _offsetBody1[0], _offsetBody1[1] );
        b2Vec2 offsetBody2( _offsetBody2[0] , _offsetBody2[1] );
        b2Vec2 groundOffsetBody1( _offsetGroundBody1[0], _offsetGroundBody1[1] );
        b2Vec2 groundOffsetBody2( _offsetGroundBody2[0] , _offsetGroundBody2[1] );

        m_scaler->scaleVectorToBox2D( offsetBody1 );
        m_scaler->scaleVectorToBox2D( offsetBody2 );
        m_scaler->scaleVectorToBox2D( groundOffsetBody1 );
        m_scaler->scaleVectorToBox2D( groundOffsetBody2 );

        b2Vec2 anchor1 = centerBody1 + offsetBody1;
        b2Vec2 anchor2 = centerBody2 + offsetBody2;
        b2Vec2 groundAnchor1 = centerBody1 + groundOffsetBody1;
        b2Vec2 groundAnchor2 = centerBody2 + groundOffsetBody2;

        b2PulleyJointDef* jointDef = new b2PulleyJointDef();
        jointDef->Initialize(body1,  body2,  groundAnchor1,  groundAnchor2,  anchor1, anchor2, _ratio);
        jointDef->collideConnected = _collideConnected;

        Box2DPhysicJoint* joint = new Box2DPhysicJoint( m_world, NULL, false );

        createJoint_( jointDef, joint );
        delete jointDef;
        return static_cast<Menge::PhysicJoint2DInterface*>( joint );
    }
    //////////////////////////////////////////////////////////////////////////
    Menge::PhysicJoint2DInterface * Box2DPhysicSystem::createGearJoint( Menge::PhysicBody2DInterface * _body1, Menge::PhysicBody2DInterface * _body2
        , Menge::PhysicJoint2DInterface * _joint1, Menge::PhysicJoint2DInterface * _joint2
        , float _ratio, bool _collideConnected )
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
        jointDef->collideConnected = _collideConnected;

        Box2DPhysicJoint* joint = new Box2DPhysicJoint( m_world, NULL, false );
        createJoint_( jointDef, joint );
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

        b2Vec2 offsetBody1( _offsetBody1[0], _offsetBody1[1] );
        b2Vec2 offsetBody2( _offsetBody2[0] , _offsetBody2[1] );

        m_scaler->scaleVectorToBox2D( offsetBody1 );
        m_scaler->scaleVectorToBox2D( offsetBody2 );

        b2RopeJointDef* jointDef = new b2RopeJointDef();

        jointDef->localAnchorA = offsetBody1;
        jointDef->localAnchorB = offsetBody2;
        jointDef->collideConnected = _collideConnected;
        jointDef->maxLength = _maxlength * Menge::physicsScaler;
        jointDef->bodyA = body1;
        jointDef->bodyB = body2;

        Box2DPhysicJoint* joint = new Box2DPhysicJoint( m_world, NULL, false );
        createJoint_( jointDef, joint );
        delete jointDef;
        return static_cast<Menge::PhysicJoint2DInterface*>( joint );
    }	

    //////////////////////////////////////////////////////////////////////////
    Menge::PhysicJoint2DInterface * Box2DPhysicSystem::createWheelJoint( Menge::PhysicBody2DInterface * _body1 ,Menge::PhysicBody2DInterface * _body2
        ,const mt::vec2f & _offsetBody1 ,const mt::vec2f & _offsetBody2 
        ,const mt::vec2f & _localAxis1 ,float _frequencyHz ,float _dampingRatio ,bool _collideConnected
        ,float _maxMotorTorque ,float _motorSpeed )
    {
        (void)_offsetBody2;

        b2Body* body1 = static_cast<Box2DPhysicBody*>( _body1 )->getBody();
        b2Body* body2 = static_cast<Box2DPhysicBody*>( _body2 )->getBody();

        b2Vec2 localAxis ( _localAxis1[0] , _localAxis1[1] );
        m_scaler->scaleVectorToBox2D(localAxis);

        b2WheelJointDef * jointDef = new b2WheelJointDef();

        b2Vec2 offsetBody1( _offsetBody1[0] ,_offsetBody1[1] );
        m_scaler->scaleVectorToBox2D( offsetBody1 );
        //b2Vec2 offsetBody2( _offsetBody2[0] * Menge::physicsScaler, _offsetBody2[1] * Menge::physicsScaler);
        b2Vec2 positionBody1 = body1->GetPosition();
        //b2Vec2 positionBody2 = body2->GetPosition();
        b2Vec2 anchor1 =  offsetBody1;
        //b2Vec2 anchor2 = positionBody2 + offsetBody2;

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

        Box2DPhysicJoint* joint = new Box2DPhysicJoint( m_world, NULL, false );

        createJoint_( jointDef, joint );
        delete jointDef;
        return static_cast<Menge::PhysicJoint2DInterface*>( joint );
    }	
    //////////////////////////////////////////////////////////////////////////
    Menge::PhysicJoint2DInterface* Box2DPhysicSystem::createMouseJoint( Menge::PhysicBody2DInterface* _body, int _x, int _y )
    {
        b2MouseJointDef* jointDef = new b2MouseJointDef();

        b2Body* body = static_cast<Box2DPhysicBody*>( _body )->getBody();
        jointDef->collideConnected = false;
        jointDef->bodyA = m_ground;
        jointDef->bodyB = body;
        jointDef->maxForce = 200 * body->GetMass();

        mt::vec2f pos( (float) _x , (float) _y );
        m_scaler->scalePositionToBox2D( pos );
        jointDef->target = b2Vec2( pos[0], pos[1] );
        Box2DPhysicJoint* joint = new Box2DPhysicJoint( m_world, NULL, true );
        body->SetAwake(true);
        createJoint_( jointDef, joint );
        delete jointDef;
        return static_cast<Menge::PhysicJoint2DInterface*>( joint );
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicSystem::onMouseMove( float _x, float _y )
    {
        if( !m_mouseJoint )
        {
            return;
        }
        b2Vec2 pos( _x, _y );
        m_scaler->scalePositionToBox2D( pos );
        m_mouseJoint->SetTarget( pos );
    }
    //////////////////////////////////////////////////////////////////////////
}