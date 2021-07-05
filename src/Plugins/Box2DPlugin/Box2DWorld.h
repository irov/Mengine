#pragma once

#include "Interface/TimepipeServiceInterface.h"

#include "Box2DInterface.h"
#include "Box2DIncluder.h"
#include "Box2DScaler.h"

#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class Box2DBody, Box2DBodyInterface> Box2DBodyPtr;
    typedef IntrusivePtr<class Box2DJoint, Box2DJointInterface> Box2DJointPtr;
    //////////////////////////////////////////////////////////////////////////
    class Box2DWorld
        : public Box2DWorldInterface
        , public TimepipeInterface
        , public b2DestructionListener
        , public b2ContactFilter
        , public b2ContactListener
    {
        DECLARE_FACTORABLE( Box2DWorld );

    public:
        Box2DWorld();
        ~Box2DWorld() override;

    public:
        bool initialize( const mt::vec2f & _gravity, float _scaler );
        void finalize();

    protected:
        void onTimepipe( const UpdateContext * _contet ) override;

    public:
        void setDead();
        bool isDead() const;

    public:
        void setTimeStep( float _timeStep, uint32_t _velocityIterations, uint32_t _positionIterations ) override;

    public:
        Box2DBodyInterfacePtr createBody( bool _static
            , const mt::vec2f & _pos
            , float _angle
            , float _linearDamping
            , float _angularDamping
            , bool _allowSleep
            , bool _isBullet
            , bool _fixedRotation
            , const DocumentPtr & _doc ) override;

    public:
        Box2DJointInterfacePtr createDistanceJoint( const Box2DBodyInterfacePtr & _body1
            , const Box2DBodyInterfacePtr & _body2
            , const mt::vec2f & _offsetBody1
            , const mt::vec2f & _offsetBody2
            , bool _collideBodies
            , const DocumentPtr & _doc ) override;

        Box2DJointInterfacePtr createHingeJoint( const Box2DBodyInterfacePtr & _body1
            , const Box2DBodyInterfacePtr & _body2
            , const mt::vec2f & _offsetBody1
            , const mt::vec2f & _limits
            , bool _collideBodies
            , const DocumentPtr & _doc ) override;

        Box2DJointInterfacePtr createPrismaticJoint( const Box2DBodyInterfacePtr & _body1
            , const Box2DBodyInterfacePtr & _body2
            , const mt::vec2f & _unitsWorldAxis
            , bool _collideConnected
            , bool _enableLimit
            , const mt::vec2f & _translation
            , bool _enableMotor
            , float _maxMotorForce
            , float _motorSpeed 
            , const DocumentPtr & _doc ) override;

        Box2DJointInterfacePtr createPulleyJoint( const Box2DBodyInterfacePtr & _body1
            , const Box2DBodyInterfacePtr & _body2
            , const mt::vec2f & _offsetBody1
            , const mt::vec2f & _offsetBody2
            , const mt::vec2f & _offsetGroundBody1
            , const mt::vec2f & _offsetGroundBody2
            , float _ratio
            , bool _collideConnected
            , const DocumentPtr & _doc ) override;

        Box2DJointInterfacePtr createGearJoint( const Box2DBodyInterfacePtr & _body1
            , const Box2DBodyInterfacePtr & _body2
            , const Box2DJointInterfacePtr & _joint1
            , const Box2DJointInterfacePtr & _joint2
            , float _ratio
            , bool _collideConnected
            , const DocumentPtr & _doc ) override;

        Box2DJointInterfacePtr createRopeJoint( const Box2DBodyInterfacePtr & _body1
            , const Box2DBodyInterfacePtr & _body2
            , const mt::vec2f & _offsetBody1
            , const mt::vec2f & _offsetBody2
            , float _maxlength
            , bool _collideConnected
            , const DocumentPtr & _doc ) override;


        Box2DJointInterfacePtr createWheelJoint( const Box2DBodyInterfacePtr & _body1
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
            , const DocumentPtr & _doc ) override;

        Box2DJointInterfacePtr createRevoluteJoint( const Box2DBodyInterfacePtr & _body1
            , const Box2DBodyInterfacePtr & _body2
            , const mt::vec2f & _localAnchor
            , bool _enableLimit
            , float _lowerAngle
            , float _upperAngle
            , bool _enableMotor
            , float _motorSpeed
            , float _maxMotorTorque
            , const DocumentPtr & _doc ) override;

    public:
        void rayCast( const mt::vec2f & _point1, const mt::vec2f & _point2, const Box2DRayCastInterfacePtr & _response ) const override;

    protected:
        void SayGoodbye( b2Joint * joint ) override;
        void SayGoodbye( b2Fixture * fixture ) override;

    protected:
        bool ShouldCollide( b2Fixture * fixtureA, b2Fixture * fixtureB ) override;

    protected:
        void BeginContact( b2Contact * contact ) override;
        void EndContact( b2Contact * contact ) override;
        void PreSolve( b2Contact * contact, const b2Manifold * oldManifold ) override;
        void PostSolve( b2Contact * contact, const b2ContactImpulse * impulse ) override;

    protected:
        Box2DJointInterfacePtr createJoint_( const b2JointDef * _jointDef, const DocumentPtr & _doc );

    protected:
        uint32_t m_timepipeId;

        bool m_dead;

        float m_time;

        b2World * m_world;

        float m_timeStep;
        uint32_t m_velocityIterations;
        uint32_t m_positionIterations;

        Box2DScaler m_scaler;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Box2DWorld, Box2DWorldInterface> Box2DWorldPtr;
    //////////////////////////////////////////////////////////////////////////
}