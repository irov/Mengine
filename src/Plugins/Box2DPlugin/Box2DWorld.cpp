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
            b2BodyId bodyId = ::b2Shape_GetBody( _shapeId );

            b2BodyType bodyType = ::b2Body_GetType( bodyId );

            if( bodyType == b2_staticBody )
            {
                return true;
            }

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
        TIMEPIPE_SERVICE()
            ->removeTimepipe( TimepipeInterfacePtr::from( this ) );

        ::b2DestroyWorld( m_worldId );
        m_worldId = b2_nullWorldId;
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
        b2BodyDef bodyDef = ::b2DefaultBodyDef();

        bodyDef.position = m_scaler.toBox2DWorld( _position );
        bodyDef.rotation = ::b2MakeRot( _angle );
        bodyDef.linearDamping = _linearDamping;
        bodyDef.angularDamping = _angularDamping;
        bodyDef.enableSleep = _allowSleep;
        bodyDef.isBullet = _isBullet;
        bodyDef.fixedRotation = _fixedRotation;
        bodyDef.type = _static == true ? b2_staticBody : b2_dynamicBody;

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

            m_time -= m_timeStep;
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

            float b2_result = scaler.toBox2DWorld( result );

            return b2_result;
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

        b2Vec2 offsetBody1 = m_scaler.toBox2DWorld( _offsetBody1 );
        b2Vec2 offsetBody2 = m_scaler.toBox2DWorld( _offsetBody2 );

        b2Vec2 positionBody1 = ::b2Body_GetPosition( b2_body1 );
        b2Vec2 positionBody2 = ::b2Body_GetPosition( b2_body2 );

        b2Vec2 anchor1 = ::b2Add( positionBody1, offsetBody1 );
        b2Vec2 anchor2 = ::b2Add( positionBody2, offsetBody2 );

        b2DistanceJointDef jointDef = ::b2DefaultDistanceJointDef();

        jointDef.bodyIdA = b2_body1;
        jointDef.bodyIdB = b2_body2;

        jointDef.localAnchorA = anchor1;
        jointDef.localAnchorB = anchor2;

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

        b2Vec2 body1_position = ::b2Body_GetPosition( b2_body1 );

        b2PrismaticJointDef jointDef = ::b2DefaultPrismaticJointDef();

        jointDef.bodyIdA = b2_body1;
        jointDef.bodyIdB = b2_body2;
        jointDef.localAnchorA = body1_position;
        jointDef.localAxisA = worldAxis;
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
        , const mt::vec2f & _localAnchor
        , bool _enableLimit, float _lowerAngle, float _upperAngle
        , bool _enableMotor, float _motorSpeed, float _maxMotorTorque
        , const DocumentInterfacePtr & _doc )
    {
        Box2DBody * body1 = Box2DBodyPtr::ptr( _body1 );
        Box2DBody * body2 = Box2DBodyPtr::ptr( _body2 );

        b2BodyId b2_body1 = body1->getBodyId();
        b2BodyId b2_body2 = body2->getBodyId();

        b2Vec2 b2_localAnchor = m_scaler.toBox2DWorld( _localAnchor );

        b2RevoluteJointDef jointDef = ::b2DefaultRevoluteJointDef();

        jointDef.bodyIdA = b2_body1;
        jointDef.bodyIdB = b2_body2;
        jointDef.localAnchorA = b2_localAnchor;
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

        if( join->initialize( jointId ) == false )
        {
            return nullptr;
        }

        return join;
    }
    //////////////////////////////////////////////////////////////////////////
}
