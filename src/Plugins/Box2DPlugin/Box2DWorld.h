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
        void setTimeStep( float _timeStep, uint32_t _subStepCount ) override;

    public:
        Box2DBodyInterfacePtr createBody( bool _static
            , const mt::vec2f & _pos
            , float _angle
            , float _linearDamping
            , float _angularDamping
            , bool _allowSleep
            , bool _isBullet
            , bool _fixedRotation
            , const DocumentInterfacePtr & _doc ) override;

    public:
        Box2DJointInterfacePtr createDistanceJoint( const Box2DBodyInterfacePtr & _body1
            , const Box2DBodyInterfacePtr & _body2
            , const mt::vec2f & _offsetBody1
            , const mt::vec2f & _offsetBody2
            , bool _collideBodies
            , const DocumentInterfacePtr & _doc ) override;

        Box2DJointInterfacePtr createHingeJoint( const Box2DBodyInterfacePtr & _body1
            , const Box2DBodyInterfacePtr & _body2
            , const mt::vec2f & _offsetBody1
            , const mt::vec2f & _limits
            , bool _collideBodies
            , const DocumentInterfacePtr & _doc ) override;

        Box2DJointInterfacePtr createPrismaticJoint( const Box2DBodyInterfacePtr & _body1
            , const Box2DBodyInterfacePtr & _body2
            , const mt::vec2f & _unitsWorldAxis
            , bool _collideConnected
            , bool _enableLimit
            , const mt::vec2f & _translation
            , bool _enableMotor
            , float _maxMotorForce
            , float _motorSpeed 
            , const DocumentInterfacePtr & _doc ) override;

        Box2DJointInterfacePtr createWeldJoint( const Box2DBodyInterfacePtr & _body1
            , const Box2DBodyInterfacePtr & _body2
            , const mt::vec2f & _offsetBody1
            , const mt::vec2f & _offsetBody2
            , bool _collideConnected
            , const DocumentInterfacePtr & _doc ) override;

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
            , const DocumentInterfacePtr & _doc ) override;

        Box2DJointInterfacePtr createRevoluteJoint( const Box2DBodyInterfacePtr & _body1
            , const Box2DBodyInterfacePtr & _body2
            , const mt::vec2f & _localAnchor
            , bool _enableLimit
            , float _lowerAngle
            , float _upperAngle
            , bool _enableMotor
            , float _motorSpeed
            , float _maxMotorTorque
            , const DocumentInterfacePtr & _doc ) override;

    public:
        void rayCast( const mt::vec2f & _point1, const mt::vec2f & _point2, const Box2DRayCastInterfacePtr & _response ) const override;

    protected:
        Box2DJointInterfacePtr createJoint_( b2JointId jointId, const DocumentInterfacePtr & _doc );

    protected:
        UniqueId m_timepipeId;

        bool m_dead;

        float m_time;

        b2WorldId m_worldId;

        float m_timeStep;
        uint32_t m_subStepCount;

        Box2DScaler m_scaler;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Box2DWorld, Box2DWorldInterface> Box2DWorldPtr;
    //////////////////////////////////////////////////////////////////////////
}