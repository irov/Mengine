#include "Box2DBody.h"
#include "Box2DScaler.h"

#include "Kernel/EventableHelper.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Box2DBody::Box2DBody()
        : m_bodyId( b2_nullBodyId )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DBody::~Box2DBody()
    {
        if( B2_IS_NON_NULL( m_bodyId ) )
        {
            ::b2DestroyBody( m_bodyId );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DBody::initialize( const Box2DScaler & _scaler, b2BodyId _bodyId )
    {
        m_scaler = _scaler;
        m_bodyId = _bodyId;

        ::b2Body_SetUserData( m_bodyId, (Box2DBodyInterface *)this );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DBody::setNode( const NodePtr & _node )
    {
        m_node = _node;
    }
    //////////////////////////////////////////////////////////////////////////
    const NodePtr & Box2DBody::getNode() const
    {
        return m_node;
    }
    //////////////////////////////////////////////////////////////////////////
    b2BodyId Box2DBody::getBodyId() const
    {
        return m_bodyId;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DBody::addShapeConvex( const Polygon & _polygon
        , float _density
        , float _friction
        , float _restitution
        , bool _isSensor
        , uint32_t _categoryBits
        , uint32_t _collisionMask
        , int32_t _groupIndex )
    {
        uint32_t num_points = _polygon.size();

        if( num_points > b2_maxPolygonVertices )
        {
            return false;
        }

        b2Vec2 vertices[b2_maxPolygonVertices];
        for( uint32_t i = 0; i != num_points; ++i )
        {
            const mt::vec2f & v = _polygon[i];

            vertices[i] = m_scaler.toBox2DWorld( v );
        }

        b2Hull hull = ::b2ComputeHull( vertices, num_points );

        MENGINE_ASSERTION_FATAL( ::b2ValidateHull( &hull ) == true, "invalid hull" );

        b2Polygon polygon = ::b2MakePolygon( &hull, 0.f );

        b2ShapeDef shapeDef = ::b2DefaultShapeDef();

        shapeDef.userData = (Box2DBodyInterface *)this;
        shapeDef.density = _density;
        shapeDef.friction = _friction;
        shapeDef.restitution = _restitution;
        shapeDef.isSensor = _isSensor;
        shapeDef.filter.categoryBits = _categoryBits;
        shapeDef.filter.maskBits = _collisionMask;
        shapeDef.filter.groupIndex = _groupIndex;

        b2ShapeId shapeId = ::b2CreatePolygonShape( m_bodyId, &shapeDef, &polygon );

        if( B2_IS_NULL( shapeId ) )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DBody::addShapeCircle( float _radius, const mt::vec2f & _position
        , float _density
        , float _friction
        , float _restitution
        , bool _isSensor
        , uint32_t _categoryBits
        , uint32_t _collisionMask
        , int32_t _groupIndex )
    {
        float b2_radius = m_scaler.toBox2DWorld( _radius );
        b2Vec2 b2_position = m_scaler.toBox2DWorld( _position );

        b2Circle circle = {b2_position, b2_radius};

        b2ShapeDef shapeDef = ::b2DefaultShapeDef();

        shapeDef.userData = (Box2DBodyInterface *)this;
        shapeDef.density = _density;
        shapeDef.friction = _friction;
        shapeDef.restitution = _restitution;
        shapeDef.isSensor = _isSensor;
        shapeDef.filter.categoryBits = _categoryBits;
        shapeDef.filter.maskBits = _collisionMask;
        shapeDef.filter.groupIndex = _groupIndex;

        b2ShapeId shapeId = ::b2CreateCircleShape( m_bodyId, &shapeDef, &circle );

        if( B2_IS_NULL( shapeId ) )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DBody::addShapeBox( float _width, float _height, const mt::vec2f & _position, float _angle
        , float _density
        , float _friction
        , float _restitution
        , bool _isSensor
        , uint32_t _categoryBits
        , uint32_t _collisionMask
        , int32_t _groupIndex )
    {
        b2Vec2 b2_position = m_scaler.toBox2DWorld( _position );
        float b2_width = m_scaler.toBox2DWorld( _width );
        float b2_height = m_scaler.toBox2DWorld( _height );

        b2Polygon box = ::b2MakeOffsetBox( b2_width, b2_height, b2_position, _angle );

        b2ShapeDef shapeDef = ::b2DefaultShapeDef();

        shapeDef.userData = (Box2DBodyInterface *)this;
        shapeDef.density = _density;
        shapeDef.friction = _friction;
        shapeDef.restitution = _restitution;
        shapeDef.isSensor = _isSensor;
        shapeDef.filter.categoryBits = _categoryBits;
        shapeDef.filter.maskBits = _collisionMask;
        shapeDef.filter.groupIndex = _groupIndex;

        b2ShapeId shapeId = ::b2CreatePolygonShape( m_bodyId, &shapeDef, &box );

        if( B2_IS_NULL( shapeId ) )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f Box2DBody::getBodyPosition() const
    {
        b2Vec2 b2_position = ::b2Body_GetPosition( m_bodyId );

        mt::vec2f position = m_scaler.toEngineWorld( b2_position );

        return position;
    }
    //////////////////////////////////////////////////////////////////////////
    float Box2DBody::getBodyAngle() const
    {
        b2Rot b2_rot = ::b2Body_GetRotation( m_bodyId );

        float angle = ::b2Rot_GetAngle( b2_rot );

        return angle;
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f Box2DBody::getBodyWorldVector( const mt::vec2f & _localVector )
    {
        b2Vec2 b2_localVector = m_scaler.toBox2DWorld( _localVector );

        b2Vec2 b2_worldVector = b2Body_GetWorldVector( m_bodyId, b2_localVector );

        mt::vec2f worldVector = m_scaler.toEngineWorld( b2_worldVector );

        return worldVector;
    }
    //////////////////////////////////////////////////////////////////////////
    float Box2DBody::getBodyMass() const
    {
        float mass = ::b2Body_GetMass( m_bodyId );

        return mass;
    }
    //////////////////////////////////////////////////////////////////////////
    float Box2DBody::getBodyInertiaTensor() const
    {
        float inertiaTensor = ::b2Body_GetInertiaTensor( m_bodyId );

        return inertiaTensor;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DBody::setBodyLinearVelocity( const mt::vec2f & _velocity )
    {
        b2Vec2 b2_velocity = m_scaler.toBox2DWorld( _velocity );

        ::b2Body_SetLinearVelocity( m_bodyId, b2_velocity );
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f Box2DBody::getBodyLinearVelocity() const
    {
        b2Vec2 b2_linearVelocity = ::b2Body_GetLinearVelocity( m_bodyId );

        mt::vec2f linearVelocity = m_scaler.toEngineWorld( b2_linearVelocity );

        return linearVelocity;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DBody::setBodyAngularVelocity( float _angularVelocity )
    {
        ::b2Body_SetAngularVelocity( m_bodyId, _angularVelocity );
    }
    //////////////////////////////////////////////////////////////////////////
    float Box2DBody::getBodyAngularVelocity() const
    {
        float b2_angularVelocity = ::b2Body_GetAngularVelocity( m_bodyId );

        return b2_angularVelocity;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DBody::applyForce( const mt::vec2f & _force, const mt::vec2f & _point )
    {
        b2Vec2 b2_force = m_scaler.toBox2DWorld( _force );
        b2Vec2 b2_point = m_scaler.toBox2DWorld( _point );

        b2Vec2 b2_worldCenterOfMass = ::b2Body_GetWorldCenterOfMass( m_bodyId );

        b2Vec2 b2_forcePoint = ::b2Add( b2_worldCenterOfMass, b2_point );
        
        ::b2Body_ApplyForce( m_bodyId, b2_force, b2_forcePoint, true );
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DBody::applyImpulse( const mt::vec2f & _impulse, const mt::vec2f & _point )
    {
        b2Vec2 b2_impulse = m_scaler.toBox2DWorld( _impulse );
        b2Vec2 b2_point = m_scaler.toBox2DWorld( _point );

        b2Vec2 b2_worldCenterOfMass = ::b2Body_GetWorldCenterOfMass( m_bodyId );

        b2Vec2 b2_forcePoint = ::b2Add( b2_worldCenterOfMass, b2_point );

        ::b2Body_ApplyLinearImpulse( m_bodyId, b2_impulse, b2_forcePoint, true );
    }
    //////////////////////////////////////////////////////////////////////////      
    void Box2DBody::applyAngularImpulse( float _impulse )
    {
        ::b2Body_ApplyAngularImpulse( m_bodyId, _impulse, true );
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DBody::setBodyLinearDamping( float _dumping )
    {
        ::b2Body_SetLinearDamping( m_bodyId, _dumping );
    }
    //////////////////////////////////////////////////////////////////////////
    float Box2DBody::getBodyLinearDamping() const
    {
        float b2_dumping = ::b2Body_GetLinearDamping( m_bodyId );

        return b2_dumping;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DBody::setBodyAngularDamping( float _dumping )
    {
        ::b2Body_SetAngularDamping( m_bodyId, _dumping );
    }
    //////////////////////////////////////////////////////////////////////////
    float Box2DBody::getBodyAngularDamping() const
    {
        float b2_dumping = ::b2Body_GetAngularDamping( m_bodyId );

        return b2_dumping;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DBody::setBodyFixedRotation( bool _rotation )
    {
        ::b2Body_SetFixedRotation( m_bodyId, _rotation );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DBody::isBodyFixedRotation() const
    {
        bool result = ::b2Body_IsFixedRotation( m_bodyId );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DBody::setBodyBulletMode( bool _bulletMode )
    {
        ::b2Body_SetBullet( m_bodyId, _bulletMode );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DBody::isBodyBulletMode() const
    {
        bool result = ::b2Body_IsBullet( m_bodyId );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DBody::isFrozen() const
    {
        if( ::b2Body_IsEnabled( m_bodyId ) == false )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DBody::isSleeping() const
    {
        bool result = ::b2Body_IsSleepEnabled( m_bodyId );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DBody::isStatic() const
    {
        b2BodyType b2_type = ::b2Body_GetType( m_bodyId );

        if( b2_type != b2_staticBody )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DBody::isKinematic() const
    {
        b2BodyType b2_type = ::b2Body_GetType( m_bodyId );

        if( b2_type != b2_kinematicBody )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DBody::isDynamic() const
    {
        b2BodyType b2_type = ::b2Body_GetType( m_bodyId );

        if( b2_type != b2_dynamicBody )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DBody::setTransform( const mt::vec2f & _position, float _angle )
    {
        b2Vec2 b2_position = m_scaler.toBox2DWorld( _position );
        b2Rot b2_rot = ::b2MakeRot( _angle );

        ::b2Body_SetTransform( m_bodyId, b2_position, b2_rot );
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DBody::sleep()
    {
        ::b2Body_EnableSleep( m_bodyId, true );
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DBody::wakeUp()
    {
        ::b2Body_SetAwake( m_bodyId, true );
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DBody::applyTorque( float _torque )
    {
        ::b2Body_ApplyTorque( m_bodyId, _torque, true );
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DBody::setFilterData( uint16_t _categoryBits, uint16_t _collisionMask, int16_t _groupIndex )
    {
        b2Filter filter = ::b2DefaultFilter();

        filter.categoryBits = _categoryBits;
        filter.maskBits = _collisionMask;
        filter.groupIndex = _groupIndex;

        b2ShapeId shapes[64];
        int shapeCount = ::b2Body_GetShapes( m_bodyId, shapes, 64 );

        for( int i = 0; i != shapeCount; ++i )
        {
            b2ShapeId shapeId = shapes[i];

            ::b2Shape_SetFilter( shapeId, filter );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
