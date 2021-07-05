#pragma once

#include "Box2DInterface.h"

#include "Box2DIncluder.h"
#include "Box2DScaler.h"

#include "Kernel/Eventable.h"

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
        bool initialize( const Box2DScaler & _scaler, b2World * _world, const b2BodyDef * _bodyDef );

    public:
        void setEventable( const EventablePtr & _eventable );
        const EventablePtr & getEventable() const;

    public:        
        b2World * getWorld() const;
        b2Body * getBody() const;

    public:
        bool addShapeConvex( const Polygon & _polygon, float _density, float _friction, float _restitution, bool _isSensor, uint16_t _collisionMask, uint16_t _categoryBits, uint16_t _groupIndex ) override;
        bool addShapeCircle( float _radius, const mt::vec2f & _localPos, float _density, float _friction, float _restitution, bool _isSensor, uint16_t _collisionMask, uint16_t _categoryBits, uint16_t _groupIndex ) override;
        bool addShapeBox( float _width, float _height, const mt::vec2f & _localPos, float _angle, float _density, float _friction, float _restitution, bool _isSensor, uint16_t _collisionMask, uint16_t _categoryBits, uint16_t _groupIndex ) override;

    public:
        mt::vec2f getPosition() const override;
        float getAngle() const override;

    public:
        mt::vec2f getWorldVector( const mt::vec2f & _localVector ) override;        

    public:
        float getMass() const override;
        float getInertia() const override;

        void setLinearVelocity( const mt::vec2f & _velocity ) override;
        mt::vec2f getLinearVelocity() const override;

        void setAngularVelocity( float _w ) override;
        float getAngularVelocity() const override;

        void applyForce( const mt::vec2f & _force, const mt::vec2f & _point ) override;
        void applyImpulse( const mt::vec2f & _impulse, const mt::vec2f & _point ) override;
        void applyAngularImpulse( float _impulse ) override;
        void applyTorque( float _torque ) override;

    public:
        bool isFrozen() const override;
        bool isSleeping() const override;
        bool isStatic() const override;

        void setTransform( const mt::vec2f & _position, float _angle ) override;

        void setLinearDumping( float _dumping ) override;
        float getLinearDumping() const override;
        void setAngularDumping( float _dumping ) override;
        float getAngularDumping() const override;
        void setFixedRotation( bool _rotation ) override;
        bool getFixedRotation() const override;
        void setIsBullet( bool _isBullet ) override;
        bool getIsBullet() const override;

        void sleep() override;
        void wakeUp() override;

        void setFilterData( uint16_t _categoryBits, uint16_t _collisionMask, int16_t _groupIndex ) override;

    protected:
        void onBeginContact( Box2DBody * _body, b2Contact * _contact );
        void onEndContact( Box2DBody * _body, b2Contact * _contact );
        void onPreSolve( Box2DBody * _body, b2Contact * _contact, const b2Manifold * _oldManifold );
        void onPostSolve( Box2DBody * _body, b2Contact * _contact, const b2ContactImpulse * _impulse );

    protected:
        Box2DScaler m_scaler;

        b2World * m_world;
        b2Body * m_body;

        EventablePtr m_eventable;

        friend class Box2DWorld;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Box2DBody, Box2DBodyInterface> Box2DBodyPtr;
    //////////////////////////////////////////////////////////////////////////
}