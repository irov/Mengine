#pragma once

#include "Interface/UnknownInterface.h"
#include "Interface/ServantInterface.h"
#include "Interface/ServiceInterface.h"
#include "Interface/EventReceiverInterface.h"

#include "Kernel/Document.h"
#include "Kernel/Polygon.h"

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
    enum Box2DBodyEventFlag
    {
        EVENT_BOX2DBODY_BEGIN_CONTACT = 0,
        EVENT_BOX2DBODY_END_CONTACT,
        EVENT_BOX2DBODY_PRE_SOLVE,
        EVENT_BOX2DBODY_POST_SOLVE,
        EVENT_BOX2DBODY_UPDATE_CONTACT,

        __EVENT_BOX2DBODY_LAST__
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
    typedef IntrusivePtr<Box2DBodyEventReceiverInterface> Box2DBodyEventReceiverInterfacePtr;
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
        virtual bool addShapeConvex( const Polygon & _vertices, float _density, float _friction, float _restitution, bool _isSensor, uint16_t _collisionMask, uint16_t _categoryBits, uint16_t _groupIndex ) = 0;
        virtual bool addShapeCircle( float _radius, const mt::vec2f & _localPos, float _density, float _friction, float _restitution, bool _isSensor, uint16_t _collisionMask, uint16_t _categoryBits, uint16_t _groupIndex ) = 0;
        virtual bool addShapeBox( float _width, float _height, const mt::vec2f & _localPos, float _angle, float _density, float _friction, float _restitution, bool _isSensor, uint16_t _collisionMask, uint16_t _categoryBits, uint16_t _groupIndex ) = 0;

    public:
        virtual mt::vec2f getPosition() const = 0;
        virtual float getAngle() const = 0;

    public:
        virtual mt::vec2f getWorldVector( const mt::vec2f & _localVector ) = 0;     

    public:
        virtual float getMass() const = 0;
        virtual float getInertia() const = 0;

        virtual void setLinearVelocity( const mt::vec2f & _velocity ) = 0;
        virtual mt::vec2f getLinearVelocity() const = 0;

        virtual void setAngularVelocity( float _w ) = 0;
        virtual float getAngularVelocity() const = 0;

    public:
        virtual void applyForce( const mt::vec2f & _force, const mt::vec2f & _point ) = 0;
        virtual void applyImpulse( const mt::vec2f & _impulse, const mt::vec2f & _point ) = 0;
        virtual void applyAngularImpulse( float _impulse ) = 0;
        virtual void applyTorque( float _torque ) = 0;

    public:
        virtual bool isFrozen() const = 0;
        virtual bool isSleeping() const = 0;
        virtual bool isStatic() const = 0;

    public:
        virtual void setTransform( const mt::vec2f & _position, float _angle ) = 0;
        virtual void setLinearDumping( float _dumping ) = 0;
        virtual float getLinearDumping() const = 0;
        virtual void setAngularDumping( float _dumping ) = 0;
        virtual float getAngularDumping() const = 0;
        virtual void setFixedRotation( bool _rotation ) = 0;
        virtual bool getFixedRotation() const = 0;
        virtual void setIsBullet( bool _isBullet ) = 0;
        virtual bool getIsBullet() const = 0;

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
        virtual float onResponse( uint32_t _index, const Box2DBodyInterface * _body, const mt::vec2f & _point, const mt::vec2f & _normal, float _fraction ) = 0;
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
        virtual void setTimeStep( float _timeStep, uint32_t _velocityIterations, uint32_t _positionIterations ) = 0;

    public:
        virtual Box2DBodyInterfacePtr createBody( bool _static
            , const mt::vec2f & _pos
            , float _angle
            , float _linearDamping
            , float _angularDamping
            , bool _allowSleep
            , bool _isBullet
            , bool _fixedRotation
            , const DocumentPtr & _doc ) = 0;

    public:
        virtual Box2DJointInterfacePtr createDistanceJoint( const Box2DBodyInterfacePtr & _body1
            , const Box2DBodyInterfacePtr & _body2
            , const mt::vec2f & _offsetBody1
            , const mt::vec2f & _offsetBody2
            , bool _collideBodies
            , const DocumentPtr & _doc ) = 0;

        virtual Box2DJointInterfacePtr createHingeJoint( const Box2DBodyInterfacePtr & _body1
            , const Box2DBodyInterfacePtr & _body2
            , const mt::vec2f & _offsetBody1
            , const mt::vec2f & _limits
            , bool _collideBodies
            , const DocumentPtr & _doc ) = 0;

        virtual Box2DJointInterfacePtr createPrismaticJoint( const Box2DBodyInterfacePtr & _body1
            , const Box2DBodyInterfacePtr & _body2
            , const mt::vec2f & _unitsWorldAxis
            , bool _collideConnected
            , bool _enableLimit
            , const mt::vec2f & _translation
            , bool _enableMotor
            , float _maxMotorForce
            , float _motorSpeed
            , const DocumentPtr & _doc ) = 0;

        virtual Box2DJointInterfacePtr createPulleyJoint( const Box2DBodyInterfacePtr & _body1
            , const Box2DBodyInterfacePtr & _body2
            , const mt::vec2f & _offsetBody1
            , const mt::vec2f & _offsetBody2
            , const mt::vec2f & _offsetGroundBody1
            , const mt::vec2f & _offsetGroundBody2
            , float _ratio
            , bool _collideConnected
            , const DocumentPtr & _doc ) = 0;

        virtual Box2DJointInterfacePtr createGearJoint( const Box2DBodyInterfacePtr & _body1
            , const Box2DBodyInterfacePtr & _body2
            , const Box2DJointInterfacePtr & _joint1
            , const Box2DJointInterfacePtr & _joint2
            , float _ratio
            , bool _collideConnected
            , const DocumentPtr & _doc ) = 0;

        virtual Box2DJointInterfacePtr createRopeJoint( const Box2DBodyInterfacePtr & _body1
            , const Box2DBodyInterfacePtr & _body2
            , const mt::vec2f & _offsetBody1
            , const mt::vec2f & _offsetBody2
            , float _maxlength
            , bool _collideConnected
            , const DocumentPtr & _doc ) = 0;


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
            , const DocumentPtr & _doc ) = 0;

        virtual Box2DJointInterfacePtr createRevoluteJoint( const Box2DBodyInterfacePtr & _body1
            , const Box2DBodyInterfacePtr & _body2
            , const mt::vec2f & _localAnchor
            , bool _enableLimit
            , float _lowerAngle
            , float _upperAngle
            , bool _enableMotor
            , float _motorSpeed
            , float _maxMotorTorque
            , const DocumentPtr & _doc ) = 0;

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
        virtual Box2DWorldInterfacePtr createWorld( const mt::vec2f & _gravity, float _scaler, const DocumentPtr & _doc ) = 0;
        virtual void destroyWorld( const Box2DWorldInterfacePtr & _world ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define BOX2D_SERVICE()\
    ((Mengine::Box2DServiceInterface *)SERVICE_GET(Mengine::Box2DServiceInterface))
//////////////////////////////////////////////////////////////////////////
