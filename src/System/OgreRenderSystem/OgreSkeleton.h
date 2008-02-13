#	pragma once

#	include "OgreRenderSystemInterface.h"

#	include "Ogre.h"

class OgreSkeleton : public SkeletonInterface
{
public:
	OgreSkeleton( Ogre::Entity & _entity );
	~OgreSkeleton();
public:
	void enableAnimation( const std::string & _anim, bool _enable );
	void advanceAnimation( const std::string & _anim, float _timeDelta );
	void advanceAllAnimations( float _timeDelta );
	void setAnimationWeight( const std::string & _anim, float _weight );
	void attachEntityToBone( const std::string & _bone, EntityInterface * _entity );
private:
	Ogre::Entity & m_entity;
};