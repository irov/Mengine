#	pragma once

#	include "../../Interface/PhysicSystem2DInterface.h"
#	include "Box2D.h"
#	include "Box2DPhysicSystem.h"	

class Box2DPhysicBody
	: public PhysicBody2DInterface
	, public b2BodyListener
{
public:
	Box2DPhysicBody( b2World* _world, bool _static );
	virtual ~Box2DPhysicBody();
	bool initialize( const b2BodyDef& _bodyDef );

	void addShapeConvex( unsigned int _pointsNum, const float* _convex,
		float _density, float _friction, float _restitution, bool _isSensor,
		unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex ) override;
	void addShapeCircle( float _radius, const float* _localPos,
		float _density, float _friction, float _restitution, bool _isSensor,
		unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex ) override;
	void addShapeBox( float _width, float _height, const float* _localPos, float _angle,
		float _density, float _friction, float _restitution, bool _isSensor,
		unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex ) override;


	const float * getPosition() const override;
	void setPosition( float _x, float _y ) override;
	const float * getOrientation() override;
	float getAngle() override;
	void setOrientation( float _angle ) override;
	float getMass() const override;

	void setLinearVelocity( float _x, float _y ) override;
	const float* getLinearVelocity() override;
	void setAngularVelocity( float _w ) override;
	float getAngularVelocity() override;
	void applyForce( float _forceX, float _forceY, float _pointX, float _pointY ) override;
	void applyImpulse( float _impulseX, float _impulseY, float _pointX, float _pointY ) override;
	void applyTorque( float _torque ) override;

	void setBodyListener( PhysicBody2DListener* _listener ) override;
	void setUserData( void* _data ) override;
	void* getUserData() const override;

	bool isFrozen() const override;
	bool isSleeping() const override;
	bool isStatic() const override;

	void wakeUp() override;

	void applyForceAndTorque() override;

	b2Body* getBody();

private:
	b2World* m_world;
	b2Body* m_body;
	PhysicBody2DListener* m_listener;
	void* m_userData;
	bool m_isStatic;

	//friend void Box2DPhysicSystem::Add( b2ContactPoint* _point );
	friend void Box2DPhysicSystem::update( float , int );
	void _collide( b2Body* _otherBody, b2ContactPoint* _contact );
};