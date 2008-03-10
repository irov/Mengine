#	pragma once

#	include "OgreRenderSystemInterface.h"

#	include "Ogre.h"

class OgreSkeleton;

class OgreEntity
	: public EntityInterface
{
public:
	OgreEntity( Ogre::Entity * _entity, Ogre::SceneManager* _sceneMgr );
	~OgreEntity();
public:
	SkeletonInterface * getSkeleton() const override;
	void setCastsShadow( bool _castsShadow ) override;
	void setVisible( bool _visible ) override;
	void setMaterial( const std::string & _material ) override;
	void setSubEntityMaterial( const std::string & _subEntity, const std::string & _material ) override;
	void createRenderToTexture( const char* _cameraName, int _width, int _height ) override;
	void getAABB( float * _min, float * _max ) const override;

public:
	Ogre::Entity * getOgreEntity();

private:
	Ogre::Entity * m_entity;
	Ogre::SceneManager* m_sceneMgr;

	mutable OgreSkeleton * m_skeleton;
};
