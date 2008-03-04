#	pragma once

class PhysicBody2DInterface;

class PhysicBody2DCollisionListener
{
public:
	virtual void onCollide( PhysicBody2DInterface* _otherObj, float _worldX, float _worldY, float _normalX, float _normalY ) = 0;
};

class PhysicBody2DInterface
{
public:

	virtual const float * getPosition() const = 0;
	virtual void setPosition( float _x, float _y ) = 0;
	virtual const float * getOrientation() = 0;
	virtual void setOrientation( float _angle ) = 0;

	virtual void setLinearVelocity( float _x, float _y ) = 0;
	virtual const float* getLinearVelocity() = 0;
	virtual void setAngularVelocity( float _w ) = 0;
	virtual float getAngularVelocity() = 0;
	virtual void applyForce( float _forceX, float _forceY, float _pointX, float _pointY ) = 0;
	virtual void applyImpulse( float _impulseX, float _impulseY, float _pointX, float _pointY ) = 0;

	virtual void setCollisionListener( PhysicBody2DCollisionListener* ) = 0;
	virtual void setUserData( void* ) = 0;
	virtual void* getUserData() const = 0;

	virtual bool isFrozen() const = 0;
	virtual bool isSleeping() const = 0;
	virtual bool isStatic() const = 0;
};

class PhysicSystem2DInterface
{
public:
	virtual void createWorld( const float* _upperLeft, const float* _lowerRight, const float* _gravity, bool _doSleep ) = 0;
	virtual void destroyWorld() = 0;
	virtual void update( float _timing, int _iterations ) = 0;
	
	virtual PhysicBody2DInterface* createPhysicBodyBox( float _width, float _heigth, 
														float _posX, float _posY, float _angle,
														float _density, float _friction, float _restitution,
														float _shapeX, float _shapeY, float _shapeAngle,
														unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex ) = 0;
	virtual PhysicBody2DInterface* createPhysicBodyCircle( float _radius, 
															float _posX, float _posY, float _angle,
															float _density, float _friction, float _restitution,
															float _shapeX, float _shapeY,
															unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex ) = 0;
	virtual PhysicBody2DInterface* createPhysicBodyConvex( int _pointsNum, const float* _convex,
															float _posX, float _posY, float _angle,
															float _density, float _friction, float _restitution,
															float _shapeX, float _shapeY, float _shapeAngle,
															unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex ) = 0;
	virtual void destroyBody( PhysicBody2DInterface* _body ) = 0;
};

bool initInterfaceSystem(PhysicSystem2DInterface** _ptrInterface);
void releaseInterfaceSystem(PhysicSystem2DInterface* _ptrInterface);