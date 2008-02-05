#	pragma once

#	include "OgreRenderSystemInterface.h"

#	include "Ogre.h"

class	OgreSceneNode : public SceneNodeInterface
{
public:
	OgreSceneNode(Ogre::SceneNode * _node);
public:
	const float * getWorldOrient() override;
	const float * getWorldPosition() override;
	float * getLocalPosition() override;
	float * getLocalOrient() override;
	void setLocalPosition( const float * _position ) override;
	void setLocalOrient( const float * _orient ) override;
	void translate( const float * _pos ) override;
	void setScale( const float * _scale ) override;
	void setScale( float _scale ) override;
	void yaw( float _angle ) override;
	void pitch( float _angle ) override;
	void roll( float _angle ) override;
	SceneNodeInterface * createChildSceneNode( const char * _name ) override;
	void attachEntity( EntityInterface * _entity ) override;

private:
	Ogre::SceneNode * m_sceneNode;
	Ogre::Vector3 m_position;
	Ogre::Quaternion m_localOrient;
};