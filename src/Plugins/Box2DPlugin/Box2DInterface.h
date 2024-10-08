#pragma once

#include "Interface/UnknownInterface.h"
#include "Interface/ServantInterface.h"
#include "Interface/ServiceInterface.h"
#include "Interface/EventReceiverInterface.h"
#include "Interface/DocumentInterface.h"

#include "Kernel/Polygon.h"
#include "Kernel/Eventable.h"
#include "Kernel/Node.h"

#include "math/vec2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EBox2DManifoldType
    {
        EVENT_BOX2DMANIFOLD_NONE,
        EVENT_BOX2DMANIFOLD_CIRCLES,
        EVENT_BOX2DMANIFOLD_FACE_A,
        EVENT_BOX2DMANIFOLD_FACE_B,
    };
    //////////////////////////////////////////////////////////////////////////
    class Box2DContactInterface
        : public ServantInterface
    {
    };
    //////////////////////////////////////////////////////////////////////////
    struct Box2DManifoldPoint
    {
        mt::vec2f localPoint;
        float normalImpulse;
        float tangentImpulse;
    };
    //////////////////////////////////////////////////////////////////////////
    struct Box2DManifold
    {
        Box2DManifoldPoint points[2];

        mt::vec2f localNormal;
        mt::vec2f localPoint;
    };
    //////////////////////////////////////////////////////////////////////////
    struct Box2DContactImpulse
    {
        float normalImpulses[2];
        float tangentImpulses[2];
    };
    //////////////////////////////////////////////////////////////////////////
    enum EBox2DBodyEventFlag
    {
        EVENT_BOX2DBODY_BEGIN_CONTACT = 0,
        EVENT_BOX2DBODY_END_CONTACT,
        EVENT_BOX2DBODY_PRE_SOLVE,
        EVENT_BOX2DBODY_POST_SOLVE,
        EVENT_BOX2DBODY_UPDATE_CONTACT,
    };
    //////////////////////////////////////////////////////////////////////////
    class Box2DBodyEventReceiverInterface
        : public EventReceiverInterface
    {
    public:
        virtual void onBox2DBodyBeginContact( class Box2DBody * _other, const Box2DContactInterface * _contact ) = 0;
        virtual void onBox2DBodyEndContact( class Box2DBody * _other, const Box2DContactInterface * _contact ) = 0;

        virtual void onBox2DBodyPreSolve( class Box2DBody * _other, Box2DContactInterface * _contact, EBox2DManifoldType _type, uint32_t _points, const Box2DManifold & _oldManifold ) = 0;
        virtual void onBox2DBodyPostSolve( class Box2DBody * _other, Box2DContactInterface * _contact, uint32_t _count, const Box2DContactImpulse & _impulse ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Box2DBodyEventReceiverInterface, EventReceiverInterface> Box2DBodyEventReceiverInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    EVENTATION_TYPEID( Box2DBodyEventReceiverInterface, EVENT_BOX2DBODY_BEGIN_CONTACT );
    EVENTATION_TYPEID( Box2DBodyEventReceiverInterface, EVENT_BOX2DBODY_END_CONTACT );
    EVENTATION_TYPEID( Box2DBodyEventReceiverInterface, EVENT_BOX2DBODY_PRE_SOLVE );
    EVENTATION_TYPEID( Box2DBodyEventReceiverInterface, EVENT_BOX2DBODY_POST_SOLVE );
    EVENTATION_TYPEID( Box2DBodyEventReceiverInterface, EVENT_BOX2DBODY_UPDATE_CONTACT );
    //////////////////////////////////////////////////////////////////////////
    class Box2DBodyInterface
        : public ServantInterface
    {
    public:
        virtual void setNode( const NodePtr & _node ) = 0;
        virtual const NodePtr & getNode() const = 0;

    public:
        virtual bool addShapeConvex( const Polygon & _vertices, float _density, float _friction, float _restitution, bool _isSensor, uint32_t _collisionMask, uint32_t _categoryBits, int32_t _groupIndex ) = 0;
        virtual bool addShapeCircle( float _radius, const mt::vec2f & _localPos, float _density, float _friction, float _restitution, bool _isSensor, uint32_t _collisionMask, uint32_t _categoryBits, int32_t _groupIndex ) = 0;
        virtual bool addShapeBox( float _width, float _height, const mt::vec2f & _localPos, float _angle, float _density, float _friction, float _restitution, bool _isSensor, uint32_t _collisionMask, uint32_t _categoryBits, int32_t _groupIndex ) = 0;

    public:
        virtual mt::vec2f getBodyPosition() const = 0;
        virtual float getBodyAngle() const = 0;

    public:
        virtual mt::vec2f getBodyWorldVector( const mt::vec2f & _localVector ) = 0;

    public:
        virtual float getBodyMass() const = 0;
        virtual float getBodyInertiaTensor() const = 0;

        virtual void setBodyLinearVelocity( const mt::vec2f & _velocity ) = 0;
        virtual mt::vec2f getBodyLinearVelocity() const = 0;

        virtual void setBodyAngularVelocity( float _w ) = 0;
        virtual float getBodyAngularVelocity() const = 0;

    public:
        virtual void applyForce( const mt::vec2f & _force, const mt::vec2f & _point ) = 0;
        virtual void applyImpulse( const mt::vec2f & _impulse, const mt::vec2f & _point ) = 0;
        virtual void applyAngularImpulse( float _impulse ) = 0;
        virtual void applyTorque( float _torque ) = 0;

    public:
        virtual bool isFrozen() const = 0;
        virtual bool isSleeping() const = 0;
        virtual bool isStatic() const = 0;
        virtual bool isKinematic() const = 0;
        virtual bool isDynamic() const = 0;

    public:
        virtual void setTransform( const mt::vec2f & _position, float _angle ) = 0;
        virtual void setBodyLinearDamping( float _dumping ) = 0;
        virtual float getBodyLinearDamping() const = 0;
        virtual void setBodyAngularDamping( float _dumping ) = 0;
        virtual float getBodyAngularDamping() const = 0;
        virtual void setBodyFixedRotation( bool _rotation ) = 0;
        virtual bool isBodyFixedRotation() const = 0;
        virtual void setBodyBulletMode( bool _isBullet ) = 0;
        virtual bool isBodyBulletMode() const = 0;

    public:
        virtual void sleep() = 0;
        virtual void wakeUp() = 0;

    public:
        virtual void setFilterData( uint16_t _categoryBits, uint16_t _collisionMask, int16_t _groupIndex ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Box2DBodyInterface> Box2DBodyInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class Box2DJointInterface
        : public ServantInterface
    {
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Box2DJointInterface> Box2DJointInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class Box2DRayCastInterface
        : public Mixin
    {
    public:
        virtual float onBox2DRayCast( uint32_t _index, const Box2DBodyInterface * _body, const mt::vec2f & _point, const mt::vec2f & _normal, float _fraction ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Box2DRayCastInterface> Box2DRayCastInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class UnknownPhysicalPlaceholderInterface
        : public UnknownInterface
    {
    public:
        virtual void setBox2DBody( const Box2DBodyInterfacePtr & _body ) = 0;
        virtual const Box2DBodyInterfacePtr & getBox2DBody() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UnknownPhysicalPlaceholderInterface> UnknownPhysicalPlaceholderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class Box2DWorldInterface
        : public ServantInterface
    {
    public:
        virtual void setTimeStep( float _timeStep, uint32_t _subStepCount ) = 0;

    public:
        virtual uint32_t overlapCircle( const mt::vec2f & _pos, float _radius, uint32_t _categoryBits, uint32_t _maskBits, Box2DBodyInterface ** _bodies, uint32_t _capacity ) const = 0;

    public:
        virtual Box2DBodyInterfacePtr createBody( bool _static
            , const mt::vec2f & _pos
            , float _angle
            , float _linearDamping
            , float _angularDamping
            , bool _allowSleep
            , bool _isBullet
            , bool _fixedRotation
            , const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual Box2DJointInterfacePtr createDistanceJoint( const Box2DBodyInterfacePtr & _body1
            , const Box2DBodyInterfacePtr & _body2
            , const mt::vec2f & _offsetBody1
            , const mt::vec2f & _offsetBody2
            , bool _collideBodies
            , const DocumentInterfacePtr & _doc ) = 0;

        virtual Box2DJointInterfacePtr createHingeJoint( const Box2DBodyInterfacePtr & _body1
            , const Box2DBodyInterfacePtr & _body2
            , const mt::vec2f & _offsetBody1
            , const mt::vec2f & _limits
            , bool _collideBodies
            , const DocumentInterfacePtr & _doc ) = 0;

        virtual Box2DJointInterfacePtr createPrismaticJoint( const Box2DBodyInterfacePtr & _body1
            , const Box2DBodyInterfacePtr & _body2
            , const mt::vec2f & _unitsWorldAxis
            , bool _collideConnected
            , bool _enableLimit
            , const mt::vec2f & _translation
            , bool _enableMotor
            , float _maxMotorForce
            , float _motorSpeed
            , const DocumentInterfacePtr & _doc ) = 0;

        virtual Box2DJointInterfacePtr createWeldJoint( const Box2DBodyInterfacePtr & _body1
            , const Box2DBodyInterfacePtr & _body2
            , const mt::vec2f & _offsetBody1
            , const mt::vec2f & _offsetBody2
            , bool _collideConnected
            , const DocumentInterfacePtr & _doc ) = 0;

        virtual Box2DJointInterfacePtr createWheelJoint( const Box2DBodyInterfacePtr & _body1
            , const Box2DBodyInterfacePtr & _body2
            , const mt::vec2f & _offsetBody
            , const mt::vec2f & _localAxis
            , bool _collideConnected
            , bool _enableLimit
            , float _lowerTranslation
            , float _upperTranslation
            , float _maxMotorTorque
            , bool _enableMotor
            , float _motorSpeed
            , float _stiffness
            , float _damping
            , const DocumentInterfacePtr & _doc ) = 0;

        virtual Box2DJointInterfacePtr createRevoluteJoint( const Box2DBodyInterfacePtr & _body1
            , const Box2DBodyInterfacePtr & _body2
            , const mt::vec2f & _localAnchor
            , bool _enableLimit
            , float _lowerAngle
            , float _upperAngle
            , bool _enableMotor
            , float _motorSpeed
            , float _maxMotorTorque
            , const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual void rayCast( const mt::vec2f & _point1, const mt::vec2f & _point2, const Box2DRayCastInterfacePtr & _response ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Box2DWorldInterface> Box2DWorldInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class Box2DServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "Box2DService" );

    public:
        virtual Box2DWorldInterfacePtr createWorld( const mt::vec2f & _gravity, float _scaler, const DocumentInterfacePtr & _doc ) = 0;
        virtual void destroyWorld( const Box2DWorldInterfacePtr & _world ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define BOX2D_SERVICE()\
    ((Mengine::Box2DServiceInterface *)SERVICE_GET(Mengine::Box2DServiceInterface))
//////////////////////////////////////////////////////////////////////////
