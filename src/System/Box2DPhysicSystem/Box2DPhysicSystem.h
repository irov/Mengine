#	pragma once

#	include "../../Interface/PhysicSystem2DInterface.h"

#	include "Box2D.h"

class Box2DPhysicSystem
	: public PhysicSystem2DInterface
{
public:
	Box2DPhysicSystem();
	virtual ~Box2DPhysicSystem();

	void createWorld( const float* _upperLeft, const float* _lowerRight, const float* _gravity, bool _doSleep ) override;
	void destroyWorld() override;
	void update( float _timing, int _iterations ) override;

	PhysicBody2DInterface* createPhysicBodyBox( float _width, float _heigth, 
												float _posX, float _posY, float _angle,
												float _density, float _friction, float _restitution,
												float _shapeX, float _shapeY, float _shapeAngle,
												unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex ) override;

	PhysicBody2DInterface* createPhysicBodyCircle( float _radius, 
													float _posX, float _posY, float _angle,
													float _density, float _friction, float _restitution,
													float _shapeX, float _shapeY,
													unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex ) override;

	PhysicBody2DInterface* createPhysicBodyConvex( int _pointsNum, const float* _convex,
													float _posX, float _posY, float _angle,
													float _density, float _friction, float _restitution,
													float _shapeX, float _shapeY, float _shapeAngle,
													unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex ) override;

	void destroyBody( PhysicBody2DInterface* _body ) override;

private:
	 b2World* m_world;
};