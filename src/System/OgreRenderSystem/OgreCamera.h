#	pragma once

#	include "OgreRenderSystemInterface.h"

#	include "Ogre.h"

class Ogre3dCamera 
	: public CameraInterface
{
public:
	Ogre3dCamera(Ogre::Camera * _camera);
	~Ogre3dCamera();
public:
	void setOrient(float * _q) override;
	const float * getOrient() const override;
	void rotate(float * _dir, float _angle) override;
	void setPosition( float _x, float _y, float _z ) override;
	const float * getPosition() const override;
	const float * getDirection() const override;
	void setDirection( float _x, float _y, float _z ) override;
	void lookAt( float _x, float _y, float _z ) override;
	void setNearClipDistance( float _dist ) override;
	void setFarClipDistance( float _dist ) override;
	void setAspectRatio( float _aspect ) override;
	void yaw( float _angle ) override;
	void pitch( float _angle ) override;
	void roll( float _angle ) override;
	void getAABB( float * _min, float * _max ) const override;
	void translate( float * _v ) override;
	int isAABBIntersect( float * _min, float * _max, float & _dx, float & _dz );
	int isSphereIntersect( float x, float y, float z, float R, float * q );
public:
	Ogre::Camera * getOgreCamera();

private:
	Ogre::Camera * m_camera;
};