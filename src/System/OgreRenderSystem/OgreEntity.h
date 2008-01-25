#	pragma once

#	include "OgreRenderSystemInterface.h"

#	include "Ogre.h"

class Ogre3dEntity
	: public EntityInterface
{
public:
	Ogre3dEntity(Ogre::SceneNode * _sceneNode/*Ogre::Entity * _entity*/);
	~Ogre3dEntity();

	void setPosition(float x, float y, float z) override;
	void setDirection(float w, float x, float y, float z) override;
	void setDirection1(float * q);
public:
	Ogre::SceneNode * m_sceneNode;
	//
	//Ogre::Entity * m_entity;
};
