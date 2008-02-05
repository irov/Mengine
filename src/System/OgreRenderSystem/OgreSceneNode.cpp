#	include "OgreSceneNode.h"
#	include "OgreEntity.h"
//////////////////////////////////////////////////////////////////////////
OgreSceneNode::OgreSceneNode(Ogre::SceneNode * _node)
: m_sceneNode(_node)
{
}
//////////////////////////////////////////////////////////////////////////
const float * OgreSceneNode::getWorldOrient()
{
	const Ogre::Quaternion & orient = m_sceneNode->getWorldOrientation();
	return orient.ptr();
}
//////////////////////////////////////////////////////////////////////////
const float * OgreSceneNode::getWorldPosition()
{
	const Ogre::Vector3 & pos = m_sceneNode->getWorldPosition();
	return pos.ptr();
}
//////////////////////////////////////////////////////////////////////////
float * OgreSceneNode::getLocalPosition()
{
	return m_position.ptr();
}
//////////////////////////////////////////////////////////////////////////
float * OgreSceneNode::getLocalOrient()
{
	return m_localOrient.ptr();
}
//////////////////////////////////////////////////////////////////////////
void OgreSceneNode::setLocalPosition( const float * _position )
{
	m_position = Ogre::Vector3( _position );
	m_sceneNode->setPosition( m_position );
}
//////////////////////////////////////////////////////////////////////////
void OgreSceneNode::setLocalOrient( const float * _orient )
{
	m_localOrient = Ogre::Quaternion( _orient[0], _orient[1], _orient[2], _orient[3] );
	m_sceneNode->setOrientation( m_localOrient );
}
//////////////////////////////////////////////////////////////////////////
void OgreSceneNode::translate( const float * _pos )
{
	m_sceneNode->translate( *(Ogre::Vector3*)_pos );
}
//////////////////////////////////////////////////////////////////////////
void OgreSceneNode::setScale( const float * _scale )
{
	m_sceneNode->setScale( *(Ogre::Vector3*)_scale );
}
//////////////////////////////////////////////////////////////////////////
void OgreSceneNode::setScale( float _scale )
{
	m_sceneNode->setScale( _scale, _scale, _scale );
}
//////////////////////////////////////////////////////////////////////////
void OgreSceneNode::yaw( float _angle )
{
	m_sceneNode->yaw( Ogre::Degree( _angle ) );
}
//////////////////////////////////////////////////////////////////////////
void OgreSceneNode::pitch( float _angle )
{
	m_sceneNode->pitch( Ogre::Degree( _angle ) );
}
//////////////////////////////////////////////////////////////////////////
void OgreSceneNode::roll( float _angle )
{
	m_sceneNode->roll( Ogre::Degree( _angle ) );
}
//////////////////////////////////////////////////////////////////////////
SceneNodeInterface * OgreSceneNode::createChildSceneNode( const char * _name )
{
	Ogre::SceneNode * sceneNode = m_sceneNode->createChildSceneNode( _name );
	return new OgreSceneNode( sceneNode );
}
//////////////////////////////////////////////////////////////////////////
void OgreSceneNode::attachEntity( EntityInterface * _entity )
{
	OgreEntity * ent = static_cast<OgreEntity*>(_entity);
	m_sceneNode->attachObject( ent->m_entity );
}
//////////////////////////////////////////////////////////////////////////