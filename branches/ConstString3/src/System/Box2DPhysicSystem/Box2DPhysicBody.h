#	pragma once

#	include "../../Interface/PhysicSystem2DInterface.h"
#	include "Utils/Math/box2.h"
#	include "Utils/Math/mat2.h"
#	include "Utils/Core/Polygon.h"
#	include "Box2D/Box2D.h"

#	include "Box2DPhysicSystem.h"	

class Box2DPhysicScaler;

class Box2DPhysicBody
	: public Menge::PhysicBody2DInterface
{
public:
	Box2DPhysicBody( b2World* _world );
	virtual ~Box2DPhysicBody();

public:
	bool initialize( const b2BodyDef& _bodyDef , Box2DPhysicScaler* _scaler );

public:
	void addShapeConvex( Menge::Polygon _vertices, float _density, float _friction, float _restitution, bool _isSensor,
		unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex ) override;
	void addShapeCircle( float _radius, const mt::vec2f& _localPos,
		float _density, float _friction, float _restitution, bool _isSensor,
		unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex ) override;
	void addShapeBox( float _width, float _height, const mt::vec2f& _localPos, float _angle,
		float _density, float _friction, float _restitution, bool _isSensor,
		unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex ) override;

public:
	mt::vec2f& getPosition()  override;
	void setPosition( float _x, float _y ) override;
	mt::vec2f& getOrientation() override;
	
	void setOrientation( float _angle ) override;
	float getAngle() override;
	
	float getMass() const override;
	float getInertia() const override;
	
	void setLinearVelocity( float _x, float _y ) override;
	mt::vec2f& getLinearVelocity()  override;
	
	void setAngularVelocity( float _w ) override;
	float getAngularVelocity() const override;
	
	void applyForce( float _forceX, float _forceY, float _pointX, float _pointY ) override;
	void applyImpulse( float _impulseX, float _impulseY, float _pointX, float _pointY ) override;
	void applyTorque( float _torque ) override;
	
	void dump();

public:
	void setBodyListener( Menge::PhysicBody2DListener* _listener ) override;
	void setUserData( void* _data ) override;
	void* getUserData() const override;

public:
	bool isFrozen() const override;
	bool isSleeping() const override;
	bool isStatic() const override;

	void setLinearDumping( float _dumping ) override;
	float getLinearDumping() const override ;
	void setAngularDumping( float _dumping ) override;
	float getAngularDumping() const override;
	void setFixedRotation( bool _rotation ) override;
	bool getFixedRotation() const override;
	void setIsBullet( bool _isBullet ) override; 
	bool getIsBullet() const override; 

	void sleep() override;
	void wakeUp() override;
	
	void updateFilterData( Menge::uint16 _categoryBits, Menge::uint16 _collisionMask, signed short _groupIndex ) override;

	void update() override;
	const mt::box2f&  getBoundingBox() override;
	b2Body* getBody();
public:
	void setScaler( Box2DPhysicScaler* _scaler );
protected:
	Box2DPhysicScaler * m_scaler;
	void _updateBoundingBox();
private:
	b2World* m_world;
	b2Body* m_body;
	Menge::PhysicBody2DListener* m_listener;
	void* m_userData;
	
	mt::vec2f m_direction;
	mt::vec2f m_position;
	mt::vec2f m_velocity;
	mt::box2f m_boundingBox;
	
	
	//friend void Box2DPhysicSystem::Add( b2ContactPoint* _point );
	friend void Box2DPhysicSystem::update( float , int, int );
	void _collide( b2Body* _otherBody, b2Contact* _contact );
	
	
};
