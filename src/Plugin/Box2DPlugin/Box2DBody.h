#pragma once

#include "Factory/Factorable.h"

#include "Kernel/Scriptable.h"
#include "Kernel/Eventable.h"
#include "Kernel/Servant.h"

#include "Core/Polygon.h"

#include "Box2D/Box2D.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	enum Box2DBodyEventFlag
	{
		EVENT_BOX2DBODY_BEGIN_CONTACT,
		EVENT_BOX2DBODY_END_CONTACT,
		EVENT_BOX2DBODY_PRE_SOLVE,
		EVENT_BOX2DBODY_POST_SOLVE,
		EVENT_BOX2DBODY_UPDATE_CONTACT,
	};
    //////////////////////////////////////////////////////////////////////////
    class Box2DBodyEventReceiver
        : public EventReceiver
    {
    public:
        virtual void onBox2DBodyBeginContact( class Box2DBody * _other, b2Contact * _contact ) = 0;
		virtual void onBox2DBodyEndContact( class Box2DBody * _other, b2Contact * _contact ) = 0;

		virtual void onBox2DBodyPreSolve( class Box2DBody * _other, b2Contact * _contact, const b2Manifold* _manifold ) = 0;
		virtual void onBox2DBodyPostSolve( class Box2DBody * _other, b2Contact * _contact, const b2ContactImpulse* _impulse ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Box2DBodyEventReceiver> Box2DBodyEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
    class Box2DBody
		: public Servant
		, public Scriptable
		, public Eventable
    {
        EVENT_RECEIVER( Box2DBodyEventReceiver );

    public:
		Box2DBody();
		~Box2DBody() override;

	public:
		void setWorld( b2World * _world );
		b2World * getWorld() const;

	public:
		void setUserData( const pybind::object & _userData );
		const pybind::object & getUserData() const;

	public:
		void onBeginContact( Box2DBody * _body, b2Contact * _contact );		
		void onEndContact( Box2DBody * _body, b2Contact * _contact );
		void onPreSolve( Box2DBody * _body, b2Contact * _contact, const b2Manifold* _oldManifold );
		void onPostSolve( Box2DBody * _body, b2Contact * _contact, const b2ContactImpulse* _impulse );

    public:
		void setBody( b2Body * _body );
		b2Body * getBody() const;

    public:
		bool addShapeConvex( const Polygon & _vertices, float _density, float _friction, float _restitution, bool _isSensor
			, unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex );
        bool addShapeCircle( float _radius, const mt::vec2f& _localPos
			, float _density, float _friction, float _restitution, bool _isSensor
			, unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex );
		bool addShapeBox( float _width, float _height, const mt::vec2f& _localPos, float _angle
			, float _density, float _friction, float _restitution, bool _isSensor
			, unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex );

	public:
		mt::vec2f GetPosition() const;
		float GetAngle() const;

    public:
        float getMass() const;
        float getInertia() const;

		void setLinearVelocity( const mt::vec2f & _velocity );
        mt::vec2f getLinearVelocity() const;

        void setAngularVelocity( float _w );
        float getAngularVelocity() const;

		void applyForce( const mt::vec2f & _force, const mt::vec2f & _point );
		void applyImpulse( const mt::vec2f & _impulse, const mt::vec2f & _point );
        void applyTorque( float _torque );

    public:
        bool isFrozen() const;
		bool isSleeping() const;
        bool isStatic() const;

        void setLinearDumping( float _dumping );
        float getLinearDumping() const;
        void setAngularDumping( float _dumping );
        float getAngularDumping() const;
        void setFixedRotation( bool _rotation );
        bool getFixedRotation() const;
        void setIsBullet( bool _isBullet ); 
        bool getIsBullet() const; 

        void sleep();
        void wakeUp();

		void updateFilterData( uint16_t _categoryBits, uint16_t _collisionMask, int16_t _groupIndex );

		void filterContactList( const pybind::object & _filter, const pybind::args & _args );

	protected:
        b2World* m_world;
        b2Body* m_body;		

		pybind::object m_userData;
    };
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<Box2DBody> Box2DBodyPtr;
}