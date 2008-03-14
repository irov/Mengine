#	include "OgreSkeleton.h"
#	include "OgreEntity.h"
//////////////////////////////////////////////////////////////////////////
OgreSkeleton::OgreSkeleton( Ogre::Entity & _entity )
	: m_entity( _entity )
{
}
//////////////////////////////////////////////////////////////////////////
OgreSkeleton::~OgreSkeleton()
{
}
//////////////////////////////////////////////////////////////////////////
/*void OgreSkeleton::enableAnimation( const std::string & _anim, bool _enable )
{
	Ogre::AnimationState * state = m_entity.getAnimationState( _anim );
	state->setEnabled( _enable );
}
//////////////////////////////////////////////////////////////////////////
void OgreSkeleton::advanceAnimation( const std::string & _anim, float _timeOffset )
{
	Ogre::AnimationState * state = m_entity.getAnimationState( _anim );
	state->addTime( _timeOffset );
}
//////////////////////////////////////////////////////////////////////////
void OgreSkeleton::setAnimationWeight( const std::string & _anim, float _weight )
{
	Ogre::AnimationState * state = m_entity.getAnimationState( _anim );
	state->setWeight( _weight );
}*/
//////////////////////////////////////////////////////////////////////////
void OgreSkeleton::attachEntityToBone( const std::string & _bone, EntityInterface * _entity )
{
	m_entity.attachObjectToBone( _bone, static_cast<OgreEntity*>( _entity )->getOgreEntity() );
}
//////////////////////////////////////////////////////////////////////////
/*void OgreSkeleton::advanceAllAnimations( float _timeOffset )
{
	Ogre::AnimationStateSet* pAnimStates = m_entity.getAllAnimationStates();
	Ogre::AnimationStateIterator i = pAnimStates->getAnimationStateIterator();
	
	while( i.hasMoreElements() )
	{
		Ogre::AnimationState* pAnimState = i.getNext();
		
		if ( pAnimState->getEnabled() )
		{
			pAnimState->addTime( _timeOffset );
		}
	}
}*/
//////////////////////////////////////////////////////////////////////////