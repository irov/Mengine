#	pragma once

#	include "OgreRenderSystemInterface.h"

#	include "Ogre.h"

class Ogre3dEntity
	: public Entity3dInterface
{
public:
	Ogre3dEntity(Ogre::SceneNode * _sceneNode/*Ogre::Entity * _entity*/);
	~Ogre3dEntity();

	void setPosition(float x, float y, float z) override;
	void setDirection(float * q) override;
	void setDirection1(float * q);
public:
	Ogre::SceneNode * m_sceneNode;
	//
	//Ogre::Entity * m_entity;
};
