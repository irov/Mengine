#	pragma once

class PhysicBody2DInterface
{
public:

	virtual const float * getPosition() = 0;
	virtual const float * getOrientation() = 0;

	virtual void setLinearVelocity( float x, float y ) = 0;
};

class PhysicSystem2DInterface
{
public:
	virtual void createWorld( float* _upperLeft, float* _lowerRight, float* _gravity, bool _doSleep ) = 0;
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
	virtual PhysicBody2DInterface* createPhysicBodyConvex( float _pointsNum, float* _convex,
															float _posX, float _posY, float _angle,
															float _density, float _friction, float _restitution,
															float _shapeX, float _shapeY, float _shapeAngle,
															unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex ) = 0;
	virtual void destroyBody( PhysicBody2DInterface* _body ) = 0;
};

bool initInterfaceSystem(PhysicSystem2DInterface** _ptrInterface);
void releaseInterfaceSystem(PhysicSystem2DInterface* _ptrInterface);