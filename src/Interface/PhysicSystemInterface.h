#	pragma once

class RigidBodyInterface
{
public:
	virtual ~RigidBodyInterface(){};

	virtual float * getPosition() = 0;
	virtual float * getOrient() = 0;
	
	virtual void applyImpulse( float _x, float _y, float _z ) = 0;
	virtual void applyAngularImpulse( float _x, float _y, float _z ) = 0;
	virtual void applyForce( float _x, float _y, float _z ) = 0;
	virtual void applyTorque( float _x, float _y, float _z ) = 0;

	virtual void setLinearVelocity( float _x, float _y, float _z ) = 0;
	virtual void setAngularVelocity( float _x, float _y, float _z ) = 0;
	virtual void setActive(bool _active) = 0;

	virtual void setPosition( float _x, float _y, float _z ) = 0;
	virtual void setOrient( float _w, float _x, float _y, float _z ) = 0;
};

class GeometryInterface
{
public:
	virtual ~GeometryInterface(){};
};
//joints: пока в процессе.
class	JointInterface
{
public:
	virtual ~JointInterface(){};
};

class	BallSocketJointInterface : public JointInterface
{
public:
	virtual ~BallSocketJointInterface(){}; 

	virtual void init(
		RigidBodyInterface * body0,
		RigidBodyInterface * body1,
		float x, float y, float z) = 0;

	virtual void setLimits(float cone_limit_rad, float twist_limit_rad) = 0;
};

class PhysicSystemInterface
{
public:
	virtual void init( float _gx, float _gy, float _gz) = 0;
	virtual void update( float _timestep ) = 0;
public:
	virtual void setGravity( float _x, float _y, float _z ) = 0;
	virtual void setRestitution( float _value ) = 0;
	virtual void setStaticFriction( float _value ) = 0;
	virtual void setDynamicFriction( float _value ) = 0;
	virtual float rayCast(const char * _name, float * pos, float * dir) = 0;
public:

	virtual float * getControllerPos() = 0;
	virtual void moveController( float * _dispvec ) = 0;
	virtual void createController( float * _startPos, float _initialRadius, float _initialHeight ) = 0;
	virtual GeometryInterface * cookConvex( const float * _verts, int _vertexSize ) = 0;
	virtual GeometryInterface * cookConvex( const float * _verts, int _vertexSize, const int * _indecies, int _indexSize ) = 0;
	virtual GeometryInterface * cookConvex( const char * _filename ) = 0;
	virtual GeometryInterface * cookConcave( const float * _verts, int _vertexSize, const int * _indecies, int _indexSize ) = 0;
	virtual GeometryInterface * cookConcave( const char * _filename ) = 0;
	virtual GeometryInterface * cookBox( float _width, float _height, float _depth ) = 0;
	virtual void removeGeometry( GeometryInterface * _geom ) = 0;
public:
	virtual RigidBodyInterface * createRigidBody( float _density, bool _dynamic, const GeometryInterface * _geometry) = 0;
	virtual void	removeRigidBody( RigidBodyInterface * _rigidBody ) = 0;
	virtual void	createJoint(RigidBodyInterface * body0, RigidBodyInterface * body1, float x, float y, float z) = 0;
};

bool initInterfaceSystem(PhysicSystemInterface** _ptrInterface);
void releaseInterfaceSystem(PhysicSystemInterface* _ptrInterface);