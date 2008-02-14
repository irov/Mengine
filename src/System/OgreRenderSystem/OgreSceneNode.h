#	pragma once

#	include "OgreRenderSystemInterface.h"

#	include "Ogre.h"

class	OgreSceneNode : public SceneNodeInterface
{
public:
	OgreSceneNode( Ogre::SceneNode * _node, OgreSceneNode * _parent );
	~OgreSceneNode();
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
	void attachEntity( EntityInterface * _entity ) override;
	void attachLight( LightInterface * _light ) override;
	void addChild( SceneNodeInterface * _node ) override;

public:

	Ogre::SceneNode * getOgreSceneNode();

private:

	void detach( SceneNodeInterface * _node );

	Ogre::SceneNode * m_sceneNode;

	OgreSceneNode * m_parentNode;

	std::list<LightInterface*> m_lights;
};