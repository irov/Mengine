#	include "Ogre3dEntity.h"
//////////////////////////////////////////////////////////////////////////
Ogre3dEntity::Ogre3dEntity(Ogre::SceneNode * _sceneNode/*Ogre::Entity * _entity*/)
: m_sceneNode(_sceneNode)
{
}
//////////////////////////////////////////////////////////////////////////
Ogre3dEntity::~Ogre3dEntity()
{
}
//////////////////////////////////////////////////////////////////////////
void Ogre3dEntity::setPosition(float x, float y, float z)
{
	m_sceneNode->setPosition(x,y,z);
}
//////////////////////////////////////////////////////////////////////////
void Ogre3dEntity::setDirection(float * q)
{
	m_sceneNode->setOrientation(*(Ogre::Quaternion *)q);
}
//////////////////////////////////////////////////////////////////////////
void Ogre3dEntity::setDirection1(float * q)
{
	m_sceneNode->setDirection(q[0],q[1],q[2],Ogre::Node::TS_WORLD,Ogre::Vector3(0,0,1));
}