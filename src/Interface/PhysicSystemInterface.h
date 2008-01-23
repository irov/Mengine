#	pragma once

class RigidBodyInterface
{
public:
	virtual ~RigidBodyInterface(){};

	virtual float * getPosition() = 0;
	virtual float * getDirection() = 0;
	virtual float * getRotationMatrix() = 0;
	
	virtual void applyImpulse( float _x, float _y, float _z ) = 0;
	virtual void applyAngularImpulse( float _x, float _y, float _z ) = 0;
	virtual void applyForce( float _x, float _y, float _z ) = 0;
	virtual void applyTorque( float _x, float _y, float _z ) = 0;

	virtual void setLinearVelocity( float _x, float _y, float _z ) = 0;
	virtual void setAngularVelocity( float _x, float _y, float _z ) = 0;
	virtual void setActive(bool _active) = 0;

	virtual void setLocationMatrix( float * _location ) = 0;
	virtual void setPosition( float _x, float _y, float _z ) = 0;
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
	virtual ~PhysicSystemInterface(){};
	virtual void	init( float gx, float gy, float gz) = 0;
	virtual void	update( float timestep ) = 0;

	virtual GeometryInterface * cookConvex( const float * _verts, int _vertexSize ) = 0;
	virtual GeometryInterface * cookConvex( const float * _verts, int _vertexSize, const int * _indecies, int _indexSize ) = 0;
	virtual GeometryInterface * cookConvex( const char * _filename ) = 0;
	virtual GeometryInterface * cookConcave( const float * _verts, int _vertexSize, const int * _indecies, int _indexSize ) = 0;
	virtual GeometryInterface * cookConcave( const char * _filename ) = 0;
	virtual GeometryInterface * cookBox( float _width, float _height, float _depth ) = 0;
	virtual RigidBodyInterface * createRigidBody( float _density, bool _dynamic, const GeometryInterface * _geometry) = 0;
	virtual void	removeRigidBody( RigidBodyInterface * _rigidBody ) = 0;
	virtual void	createJoint(RigidBodyInterface * body0, RigidBodyInterface * body1, float x, float y, float z) = 0;
};

bool initInterfaceSystem(PhysicSystemInterface** _ptrInterface);
void releaseInterfaceSystem(PhysicSystemInterface* _ptrInterface);