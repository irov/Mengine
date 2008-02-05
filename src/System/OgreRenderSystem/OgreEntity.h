#	pragma once

#	include "OgreRenderSystemInterface.h"

#	include "Ogre.h"

class OgreEntity
	: public EntityInterface
{
public:
	OgreEntity( Ogre::Entity * _entity );
	~OgreEntity();
public:

	bool hasSkeleton() const override;
	const float * getBoneOrientation( const char * _name ) override;
	const float * getBonePosition( const char * _name ) override;
	void play( const char * _name ) override;
	void play( 
		const char * _name1, float _weight1, 
		const char * _name2, float _weight2 ) override;
	void stop() override;
	void setLooped( bool _looped ) override;
	const char * getName() const override;
	void update( float _timing ) override;

	Ogre::Entity * m_entity;

	Ogre::RaySceneQuery * mRaySceneQuery;

private:

	Ogre::AnimationState * m_animState0;
	Ogre::AnimationState * m_animState1;
	Ogre::AnimationState * m_currentState;
};
