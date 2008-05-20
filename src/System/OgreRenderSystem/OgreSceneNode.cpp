#	include "OgreSceneNode.h"
#	include "OgreEntity.h"
#	include "OgreLight.h"
#	include "OgreCamera.h"
//////////////////////////////////////////////////////////////////////////
OgreSceneNode::OgreSceneNode(Ogre::SceneNode * _node, OgreSceneNode * _parent = 0)
: m_sceneNode(_node)
, m_parentNode(_parent)
{

}
//////////////////////////////////////////////////////////////////////////
OgreSceneNode::~OgreSceneNode()
{
	if ( m_parentNode )
	{
		m_parentNode->detach( this );
		m_parentNode = 0;
	}
/*
	for each( LightInterface * light in m_lights )
	{
		OgreLight * pObj = static_cast<OgreLight*>(light);
		m_sceneNode->detachObject( pObj->getOgreLight() );
		delete light;
	}

	m_lights.clear();*/
}
//////////////////////////////////////////////////////////////////////////
Ogre::SceneNode * OgreSceneNode::getOgreSceneNode()
{
	return m_sceneNode;
}
//////////////////////////////////////////////////////////////////////////
void OgreSceneNode::detach( SceneNodeInterface * _node )
{
	OgreSceneNode * ogreChild = static_cast<OgreSceneNode*>(_node);
	Ogre::SceneNode * child = ogreChild->getOgreSceneNode();
	const std::string & name = child->getName();
	m_sceneNode->removeChild( name );	

	ogreChild->m_parentNode = NULL;
}
//////////////////////////////////////////////////////////////////////////
const float * OgreSceneNode::getWorldOrient()
{
	return m_sceneNode->getWorldOrientation().ptr();
}
//////////////////////////////////////////////////////////////////////////
const float * OgreSceneNode::getWorldPosition()
{
	return m_sceneNode->getWorldPosition().ptr();
}
//////////////////////////////////////////////////////////////////////////
float * OgreSceneNode::getLocalPosition()
{
	return (float*)m_sceneNode->getPosition().ptr();
}
//////////////////////////////////////////////////////////////////////////
float * OgreSceneNode::getLocalOrient()
{
	return (float*)m_sceneNode->getOrientation().ptr();
}
//////////////////////////////////////////////////////////////////////////
void OgreSceneNode::setLocalPosition( const float * _position )
{
	m_sceneNode->setPosition( 
		Ogre::Vector3( _position ) );
}
//////////////////////////////////////////////////////////////////////////
void OgreSceneNode::setLocalOrient( const float * _orient )
{
	m_sceneNode->setOrientation( 
		Ogre::Quaternion( _orient[0], _orient[1], _orient[2], _orient[3] ) );
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
void OgreSceneNode::attachEntity( EntityInterface * _entity )
{
	OgreEntity * ent = static_cast<OgreEntity*>(_entity);
	m_sceneNode->attachObject( ent->getOgreEntity() );
}
//////////////////////////////////////////////////////////////////////////
void OgreSceneNode::attachLight( LightInterface * _light )
{
	OgreLight * ogreLight = static_cast<OgreLight*>( _light );
	m_sceneNode->attachObject( ogreLight->getOgreLight() );
	m_lights.push_back( _light );
}
//////////////////////////////////////////////////////////////////////////
void OgreSceneNode::attachCamera( CameraInterface * _camera )
{
	Ogre3dCamera * ogreCam = static_cast<Ogre3dCamera*>( _camera );
	m_sceneNode->attachObject( ogreCam->getOgreCamera() );
//	m_lights.push_back( _light );
}
//////////////////////////////////////////////////////////////////////////
void OgreSceneNode::addChild( SceneNodeInterface * _node )
{
	OgreSceneNode * ogreSceneNode = static_cast<OgreSceneNode*>( _node );

	OgreSceneNode * otherParent = ogreSceneNode->m_parentNode;

	if ( otherParent )
	{
		otherParent->detach( ogreSceneNode );
	}

	Ogre::SceneNode * child = ogreSceneNode->m_sceneNode;

	Ogre::SceneNode * parent = static_cast<Ogre::SceneNode*>( child->getParent() );

	if ( parent && parent != m_sceneNode )
	{
		parent->removeChild( child );
	}

	if ( parent != m_sceneNode )
	{
		m_sceneNode->addChild( child );
	}
	
	ogreSceneNode->m_parentNode = this;
}
//////////////////////////////////////////////////////////////////////////
SceneNodeInterface * OgreSceneNode::createChildSceneNode( const char * _name )
{
	Ogre::SceneNode * ogreSceneNode = m_sceneNode->createChildSceneNode( _name );
	return new OgreSceneNode(ogreSceneNode, this);
}
//////////////////////////////////////////////////////////////////////////
void OgreSceneNode::setFixedYawAxis( bool _fixed )
{
	m_sceneNode->setFixedYawAxis( _fixed );
}
//////////////////////////////////////////////////////////////////////////