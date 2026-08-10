#include "Box2DWorld.h"

#include "Box2DBody.h"
#include "Box2DJoint.h"
#include "Box2DScaler.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/MemoryAllocator.h"
#include "Kernel/PrototypeHelper.h"

#include "Config/StdAlgorithm.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        struct Box2DOverlapResultDesc
        {
            Box2DBodyInterface ** bodies;
            uint32_t capacity;
            uint32_t found;
        };
        //////////////////////////////////////////////////////////////////////////
        static bool b2OverlapResult( b2ShapeId _shapeId, void * _context )
        {
            Box2DOverlapResultDesc * desc = (Box2DOverlapResultDesc *)_context;

            Box2DBodyInterface * body = (Box2DBodyInterface *)::b2Shape_GetUserData( _shapeId );

            for( Box2DBodyInterface ** it_body = desc->bodies, **it_body_end = desc->bodies + desc->capacity; it_body != it_body_end; ++it_body )
            {
                if( *it_body == nullptr )
                {
                    *it_body = body;

                    ++desc->found;

                    return true;
                }

                if( *it_body == body )
                {
                    return true;
                }
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DWorld::Box2DWorld()
        : m_dead( false )
        , m_time( 0.f )
        , m_worldId( b2_nullWorldId )
        , m_timeStep( 1000.f / 60.f )
        , m_subStepCount( 4 )
        , m_scaler( 1024.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DWorld::~Box2DWorld()
    {
        if( this->isValid() == true )
        {
            this->finalize();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DWorld::initialize( const mt::vec2f & _gravity, float _scaler )
    {
        m_scaler = Box2DScaler( _scaler );

        b2Vec2 b2_gravity = m_scaler.toBox2DWorld( _gravity );

        b2WorldDef worldDef = ::b2DefaultWorldDef();
        worldDef.gravity = b2_gravity;
        worldDef.enableSleep = true;

        m_worldId = ::b2CreateWorld( &worldDef );

        TIMEPIPE_SERVICE()
            ->addTimepipe( TimepipeInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DWorld::finalize()
    {
        if( this->isValid() == false )
        {
            return;
        }

        TIMEPIPE_SERVICE()
            ->removeTimepipe( TimepipeInterfacePtr::from( this ) );

        m_contactListener = nullptr;

        ::b2DestroyWorld( m_worldId );
        m_worldId = b2_nullWorldId;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DWorld::isValid() const
    {
        return B2_IS_NON_NULL( m_worldId ) && ::b2World_IsValid( m_worldId );
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
    void Box2DWorld::setTimeStep( float _timeStep, uint32_t _subStepCount )
    {
        m_timeStep = _timeStep;
        m_subStepCount = _subStepCount;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DWorld::setGravity( const mt::vec2f & _gravity )
    {
        ::b2World_SetGravity( m_worldId, m_scaler.toBox2DWorld( _gravity ) );
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f Box2DWorld::getGravity() const
    {
        return m_scaler.toEngineWorld( ::b2World_GetGravity( m_worldId ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DWorld::setContactListener( const Box2DContactListenerInterfacePtr & _listener )
    {
        m_contactListener = _listener;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Box2DWorld::overlapCircle( const mt::vec2f & _position, float _radius, uint32_t _categoryBits, uint32_t _maskBits, Box2DBodyInterface ** _bodies, uint32_t _capacity ) const
    {
        b2Vec2 b2_position = m_scaler.toBox2DWorld( _position );
        float b2_radius = m_scaler.toBox2DWorld( _radius );

        b2ShapeProxy b2_proxy = ::b2MakeProxy( &b2_position, 1, b2_radius );

        b2QueryFilter b2_queryFilter = ::b2DefaultQueryFilter();        
        b2_queryFilter.categoryBits = _categoryBits;
        b2_queryFilter.maskBits = _maskBits;

        Detail::Box2DOverlapResultDesc desc;
        desc.bodies = _bodies;
        desc.capacity = _capacity;
        desc.found = 0;

        ::b2World_OverlapShape( m_worldId, &b2_proxy, b2_queryFilter, &Detail::b2OverlapResult, &desc );

        return desc.found;
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DBodyInterfacePtr Box2DWorld::createBody( bool _static, const mt::vec2f & _position, float _angle, float _linearDamping, float _angularDamping, bool _allowSleep, bool _isBullet, bool _fixedRotation, const DocumentInterfacePtr & _doc )
    {
        EBox2DBodyType type = _static == true ? EBOX2D_BODY_STATIC : EBOX2D_BODY_DYNAMIC;

        return this->createBodyType( type, _position, _angle, _linearDamping, _angularDamping, _allowSleep, _isBullet, _fixedRotation, 1.f, _doc );
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DBodyInterfacePtr Box2DWorld::createBodyType( EBox2DBodyType _type, const mt::vec2f & _position, float _angle, float _linearDamping, float _angularDamping, bool _allowSleep, bool _isBullet, bool _fixedRotation, float _gravityScale, const DocumentInterfacePtr & _doc )
    {
        b2BodyDef bodyDef = ::b2DefaultBodyDef();

        bodyDef.position = m_scaler.toBox2DWorld( _position );
        bodyDef.rotation = ::b2MakeRot( _angle );
        bodyDef.linearDamping = _linearDamping;
        bodyDef.angularDamping = _angularDamping;
        bodyDef.enableSleep = _allowSleep;
        bodyDef.isBullet = _isBullet;
        bodyDef.fixedRotation = _fixedRotation;
        bodyDef.gravityScale = _gravityScale;

        switch( _type )
        {
        case EBOX2D_BODY_STATIC:
            bodyDef.type = b2_staticBody;
            break;
        case EBOX2D_BODY_KINEMATIC:
            bodyDef.type = b2_kinematicBody;
            break;
        case EBOX2D_BODY_DYNAMIC:
            bodyDef.type = b2_dynamicBody;
            break;
        default:
            return nullptr;
        }

        Box2DBodyPtr body = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Box2D" ), Box2DBody::getFactorableType(), _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( body, "invalid create body" );

        b2BodyId bodyId = ::b2CreateBody( m_worldId, &bodyDef );

        if( body->initialize( m_scaler, bodyId ) == false )
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
            float timeStepSeconds = m_timeStep * 0.001f;

            ::b2World_Step( m_worldId, timeStepSeconds, m_subStepCount );

            this->processEvents_();

            m_time -= m_timeStep;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static Box2DBodyInterfacePtr getEventBody( b2ShapeId _shapeId )
        {
            if( ::b2Shape_IsValid( _shapeId ) == false )
            {
                return nullptr;
            }

            Box2DBodyInterface * body = (Box2DBodyInterface *)::b2Shape_GetUserData( _shapeId );

            if( body == nullptr )
            {
                return nullptr;
            }

            return Box2DBodyInterfacePtr::from( body );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DWorld::processEvents_()
    {
        if( m_contactListener == nullptr )
        {
            return;
        }

        b2ContactEvents contactEvents = ::b2World_GetContactEvents( m_worldId );

        for( int index = 0; index != contactEvents.beginCount; ++index )
        {
            const b2ContactBeginTouchEvent & event = contactEvents.beginEvents[index];
            mt::vec2f point = mt::vec2f::identity();
            mt::vec2f normal = m_scaler.toEngineWorldNormal( event.manifold.normal );
            if( event.manifold.pointCount != 0 )
            {
                point = m_scaler.toEngineWorld( event.manifold.points[0].point );
            }
            m_contactListener->onBox2DContactEvent( EBOX2D_CONTACT_BEGIN, Detail::getEventBody( event.shapeIdA ), Detail::getEventBody( event.shapeIdB ), point, normal, 0.f );
        }

        for( int index = 0; index != contactEvents.endCount; ++index )
        {
            const b2ContactEndTouchEvent & event = contactEvents.endEvents[index];
            m_contactListener->onBox2DContactEvent( EBOX2D_CONTACT_END, Detail::getEventBody( event.shapeIdA ), Detail::getEventBody( event.shapeIdB ), mt::vec2f::identity(), mt::vec2f::identity(), 0.f );
        }

        for( int index = 0; index != contactEvents.hitCount; ++index )
        {
            const b2ContactHitEvent & event = contactEvents.hitEvents[index];
            m_contactListener->onBox2DContactEvent( EBOX2D_CONTACT_HIT, Detail::getEventBody( event.shapeIdA ), Detail::getEventBody( event.shapeIdB ), m_scaler.toEngineWorld( event.point ), m_scaler.toEngineWorldNormal( event.normal ), event.approachSpeed );
        }

        b2SensorEvents sensorEvents = ::b2World_GetSensorEvents( m_worldId );

        for( int index = 0; index != sensorEvents.beginCount; ++index )
        {
            const b2SensorBeginTouchEvent & event = sensorEvents.beginEvents[index];
            m_contactListener->onBox2DContactEvent( EBOX2D_SENSOR_BEGIN, Detail::getEventBody( event.sensorShapeId ), Detail::getEventBody( event.visitorShapeId ), mt::vec2f::identity(), mt::vec2f::identity(), 0.f );
        }

        for( int index = 0; index != sensorEvents.endCount; ++index )
        {
            const b2SensorEndTouchEvent & event = sensorEvents.endEvents[index];
            m_contactListener->onBox2DContactEvent( EBOX2D_SENSOR_END, Detail::getEventBody( event.sensorShapeId ), Detail::getEventBody( event.visitorShapeId ), mt::vec2f::identity(), mt::vec2f::identity(), 0.f );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        struct Box2DCastResultDesc
        {
            Box2DScaler scaler;
            Box2DRayCastInterfacePtr response;
            uint32_t index;
        };
        //////////////////////////////////////////////////////////////////////////
        static float box2dCastResultFcn( b2ShapeId shapeId, b2Vec2 b2_point, b2Vec2 b2_normal, float b2_fraction, void * context )
        {
            Box2DCastResultDesc * desc = (Box2DCastResultDesc *)context;

            const Box2DScaler & scaler = desc->scaler;
            const Box2DRayCastInterfacePtr & response = desc->response;

            uint32_t index = desc->index++;

            Box2DBodyInterface * body = (Box2DBodyInterface *)::b2Shape_GetUserData( shapeId );

            mt::vec2f point = scaler.toEngineWorld( b2_point );
            mt::vec2f normal = scaler.toEngineWorldNormal( b2_normal );
            float fraction = b2_fraction;

            float result = response->onBox2DRayCast( index, body, point, normal, fraction );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DWorld::rayCast( const mt::vec2f & _point1, const mt::vec2f & _point2, const Box2DRayCastInterfacePtr & _response ) const
    {
        b2Vec2 b2_point1 = m_scaler.toBox2DWorld( _point1 );
        b2Vec2 b2_point2 = m_scaler.toBox2DWorld( _point2 );
        b2Vec2 b2_translation = ::b2Sub( b2_point2, b2_point1 );

        b2RayCastInput input = {b2_point1, ::b2Sub( b2_point2, b2_point1 ), 1.0f};

        b2QueryFilter b2_queryFilter = ::b2DefaultQueryFilter();

        Detail::Box2DCastResultDesc desc;
        desc.scaler = m_scaler;
        desc.response = _response;
        desc.index = 0;

        ::b2World_CastRay( m_worldId, b2_point1, b2_translation, b2_queryFilter, &Detail::box2dCastResultFcn, &desc );
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DJointInterfacePtr Box2DWorld::createDistanceJoint( const Box2DBodyInterfacePtr & _body1, const Box2DBodyInterfacePtr & _body2
        , const mt::vec2f & _offsetBody1, const mt::vec2f & _offsetBody2
        , bool _collideBodies, const DocumentInterfacePtr & _doc )
    {
        Box2DBody * body1 = Box2DBodyPtr::ptr( _body1 );
        Box2DBody * body2 = Box2DBodyPtr::ptr( _body2 );

        b2BodyId b2_body1 = body1->getBodyId();
        b2BodyId b2_body2 = body2->getBodyId();

        b2Vec2 localAnchor1 = m_scaler.toBox2DWorld( _offsetBody1 );
        b2Vec2 localAnchor2 = m_scaler.toBox2DWorld( _offsetBody2 );
        b2Vec2 worldAnchor1 = ::b2Body_GetWorldPoint( b2_body1, localAnchor1 );
        b2Vec2 worldAnchor2 = ::b2Body_GetWorldPoint( b2_body2, localAnchor2 );
        float length = ::b2Length( ::b2Sub( worldAnchor2, worldAnchor1 ) );

        b2DistanceJointDef jointDef = ::b2DefaultDistanceJointDef();

        jointDef.bodyIdA = b2_body1;
        jointDef.bodyIdB = b2_body2;

        jointDef.localAnchorA = localAnchor1;
        jointDef.localAnchorB = localAnchor2;
        jointDef.length = length;
        jointDef.minLength = length;
        jointDef.maxLength = length;

        jointDef.collideConnected = _collideBodies;

        b2JointId jointId = ::b2CreateDistanceJoint( m_worldId, &jointDef );

        Box2DJointInterfacePtr joint = this->createJoint_( jointId, _doc );

        return joint;
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DJointInterfacePtr Box2DWorld::createHingeJoint( const Box2DBodyInterfacePtr & _body1, const Box2DBodyInterfacePtr & _body2
        , const mt::vec2f & _offsetBody1, const mt::vec2f & _limits
        , bool _collideBodies, const DocumentInterfacePtr & _doc )
    {
        Box2DBody * body1 = Box2DBodyPtr::ptr( _body1 );
        Box2DBody * body2 = Box2DBodyPtr::ptr( _body2 );

        b2BodyId b2_body1 = body1->getBodyId();
        b2BodyId b2_body2 = body2->getBodyId();

        b2Vec2 offsetBody1 = m_scaler.toBox2DWorld( _offsetBody1 );
        b2Vec2 positionBody1 = ::b2Body_GetPosition( b2_body1 );

        b2Vec2 anchor1 = ::b2Add( positionBody1, offsetBody1 );

        b2RevoluteJointDef jointDef = ::b2DefaultRevoluteJointDef();

        jointDef.bodyIdA = b2_body1;
        jointDef.bodyIdB = b2_body2;
        jointDef.localAnchorA = anchor1;
        jointDef.collideConnected = _collideBodies;
        jointDef.enableLimit = false;

        if( _limits.x - _limits.y > 0.01f )
        {
            jointDef.enableLimit = true;
            jointDef.lowerAngle = _limits.x;
            jointDef.upperAngle = _limits.y;
        }

        b2JointId jointId = ::b2CreateRevoluteJoint( m_worldId, &jointDef );

        Box2DJointInterfacePtr joint = this->createJoint_( jointId, _doc );

        return joint;
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DJointInterfacePtr Box2DWorld::createPrismaticJoint( const Box2DBodyInterfacePtr & _body1, const Box2DBodyInterfacePtr & _body2
        , const mt::vec2f & _unitsWorldAxis, bool _collideConnected
        , bool _enableLimit, const mt::vec2f & _translation
        , bool _enableMotor, float _maxMotorForce, float _motorSpeed, const DocumentInterfacePtr & _doc )
    {
        Box2DBody * body1 = Box2DBodyPtr::ptr( _body1 );
        Box2DBody * body2 = Box2DBodyPtr::ptr( _body2 );

        b2BodyId b2_body1 = body1->getBodyId();
        b2BodyId b2_body2 = body2->getBodyId();

        b2Vec2 worldAxis = m_scaler.toBox2DWorldNormal( _unitsWorldAxis );
        b2Vec2 worldAnchor = ::b2Body_GetPosition( b2_body2 );

        b2PrismaticJointDef jointDef = ::b2DefaultPrismaticJointDef();

        jointDef.bodyIdA = b2_body1;
        jointDef.bodyIdB = b2_body2;
        jointDef.localAnchorA = ::b2Body_GetLocalPoint( b2_body1, worldAnchor );
        jointDef.localAnchorB = ::b2Body_GetLocalPoint( b2_body2, worldAnchor );
        jointDef.localAxisA = ::b2Body_GetLocalVector( b2_body1, worldAxis );
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

        b2JointId jointId = ::b2CreatePrismaticJoint( m_worldId, &jointDef );

        Box2DJointInterfacePtr joint = this->createJoint_( jointId, _doc );

        return joint;
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DJointInterfacePtr Box2DWorld::createWeldJoint( const Box2DBodyInterfacePtr & _body1, const Box2DBodyInterfacePtr & _body2
        , const mt::vec2f & _offsetBody1, const mt::vec2f & _offsetBody2, bool _collideConnected, const DocumentInterfacePtr & _doc )
    {
        Box2DBody * body1 = Box2DBodyPtr::ptr( _body1 );
        Box2DBody * body2 = Box2DBodyPtr::ptr( _body2 );

        b2BodyId b2_body1 = body1->getBodyId();
        b2BodyId b2_body2 = body2->getBodyId();

        b2Vec2 offsetBody1 = m_scaler.toBox2DWorld( _offsetBody1 );
        b2Vec2 offsetBody2 = m_scaler.toBox2DWorld( _offsetBody2 );


        b2WeldJointDef jointDef = ::b2DefaultWeldJointDef();

        jointDef.bodyIdA = b2_body1;
        jointDef.bodyIdB = b2_body2;
        jointDef.localAnchorA = offsetBody1;
        jointDef.localAnchorB = offsetBody2;
        jointDef.collideConnected = _collideConnected;

        b2JointId jointId = ::b2CreateWeldJoint( m_worldId, &jointDef );

        Box2DJointInterfacePtr joint = this->createJoint_( jointId, _doc );

        return joint;
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DJointInterfacePtr Box2DWorld::createWheelJoint( const Box2DBodyInterfacePtr & _body1, const Box2DBodyInterfacePtr & _body2
        , const mt::vec2f & _offsetBody, const mt::vec2f & _localAxis, bool _collideConnected
        , bool _enableLimit, float _lowerTranslation, float _upperTranslation, float _maxMotorTorque, bool _enableMotor, float _motorSpeed, float _stiffness, float _damping, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _stiffness );

        Box2DBody * body1 = Box2DBodyPtr::ptr( _body1 );
        Box2DBody * body2 = Box2DBodyPtr::ptr( _body2 );

        b2BodyId b2_body1 = body1->getBodyId();
        b2BodyId b2_body2 = body2->getBodyId();

        b2Vec2 localAxis = m_scaler.toBox2DWorldNormal( _localAxis );
        b2Vec2 offsetBody1 = m_scaler.toBox2DWorld( _offsetBody );

        b2Vec2 anchor1 = offsetBody1;

        b2WheelJointDef jointDef = ::b2DefaultWheelJointDef();
        jointDef.bodyIdA = b2_body1;
        jointDef.bodyIdB = b2_body2;
        jointDef.localAnchorA = anchor1;
        jointDef.localAxisA = localAxis;
        jointDef.dampingRatio = _damping;
        jointDef.enableLimit = _enableLimit;
        jointDef.collideConnected = _collideConnected;
        jointDef.lowerTranslation = _lowerTranslation;
        jointDef.upperTranslation = _upperTranslation;
        jointDef.enableMotor = _enableMotor;
        jointDef.maxMotorTorque = _maxMotorTorque;
        jointDef.motorSpeed = _motorSpeed;

        b2JointId jointId = ::b2CreateWheelJoint( m_worldId, &jointDef );

        Box2DJointInterfacePtr joint = this->createJoint_( jointId, _doc );

        return joint;
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DJointInterfacePtr Box2DWorld::createRevoluteJoint( const Box2DBodyInterfacePtr & _body1, const Box2DBodyInterfacePtr & _body2
        , const mt::vec2f & _worldAnchor
        , bool _enableLimit, float _lowerAngle, float _upperAngle
        , bool _enableMotor, float _motorSpeed, float _maxMotorTorque
        , const DocumentInterfacePtr & _doc )
    {
        Box2DBody * body1 = Box2DBodyPtr::ptr( _body1 );
        Box2DBody * body2 = Box2DBodyPtr::ptr( _body2 );

        b2BodyId b2_body1 = body1->getBodyId();
        b2BodyId b2_body2 = body2->getBodyId();

        b2Vec2 b2_worldAnchor = m_scaler.toBox2DWorld( _worldAnchor );

        b2RevoluteJointDef jointDef = ::b2DefaultRevoluteJointDef();

        jointDef.bodyIdA = b2_body1;
        jointDef.bodyIdB = b2_body2;
        jointDef.localAnchorA = ::b2Body_GetLocalPoint( b2_body1, b2_worldAnchor );
        jointDef.localAnchorB = ::b2Body_GetLocalPoint( b2_body2, b2_worldAnchor );
        jointDef.enableLimit = _enableLimit;
        jointDef.lowerAngle = _lowerAngle;
        jointDef.upperAngle = _upperAngle;
        jointDef.enableMotor = _enableMotor;
        jointDef.motorSpeed = _motorSpeed;
        jointDef.maxMotorTorque = _maxMotorTorque;

        b2JointId jointId = ::b2CreateRevoluteJoint( m_worldId, &jointDef );

        Box2DJointInterfacePtr joint = this->createJoint_( jointId, _doc );

        return joint;
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DJointInterfacePtr Box2DWorld::createJoint_( b2JointId jointId, const DocumentInterfacePtr & _doc )
    {
        Box2DJointPtr join = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Box2D" ), Box2DJoint::getFactorableType(), _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( join, "invalid create joint" );

        if( join->initialize( m_scaler, jointId ) == false )
        {
            return nullptr;
        }

        return join;
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DJointInterfacePtr Box2DWorld::createMouseJoint( const Box2DBodyInterfacePtr & _groundBody, const Box2DBodyInterfacePtr & _body, const mt::vec2f & _target, float _hertz, float _dampingRatio, float _maxForce, const DocumentInterfacePtr & _doc )
    {
        Box2DBody * groundBody = Box2DBodyPtr::ptr( _groundBody );
        Box2DBody * body = Box2DBodyPtr::ptr( _body );

        b2MouseJointDef jointDef = ::b2DefaultMouseJointDef();
        jointDef.bodyIdA = groundBody->getBodyId();
        jointDef.bodyIdB = body->getBodyId();
        jointDef.target = m_scaler.toBox2DWorld( _target );
        jointDef.hertz = _hertz;
        jointDef.dampingRatio = _dampingRatio;
        jointDef.maxForce = _maxForce;

        b2JointId jointId = ::b2CreateMouseJoint( m_worldId, &jointDef );

        return this->createJoint_( jointId, _doc );
    }
    //////////////////////////////////////////////////////////////////////////
}
