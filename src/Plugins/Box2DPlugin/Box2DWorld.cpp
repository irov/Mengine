#include "Box2DWorld.h"

#include "Interface/PrototypeServiceInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/MemoryAllocator.h"
#include "Kernel/DocumentHelper.h"

#include "Box2DBody.h"
#include "Box2DJoint.h"
#include "Box2DScaler.h"

#include "Config/Algorithm.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Box2DWorld::Box2DWorld()
        : m_timepipeId( 0 )
        , m_dead( false )
        , m_time( 0.f )
        , m_world( nullptr )
        , m_timeStep( 1000.f / 60.f )
        , m_velocityIterations( 10 )
        , m_positionIterations( 10 )
        , m_scaler( 128.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DWorld::~Box2DWorld()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DWorld::initialize( const mt::vec2f & _gravity, float _scaler )
    {
        m_scaler = Box2DScaler( _scaler );

        b2Vec2 b2_gravity = m_scaler.toBox2DWorld( _gravity );

        m_world = Helper::newT<b2World>( b2_gravity );

        m_world->SetAllowSleeping( true );
        m_world->SetDestructionListener( this );
        m_world->SetContactFilter( this );
        m_world->SetContactListener( this );

        uint32_t timepipeId = TIMEPIPE_SERVICE()
            ->addTimepipe( TimepipeInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE );

        if( timepipeId == 0 )
        {
            return false;
        }

        m_timepipeId = timepipeId;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DWorld::finalize()
    {
        if( m_timepipeId != 0 )
        {
            TIMEPIPE_SERVICE()
                ->removeTimepipe( m_timepipeId );

            m_timepipeId = 0;
        }

        Helper::deleteT( m_world );
        m_world = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DWorld::setDead()
    {
        m_dead = true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DWorld::isDead() const
    {
        return m_dead;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DWorld::setTimeStep( float _timeStep, uint32_t _velocityIterations, uint32_t _positionIterations )
    {
        m_timeStep = _timeStep;
        m_velocityIterations = _velocityIterations;
        m_positionIterations = _positionIterations;
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DBodyInterfacePtr Box2DWorld::createBody( bool _static, const mt::vec2f & _pos, float _angle, float _linearDamping, float _angularDamping, bool _allowSleep, bool _isBullet, bool _fixedRotation, const DocumentPtr & _doc )
    {
        b2BodyDef bodyDef;

        bodyDef.position = m_scaler.toBox2DWorld( _pos );
        bodyDef.angle = _angle;
        bodyDef.linearDamping = _linearDamping;
        bodyDef.angularDamping = _angularDamping;
        bodyDef.allowSleep = _allowSleep;
        bodyDef.bullet = _isBullet;
        bodyDef.fixedRotation = _fixedRotation;
        bodyDef.type = _static ? b2_staticBody : b2_dynamicBody;

        Box2DBodyPtr body = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Box2D" ), STRINGIZE_STRING_LOCAL( "Box2DBody" ), _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( body );

        if( body->initialize( m_scaler, m_world, &bodyDef ) == false )
        {
            return nullptr;
        }

        return body;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DWorld::onTimepipe( const UpdateContext * _contet )
    {
        m_time += _contet->time;

        while( m_time >= m_timeStep )
        {
            m_world->Step( m_timeStep * 0.001f, m_velocityIterations, m_positionIterations );

            m_time -= m_timeStep;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        class MyRayCastCallback
            : public b2RayCastCallback
        {
        public:
            MyRayCastCallback( const Box2DScaler & _scaler, const Box2DRayCastInterfacePtr & _response )
                : m_scaler( _scaler )
                , m_response( _response )
                , m_index( 0 )
            {
            }

            ~MyRayCastCallback() override
            {
            }

        protected:
            float ReportFixture( b2Fixture * fixture, const b2Vec2 & b2_point, const b2Vec2 & b2_normal, float _fraction ) override
            {
                const b2Body * b2_body = fixture->GetBody();

                const Box2DBody * body = static_cast<const Box2DBody *>(b2_body->GetUserData());

                mt::vec2f contact_point = m_scaler.toEngineWorld( b2_point );
                mt::vec2f contact_normal = m_scaler.toEngineWorldNormal( b2_normal );
                float fraction = _fraction;

                float result = m_response->onResponse( m_index, body, contact_point, contact_normal, fraction );

                ++m_index;

                float b2_result = m_scaler.toBox2DWorld( result );

                return b2_result;
            }

        protected:
            Box2DScaler m_scaler;

            Box2DRayCastInterfacePtr m_response;

            uint32_t m_index;
        };
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DWorld::rayCast( const mt::vec2f & _point1, const mt::vec2f & _point2, const Box2DRayCastInterfacePtr & _response ) const
    {
        b2Vec2 b2_point1 = m_scaler.toBox2DWorld( _point1 );
        b2Vec2 b2_point2 = m_scaler.toBox2DWorld( _point2 );

        Detail::MyRayCastCallback rayCast_cb( m_scaler, _response );
        m_world->RayCast( &rayCast_cb, b2_point1, b2_point2 );
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DWorld::SayGoodbye( b2Joint * _joint )
    {
        MENGINE_UNUSED( _joint );
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DWorld::SayGoodbye( b2Fixture * _fixture )
    {
        MENGINE_UNUSED( _fixture );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DWorld::ShouldCollide( b2Fixture * _fixtureA, b2Fixture * _fixtureB )
    {
        MENGINE_UNUSED( _fixtureA );
        MENGINE_UNUSED( _fixtureB );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DWorld::BeginContact( b2Contact * _contact )
    {
        b2Body * body1 = _contact->GetFixtureA()->GetBody();
        b2Body * body2 = _contact->GetFixtureB()->GetBody();

        if( !body1 || !body2 )
        {
            return;
        }

        Box2DBody * mBody1 = static_cast<Box2DBody *>(body1->GetUserData());
        Box2DBody * mBody2 = static_cast<Box2DBody *>(body2->GetUserData());

        mBody1->onBeginContact( mBody2, _contact );
        mBody2->onBeginContact( mBody1, _contact );
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DWorld::EndContact( b2Contact * _contact )
    {
        b2Body * body1 = _contact->GetFixtureA()->GetBody();
        b2Body * body2 = _contact->GetFixtureB()->GetBody();

        if( !body1 || !body2 )
        {
            return;
        }

        Box2DBody * mBody1 = static_cast<Box2DBody *>(body1->GetUserData());
        Box2DBody * mBody2 = static_cast<Box2DBody *>(body2->GetUserData());

        mBody1->onEndContact( mBody2, _contact );
        mBody2->onEndContact( mBody1, _contact );
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DWorld::PreSolve( b2Contact * _contact, const b2Manifold * _oldManifold )
    {
        b2Body * body1 = _contact->GetFixtureA()->GetBody();
        b2Body * body2 = _contact->GetFixtureB()->GetBody();

        if( !body1 || !body2 )
        {
            return;
        }

        Box2DBody * mBody1 = static_cast<Box2DBody *>(body1->GetUserData());
        Box2DBody * mBody2 = static_cast<Box2DBody *>(body2->GetUserData());

        mBody1->onPreSolve( mBody2, _contact, _oldManifold );
        mBody2->onPreSolve( mBody1, _contact, _oldManifold );
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DWorld::PostSolve( b2Contact * _contact, const b2ContactImpulse * _impulse )
    {
        b2Body * body1 = _contact->GetFixtureA()->GetBody();
        b2Body * body2 = _contact->GetFixtureB()->GetBody();

        if( !body1 || !body2 )
        {
            return;
        }

        Box2DBody * mBody1 = static_cast<Box2DBody *>(body1->GetUserData());
        Box2DBody * mBody2 = static_cast<Box2DBody *>(body2->GetUserData());

        mBody1->onPostSolve( mBody2, _contact, _impulse );
        mBody2->onPostSolve( mBody1, _contact, _impulse );
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DJointInterfacePtr Box2DWorld::createDistanceJoint( const Box2DBodyInterfacePtr & _body1, const Box2DBodyInterfacePtr & _body2
        , const mt::vec2f & _offsetBody1, const mt::vec2f & _offsetBody2
        , bool _collideBodies, const DocumentPtr & _doc )
    {
        Box2DBody * body1 = Box2DBodyPtr::ptr( _body1 );
        Box2DBody * body2 = Box2DBodyPtr::ptr( _body2 );

        b2Body * b2_body1 = body1->getBody();
        b2Body * b2_body2 = body2->getBody();

        b2DistanceJointDef jointDef;
        jointDef.collideConnected = _collideBodies;

        b2Vec2 offsetBody1 = m_scaler.toBox2DWorld( _offsetBody1 );
        b2Vec2 offsetBody2 = m_scaler.toBox2DWorld( _offsetBody2 );

        b2Vec2 positionBody1 = b2_body1->GetPosition();
        b2Vec2 positionBody2 = b2_body2->GetPosition();

        b2Vec2 anchor1 = positionBody1 + offsetBody1;
        b2Vec2 anchor2 = positionBody2 + offsetBody2;

        jointDef.Initialize( b2_body1, b2_body2, anchor1, anchor2 );

        Box2DJointInterfacePtr joint = this->createJoint_( &jointDef, _doc );

        return joint;
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DJointInterfacePtr Box2DWorld::createHingeJoint( const Box2DBodyInterfacePtr & _body1, const Box2DBodyInterfacePtr & _body2
        , const mt::vec2f & _offsetBody1, const mt::vec2f & _limits
        , bool _collideBodies, const DocumentPtr & _doc )
    {
        Box2DBody * body1 = Box2DBodyPtr::ptr( _body1 );
        Box2DBody * body2 = Box2DBodyPtr::ptr( _body2 );

        b2Body * b2_body1 = body1->getBody();
        b2Body * b2_body2 = body2->getBody();

        b2RevoluteJointDef jointDef;
        jointDef.collideConnected = _collideBodies;

        b2Vec2 offsetBody1 = m_scaler.toBox2DWorld( _offsetBody1 );
        b2Vec2 positionBody1 = b2_body1->GetPosition();

        b2Vec2 anchor1 = positionBody1 + offsetBody1;
        jointDef.enableLimit = false;

        if( _limits.x - _limits.y > 0.01f )
        {
            jointDef.enableLimit = true;
            jointDef.lowerAngle = _limits.x;
            jointDef.upperAngle = _limits.y;
        }

        jointDef.Initialize( b2_body1, b2_body2, anchor1 );

        Box2DJointInterfacePtr joint = this->createJoint_( &jointDef, _doc );

        return joint;
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DJointInterfacePtr Box2DWorld::createPrismaticJoint( const Box2DBodyInterfacePtr & _body1, const Box2DBodyInterfacePtr & _body2
        , const mt::vec2f & _unitsWorldAxis, bool _collideConnected
        , bool _enableLimit, const mt::vec2f & _translation
        , bool _enableMotor, float _maxMotorForce, float _motorSpeed, const DocumentPtr & _doc )
    {
        Box2DBody * body1 = Box2DBodyPtr::ptr( _body1 );
        Box2DBody * body2 = Box2DBodyPtr::ptr( _body2 );

        b2Body * b2_body1 = body1->getBody();
        b2Body * b2_body2 = body2->getBody();

        b2PrismaticJointDef jointDef;

        b2Vec2 worldAxis = m_scaler.toBox2DWorldNormal( _unitsWorldAxis );

        const b2Vec2 & body1_position = b2_body1->GetPosition();

        jointDef.Initialize( b2_body1, b2_body2, body1_position, worldAxis );

        jointDef.enableLimit = _enableLimit;

        if( jointDef.enableLimit == true )
        {
            jointDef.lowerTranslation = m_scaler.toBox2DWorld( _translation.x );
            jointDef.upperTranslation = m_scaler.toBox2DWorld( _translation.y );
        }

        jointDef.collideConnected = _collideConnected;

        if( _enableMotor == true )
        {
            jointDef.maxMotorForce = _maxMotorForce;
            jointDef.motorSpeed = _motorSpeed;
            jointDef.enableMotor = true;
        }

        Box2DJointInterfacePtr joint = this->createJoint_( &jointDef, _doc );

        return joint;
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DJointInterfacePtr Box2DWorld::createPulleyJoint( const Box2DBodyInterfacePtr & _body1, const Box2DBodyInterfacePtr & _body2
        , const mt::vec2f & _offsetBody1, const mt::vec2f & _offsetBody2
        , const mt::vec2f & _offsetGroundBody1, const mt::vec2f & _offsetGroundBody2
        , float _ratio, bool _collideConnected, const DocumentPtr & _doc )
    {
        Box2DBody * body1 = Box2DBodyPtr::ptr( _body1 );
        Box2DBody * body2 = Box2DBodyPtr::ptr( _body2 );

        b2Body * b2_body1 = body1->getBody();
        b2Body * b2_body2 = body2->getBody();

        b2Vec2 centerBody1 = b2_body1->GetWorldCenter();
        b2Vec2 centerBody2 = b2_body2->GetWorldCenter();

        b2Vec2 offsetBody1 = m_scaler.toBox2DWorld( _offsetBody1 );
        b2Vec2 offsetBody2 = m_scaler.toBox2DWorld( _offsetBody2 );
        b2Vec2 groundOffsetBody1 = m_scaler.toBox2DWorld( _offsetGroundBody1 );
        b2Vec2 groundOffsetBody2 = m_scaler.toBox2DWorld( _offsetGroundBody2 );

        b2Vec2 anchor1 = centerBody1 + offsetBody1;
        b2Vec2 anchor2 = centerBody2 + offsetBody2;
        b2Vec2 groundAnchor1 = centerBody1 + groundOffsetBody1;
        b2Vec2 groundAnchor2 = centerBody2 + groundOffsetBody2;

        b2PulleyJointDef jointDef;
        jointDef.Initialize( b2_body1, b2_body2, groundAnchor1, groundAnchor2, anchor1, anchor2, _ratio );
        jointDef.collideConnected = _collideConnected;

        Box2DJointInterfacePtr joint = this->createJoint_( &jointDef, _doc );

        return joint;
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DJointInterfacePtr Box2DWorld::createGearJoint( const Box2DBodyInterfacePtr & _body1, const Box2DBodyInterfacePtr & _body2
        , const Box2DJointInterfacePtr & _joint1, const Box2DJointInterfacePtr & _joint2
        , float _ratio, bool _collideConnected, const DocumentPtr & _doc )
    {
        Box2DBody * body1 = Box2DBodyPtr::ptr( _body1 );
        Box2DBody * body2 = Box2DBodyPtr::ptr( _body2 );

        b2Body * b2_body1 = body1->getBody();
        b2Body * b2_body2 = body2->getBody();

        Box2DJoint * joint1 = Box2DJointPtr::ptr( _joint1 );
        Box2DJoint * joint2 = Box2DJointPtr::ptr( _joint2 );

        b2Joint * b2_joint1 = joint1->getJoint();
        b2Joint * b2_joint2 = joint2->getJoint();

        b2GearJointDef jointDef;
        jointDef.bodyA = b2_body1;
        jointDef.bodyB = b2_body2;
        jointDef.joint1 = b2_joint1;
        jointDef.joint2 = b2_joint2;
        jointDef.ratio = _ratio;
        jointDef.collideConnected = _collideConnected;

        Box2DJointInterfacePtr joint = this->createJoint_( &jointDef, _doc );

        return joint;
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DJointInterfacePtr Box2DWorld::createRopeJoint( const Box2DBodyInterfacePtr & _body1, const Box2DBodyInterfacePtr & _body2
        , const mt::vec2f & _offsetBody1, const mt::vec2f & _offsetBody2, float _maxlength
        , bool _collideConnected, const DocumentPtr & _doc )
    {
        Box2DBody * body1 = Box2DBodyPtr::ptr( _body1 );
        Box2DBody * body2 = Box2DBodyPtr::ptr( _body2 );

        b2Body * b2_body1 = body1->getBody();
        b2Body * b2_body2 = body2->getBody();

        b2Vec2 offsetBody1 = m_scaler.toBox2DWorld( _offsetBody1 );
        b2Vec2 offsetBody2 = m_scaler.toBox2DWorld( _offsetBody2 );

        b2RopeJointDef jointDef;

        jointDef.localAnchorA = offsetBody1;
        jointDef.localAnchorB = offsetBody2;
        jointDef.collideConnected = _collideConnected;
        jointDef.maxLength = m_scaler.toBox2DWorld( _maxlength );
        jointDef.bodyA = b2_body1;
        jointDef.bodyB = b2_body2;

        Box2DJointInterfacePtr joint = this->createJoint_( &jointDef, _doc );

        return joint;
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DJointInterfacePtr Box2DWorld::createWheelJoint( const Box2DBodyInterfacePtr & _body1, const Box2DBodyInterfacePtr & _body2
        , const mt::vec2f & _offsetBody, const mt::vec2f & _localAxis, bool _collideConnected
        , bool _enableLimit, float _lowerTranslation, float _upperTranslation, float _maxMotorTorque, bool _enableMotor, float _motorSpeed, float _stiffness, float _damping, const DocumentPtr & _doc )
    {
        Box2DBody * body1 = Box2DBodyPtr::ptr( _body1 );
        Box2DBody * body2 = Box2DBodyPtr::ptr( _body2 );

        b2Body * b2_body1 = body1->getBody();
        b2Body * b2_body2 = body2->getBody();

        b2Vec2 localAxis = m_scaler.toBox2DWorldNormal( _localAxis );
        b2Vec2 offsetBody1 = m_scaler.toBox2DWorld( _offsetBody );

        //b2Vec2 positionBody1 = b2_body1->GetPosition();
        b2Vec2 anchor1 = offsetBody1;

        b2WheelJointDef jointDef;

        jointDef.Initialize( b2_body1, b2_body2, anchor1, localAxis );
        jointDef.enableLimit = _enableLimit;
        jointDef.collideConnected = _collideConnected;
        jointDef.lowerTranslation = _lowerTranslation;
        jointDef.upperTranslation = _upperTranslation;

        jointDef.enableMotor = _enableMotor;
        jointDef.maxMotorTorque = _maxMotorTorque;
        jointDef.motorSpeed = _motorSpeed;
        jointDef.stiffness = _stiffness;
        jointDef.damping = _damping;

        Box2DJointInterfacePtr joint = this->createJoint_( &jointDef, _doc );

        return joint;
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DJointInterfacePtr Box2DWorld::createRevoluteJoint( const Box2DBodyInterfacePtr & _body1, const Box2DBodyInterfacePtr & _body2
        , const mt::vec2f & _localAnchor
        , bool _enableLimit, float _lowerAngle, float _upperAngle
        , bool _enableMotor, float _motorSpeed, float _maxMotorTorque
        , const DocumentPtr & _doc )
    {
        Box2DBody * body1 = Box2DBodyPtr::ptr( _body1 );
        Box2DBody * body2 = Box2DBodyPtr::ptr( _body2 );

        b2Body * b2_body1 = body1->getBody();
        b2Body * b2_body2 = body2->getBody();

        b2Vec2 b2_localAnchor = m_scaler.toBox2DWorld( _localAnchor );

        b2RevoluteJointDef jointDef;

        jointDef.Initialize( b2_body1, b2_body2, b2_localAnchor );

        jointDef.enableLimit = _enableLimit;
        jointDef.lowerAngle = _lowerAngle;
        jointDef.upperAngle = _upperAngle;
        jointDef.enableMotor = _enableMotor;
        jointDef.motorSpeed = _motorSpeed;
        jointDef.maxMotorTorque = _maxMotorTorque;

        Box2DJointInterfacePtr joint = this->createJoint_( &jointDef, _doc );

        return joint;
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DJointInterfacePtr Box2DWorld::createJoint_( const b2JointDef * _jointDef, const DocumentPtr & _doc )
    {
        Box2DJointPtr join = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Box2D" ), STRINGIZE_STRING_LOCAL( "Box2DJoint" ), _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( join );

        if( join->initialize( m_world, _jointDef ) == false )
        {
            return nullptr;
        }

        return join;
    }
    //////////////////////////////////////////////////////////////////////////
}
