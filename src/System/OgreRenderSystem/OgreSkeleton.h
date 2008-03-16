#	pragma once

#	include "OgreRenderSystemInterface.h"

#	include "Ogre.h"

class OgreSkeleton : public SkeletonInterface
{
public:
	OgreSkeleton( Ogre::Entity & _entity );
	~OgreSkeleton();
public:
	//void enableAnimation( const std::string & _anim, bool _enable ) override;
	//void advanceAnimation( const std::string & _anim, float _timeDelta ) override;
	//void advanceAllAnimations( float _timeDelta ) override;
	//void setAnimationWeight( const std::string & _anim, float _weight ) override;
//	void attachEntityToBone( const std::string & _bone, EntityInterface * _entity ) override;
private:
	Ogre::Entity & m_entity;
};