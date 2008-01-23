#	pragma once

#	include "OgreRenderSystemInterface.h"

#	include "Ogre.h"

class Ogre3dCamera 
	: public Camera3dInterface
{
public:
	Ogre3dCamera(Ogre::Camera * _camera);
	~Ogre3dCamera();
public:
	void setPosition( float x, float y, float z ) override;
	void setDirection( float x, float y, float z ) override;
	void lookAt( float x, float y, float z ) override;
	void setNearClipDistance( float _dist ) override;
	void setFarClipDistance( float _dist ) override;
	void setAspectRatio( float _aspect ) override;
private:
	Ogre::Camera * m_camera;
};
