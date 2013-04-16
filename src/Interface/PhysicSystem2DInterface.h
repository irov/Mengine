#	pragma once

#   include "Interface/ServiceInterface.h"

#	include "Config/Typedef.h"
#	include "Utils/Math/box2.h"
#	include "Utils/Core/Polygon.h"

namespace Menge
{
	const float physicsScaler = 0.01f; 
	const float oneDivPhysicsScaler = 1.f / physicsScaler;
	const int  bodyTypeStatic = 1;
	const int  bodyTypeDynamic = 2;

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
			Menge::Polygon vertices
			, float _density
			, float _friction
			, float _restitution 
			, bool _isSensor
			, unsigned short _collisionMask 
			, unsigned short _categoryBits 
			, unsigned short _groupIndex ) = 0;

		virtual void addShapeCircle( float _radius, const mt::vec2f& _localPos,
			float _density, float _friction, float _restitution, bool _isSensor,
			unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex ) = 0;

		virtual void addShapeBox( float _width, float _height,  const mt::vec2f& _localPos, float _angle,
			float _density, float _friction, float _restitution, bool _isSensor,
			unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex ) = 0;

		virtual mt::vec2f& getPosition()  = 0;
		virtual void setPosition( float _x, float _y ) = 0;
		virtual float getAngle() = 0;
		virtual mt::vec2f& getOrientation() = 0;
		virtual void setOrientation( float _angle ) = 0;
		virtual	float getMass() const = 0;
		virtual float getInertia() const = 0;

		virtual void setLinearVelocity( float _x, float _y ) = 0;
		virtual mt::vec2f& getLinearVelocity() = 0;
		virtual void setAngularVelocity( float _w ) = 0;
		virtual float getAngularVelocity() const = 0;
		virtual void setLinearDumping( float _dumping ) = 0;
		virtual float getLinearDumping() const = 0;
		virtual void setAngularDumping( float _dumping ) = 0;
		virtual float getAngularDumping() const = 0;
		virtual void setFixedRotation( bool _rotation ) = 0;
		virtual bool getFixedRotation() const = 0;
		virtual void setIsBullet( bool _isBullet ) = 0; 
		virtual bool getIsBullet() const = 0; 
		
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
		virtual const mt::box2f&  getBoundingBox( ) = 0;
		virtual void updateFilterData( uint16 _categoryBits, uint16 _collisionMask, signed short _groupIndex ) = 0;
		virtual void update() = 0;
		virtual void dump() = 0;
	};

	class PhysicJoint2DInterface
	{};

	class PhysicSystem2DInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("PhysicSystem2D");

    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;

	public:
		virtual void createWorld( const mt::vec2f& _upperLeft, const mt::vec2f& _lowerRight, const mt::vec2f& _gravity, bool _doSleep ) = 0;
		virtual void destroyWorld() = 0;
		virtual void update( float _timing, int _velocityIterations, int _positionIterations ) = 0;

		virtual PhysicBody2DInterface* createBody( const int bodyType , const mt::vec2f& _pos, float _angle, float _linearDamping, float _angularDamping,
			bool _allowSleep, bool _isBullet, bool _fixedRotation ) = 0;

		virtual void destroyBody( PhysicBody2DInterface* _body ) = 0;
		
		virtual PhysicJoint2DInterface * createDistanceJoint(
			Menge::PhysicBody2DInterface* _body1
			, Menge::PhysicBody2DInterface* _body2
			, const mt::vec2f& _offsetBody1
			, const mt::vec2f& _offsetBody2
			, bool _collideBodies ) = 0;

		virtual PhysicJoint2DInterface * createHingeJoint(
			Menge::PhysicBody2DInterface* _body1
			, Menge::PhysicBody2DInterface* _body2
			, const mt::vec2f& _offsetBody1
			, const mt::vec2f& _limits
			, bool _collideBodies ) = 0;

		virtual PhysicJoint2DInterface * createMouseJoint( Menge::PhysicBody2DInterface* _body1, int _x, int _y ) = 0;

		virtual PhysicJoint2DInterface * createPrismaticJoint(
			Menge::PhysicBody2DInterface* _body1
			, Menge::PhysicBody2DInterface* _body2
			, const mt::vec2f& _unitsWorldAxis
			, bool _collideConnected 
			, bool _enableLimit
			, const mt::vec2f& _translation 
			, bool _enableMotor
			, float _maxMotorForce
			, float _motorSpeed) = 0;

		virtual PhysicJoint2DInterface * createPulleyJoint(
			Menge::PhysicBody2DInterface* _body1
			, Menge::PhysicBody2DInterface* _body2
			, const mt::vec2f& _offsetBody1
			, const mt::vec2f& _offsetBody2
			, const mt::vec2f& _offsetGroundBody1
			, const mt::vec2f& _offsetGroundBody2
			, float _ratio
			, bool _collideConnected ) = 0;

		virtual PhysicJoint2DInterface * createGearJoint(
			Menge::PhysicBody2DInterface * _body1
			, Menge::PhysicBody2DInterface * _body2
			, Menge::PhysicJoint2DInterface * _joint1
			, Menge::PhysicJoint2DInterface * _joint2
			, float _ratio
			, bool _collideConnected ) = 0;	

		virtual PhysicJoint2DInterface * createRopeJoint(
			Menge::PhysicBody2DInterface * _body1
			, Menge::PhysicBody2DInterface * _body2
			, const mt::vec2f & _localAnchor1
			, const mt::vec2f & _localAnchor2
			, float _maxlength
			, bool _collideConnected ) = 0;	

		virtual PhysicJoint2DInterface * createWheelJoint	(
			Menge::PhysicBody2DInterface * _body1
			, Menge::PhysicBody2DInterface * _body2
			, const mt::vec2f & _localAnchor1
			, const mt::vec2f & _localAnchor2
			, const mt::vec2f & _localAxis1
			, float _frequencyHz
			, float _dampingRatio
			, bool _collideConnected 
			, float _maxMotorTorque
			, float _motorSpeed	) = 0;	

		virtual void destroyJoint( PhysicJoint2DInterface* ) = 0;
		virtual void onMouseMove( float x, float y ) = 0;
	};

#   define PHYSIC2D_SYSTEM( serviceProvider )\
    (Menge::getService<Menge::PhysicSystem2DInterface>(serviceProvider))

    class PhysicService2DInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("PhysicService2D")

    public:
        virtual bool initialize() = 0;

    public:
        virtual void createWorld( const mt::vec2f& _upperLeft, const mt::vec2f& _lowerRight, const mt::vec2f& _gravity, bool _doSleep ) = 0;
        virtual void destroyWorld() = 0;
        virtual void update( float _timing ) = 0;
    };

#   define PHYSIC2D_SERVICE( serviceProvider )\
    (Menge::getService<Menge::PhysicService2DInterface>(serviceProvider))
}