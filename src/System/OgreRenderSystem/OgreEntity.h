#	pragma once

#	include "OgreRenderSystemInterface.h"

#	include "Ogre.h"

class Ogre3dEntity
//	: public EntityInterface
{
public:
	Ogre3dEntity(Ogre::SceneNode * _sceneNode/*Ogre::Entity * _entity*/);
	~Ogre3dEntity();
public:
	void setPosition(float x, float y, float z);
	void setOrient(float w, float x, float y, float z);
public:
	Ogre::SceneNode * m_sceneNode;
	//
	//Ogre::Entity * m_entity;
};
