#	pragma once

#	include "OgreRenderSystemInterface.h"

#	include "Ogre.h"

class OgreSkeleton;
class OgreRenderSystem;

class OgreEntity
	: public EntityInterface
{
public:
	OgreEntity( OgreRenderSystem* _rendSys, Ogre::Entity * _entity, Ogre::SceneManager* _sceneMgr );
	~OgreEntity();
public:
	SkeletonInterface * getSkeleton() const override;
	void setCastsShadow( bool _castsShadow ) override;
	void setVisible( bool _visible ) override;
	void setMaterial( const std::string & _material ) override;
	void setSubEntityMaterial( const std::string & _subEntity, const std::string & _material ) override;
	void createRenderToTexture( const char* _cameraName, int _width, int _height ) override;
	void getAABB( float * _min, float * _max ) const override;
	float getBoundingRadius() const override;

	void setAnimationEnabled( const char* _animName, bool _enabled ) override;
	bool getAnimationEnabled( const char* _animName ) override;
	void setAnimationTimePosition( const char* _animName, float _timePos ) override;
	float getAnimationTimePosition( const char* _animName ) override;
	void setAnimationLength( const char* _animName, float _length ) override;
	float getAnimationLength( const char* _animName ) override;
	void setAnimationWeight( const char* _animName, float _weight ) override;
	float getAnimationWeigth( const char* _animName ) override;
	void animationAddTime( const char* _animName, float _time ) override;
	bool animationHasEnded( const char* _animName ) override;
	void animationSetLooped( const char* _animName, bool _looped ) override;
	bool animationGetLooped( const char* _animName ) override;

	void attachEntity( const char* _bone, EntityInterface* _entity ) override;
public:
	Ogre::Entity * getOgreEntity();

private:
	Ogre::Entity * m_entity;
	Ogre::SceneManager* m_sceneMgr;
	OgreRenderSystem* m_rendSys;

	mutable OgreSkeleton * m_skeleton;
};
