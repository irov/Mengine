#	pragma once

#	include "Factory/FactorablePtr.h"

#	include "Kernel/Scriptable.h"
#	include "Kernel/Eventable.h"
#	include "Kernel/Servant.h"

#	include "Core/Polygon.h"

#	include "Box2D/Box2D.h"

#   include "pybind/pybind.hpp"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    class Box2DBodyEventReceiver
        : public EventReceiver
    {
    public:
        virtual void onBox2DBodyBeginCollide( class Box2DBody * _self, class Box2DBody * _other, const mt::vec2f & _position, const mt::vec2f & _normal ) = 0;
        virtual void onBox2DBodyUpdateCollide( class Box2DBody * _self, class Box2DBody * _other, const mt::vec2f & _position, const mt::vec2f & _normal ) = 0;
        virtual void onBox2DBodyEndCollide( class Box2DBody * _self, class Box2DBody * _other ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<Box2DBodyEventReceiver> Box2DBodyEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
    class Box2DBody
		: public Servant
		, public Scriptable
		, public Eventable
    {
        EVENT_RECEIVER( Box2DBodyEventReceiver );

    public:
		Box2DBody();
		~Box2DBody();

	public:
		void setWorld( b2World * _world );
		b2World * getWorld() const;

	public:
		void setUserData( const pybind::object & _userData );
		const pybind::object & getUserData() const;

	public:
		void onBeginCollide( Box2DBody * _body, b2Contact * _contact );
		void onUpdateCollide( Box2DBody * _body, b2Contact * _contact );
		void onEndCollide( Box2DBody * _body, b2Contact * _contact );

    public:
		void setBody( b2Body * _body );
		b2Body * getBody() const;

    public:
		bool addShapeConvex( const Menge::Polygon & _vertices, float _density, float _friction, float _restitution, bool _isSensor,
            unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex );
        bool addShapeCircle( float _radius, const mt::vec2f& _localPos,
            float _density, float _friction, float _restitution, bool _isSensor,
            unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex );
		bool addShapeBox( float _width, float _height, const mt::vec2f& _localPos, float _angle,
            float _density, float _friction, float _restitution, bool _isSensor,
            unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex );

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

	//protected:
	//	void _setEventListener( const pybind::dict & _embed ) override;

	protected:
        b2World* m_world;
        b2Body* m_body;		

		pybind::object m_userData;
    };
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<Box2DBody> Box2DBodyPtr;
}