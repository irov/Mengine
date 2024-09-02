#pragma once

#include "Box2DInterface.h"

#include "Box2DIncluder.h"
#include "Box2DScaler.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Box2DBody
        : public Box2DBodyInterface
    {
        DECLARE_FACTORABLE( Box2DBody );

    public:
        Box2DBody();
        ~Box2DBody() override;

    public:
        bool initialize( const Box2DScaler & _scaler, b2BodyId _bodyId );

    public:
        void setNode( const NodePtr & _node ) override;
        const NodePtr & getNode() const override;

    public:
        b2BodyId getBodyId() const;

    public:
        bool addShapeConvex( const Polygon & _polygon, float _density, float _friction, float _restitution, bool _isSensor, uint16_t _collisionMask, uint16_t _categoryBits, uint16_t _groupIndex ) override;
        bool addShapeCircle( float _radius, const mt::vec2f & _localPos, float _density, float _friction, float _restitution, bool _isSensor, uint16_t _collisionMask, uint16_t _categoryBits, uint16_t _groupIndex ) override;
        bool addShapeBox( float _width, float _height, const mt::vec2f & _localPos, float _angle, float _density, float _friction, float _restitution, bool _isSensor, uint16_t _collisionMask, uint16_t _categoryBits, uint16_t _groupIndex ) override;

    public:
        mt::vec2f getBodyPosition() const override;
        float getBodyAngle() const override;

    public:
        mt::vec2f getBodyWorldVector( const mt::vec2f & _localVector ) override;

    public:
        float getBodyMass() const override;
        float getBodyInertiaTensor() const override;

        void setBodyLinearVelocity( const mt::vec2f & _velocity ) override;
        mt::vec2f getBodyLinearVelocity() const override;

        void setBodyAngularVelocity( float _angularVelocity ) override;
        float getBodyAngularVelocity() const override;

        void applyForce( const mt::vec2f & _force, const mt::vec2f & _point ) override;
        void applyImpulse( const mt::vec2f & _impulse, const mt::vec2f & _point ) override;
        void applyAngularImpulse( float _impulse ) override;
        void applyTorque( float _torque ) override;

    public:
        bool isFrozen() const override;
        bool isSleeping() const override;
        bool isStatic() const override;
        bool isKinematic() const override;
        bool isDynamic() const override;

    public:
        void setTransform( const mt::vec2f & _position, float _angle ) override;

    public:
        void setBodyLinearDumping( float _dumping ) override;
        float getBodyLinearDumping() const override;
        void setBodyAngularDumping( float _dumping ) override;
        float getBodyAngularDumping() const override;
        void setBodyFixedRotation( bool _rotation ) override;
        bool isBodyFixedRotation() const override;
        void setBodyBulletMode( bool _bulletMode ) override;
        bool isBodyBulletMode() const override;

    public:
        void sleep() override;
        void wakeUp() override;

    public:
        void setFilterData( uint16_t _categoryBits, uint16_t _collisionMask, int16_t _groupIndex ) override;

    protected:
        Box2DScaler m_scaler;

        b2BodyId m_bodyId;

        NodePtr m_node;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Box2DBody, Box2DBodyInterface> Box2DBodyPtr;
    //////////////////////////////////////////////////////////////////////////
}