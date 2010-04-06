#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	const float physicsScaler = 0.01f; 

	class PhysicBody2DInterface;

	class PhysicBody2DListener
	{
	public:
		virtual void onCollide( PhysicBody2DInterface* _otherObj, float _worldX, float _worldY, float _normalX, float _normalY ) = 0;
		virtual void onUpdate() = 0;
	};

	class PhysicBody2DInterface
	{
	public:

		virtual void addShapeConvex( 
			std::size_t _pointsNum, 
			const float* _convex,
			float _density,
			float _friction, 
			float _restitution, 
			bool _isSensor,
			unsigned short _collisionMask, 
			unsigned short _categoryBits, 
			unsigned short _groupIndex ) = 0;

		virtual void addShapeCircle( float _radius, const float* _localPos,
			float _density, float _friction, float _restitution, bool _isSensor,
			unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex ) = 0;

		virtual void addShapeBox( float _width, float _height, const float* _localPos, float _angle,
			float _density, float _friction, float _restitution, bool _isSensor,
			unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex ) = 0;

		virtual const float * getPosition() const = 0;
		virtual void setPosition( float _x, float _y ) = 0;
		virtual const float * getOrientation() = 0;
		virtual float getAngle() = 0;
		virtual void setOrientation( float _angle ) = 0;
		virtual	float getMass() const = 0;
		virtual float getInertia() const = 0;

		virtual void setLinearVelocity( float _x, float _y ) = 0;
		virtual const float* getLinearVelocity() = 0;
		virtual void setAngularVelocity( float _w ) = 0;
		virtual float getAngularVelocity() = 0;
		virtual void applyForce( float _forceX, float _forceY, float _pointX, float _pointY ) = 0;
		virtual void applyImpulse( float _impulseX, float _impulseY, float _pointX, float _pointY ) = 0;
		virtual void applyTorque( float _torque ) = 0;

		virtual void setBodyListener( PhysicBody2DListener* ) = 0;
		virtual void setUserData( void* ) = 0;
		virtual void* getUserData() const = 0;

		virtual bool isFrozen() const = 0;
		virtual bool isSleeping() const = 0;
		virtual bool isStatic() const = 0;

		virtual void sleep() = 0;
		virtual void wakeUp() = 0;

		virtual void updateFilterData( uint16 _categoryBits, uint16 _collisionMask, signed short _groupIndex ) = 0;
		virtual void update() = 0;
	};

	class PhysicJoint2DInterface
	{};

	class PhysicSystem2DInterface
	{
	public:
		virtual void createWorld( const float* _upperLeft, const float* _lowerRight, const float* _gravity, bool _doSleep ) = 0;
		virtual void destroyWorld() = 0;
		virtual void update( float _timing, int _velocityIterations, int _positionIterations ) = 0;

		virtual PhysicBody2DInterface* createBody( const float* _pos, float _angle, float _linearDamping, float _angularDamping,
			bool _allowSleep, bool _isBullet, bool _fixedRotation ) = 0;

		virtual void destroyBody( PhysicBody2DInterface* _body ) = 0;

		virtual PhysicJoint2DInterface* createDistanceJoint( PhysicBody2DInterface* _body1, PhysicBody2DInterface* _body2, const float* _offsetBody1, const float* _offsetBody2, bool _collideBodies ) = 0;
		virtual PhysicJoint2DInterface* createHingeJoint( PhysicBody2DInterface* _body1, PhysicBody2DInterface* _body2, const float* _offsetBody1, const float* _limits, bool _collideBodies ) = 0;
		virtual PhysicJoint2DInterface* createMouseJoint( PhysicBody2DInterface* _body, int _x, int _y  ) = 0;
		virtual void destroyJoint( PhysicJoint2DInterface* ) = 0;

		virtual void onMouseMove( float x, float y ) = 0;
	};
}

bool initInterfaceSystem( Menge::PhysicSystem2DInterface** );
void releaseInterfaceSystem( Menge::PhysicSystem2DInterface* );
