#	include "OgreEntity.h"
//////////////////////////////////////////////////////////////////////////
OgreEntity::OgreEntity( Ogre::Entity * _entity )
: m_entity( _entity )
, m_animState0(0)
, m_animState1(0)
{
}
//////////////////////////////////////////////////////////////////////////
OgreEntity::~OgreEntity()
{
}
//////////////////////////////////////////////////////////////////////////
bool OgreEntity::hasSkeleton() const 
{
	Ogre::MeshPtr mesh = m_entity->getMesh();
	return mesh->hasSkeleton();
}
//////////////////////////////////////////////////////////////////////////
const float * OgreEntity::getBoneOrientation( const char * _name )
{
	Ogre::MeshPtr mesh = m_entity->getMesh();
	Ogre::SkeletonPtr skeleton = mesh->getSkeleton();
	Ogre::Bone * b = skeleton->getBone( _name );
	Ogre::Quaternion q = b->getOrientation();
	return q.ptr();
}
//////////////////////////////////////////////////////////////////////////
const float * OgreEntity::getBonePosition( const char * _name )
{
	Ogre::MeshPtr mesh = m_entity->getMesh();
	Ogre::SkeletonPtr skeleton = mesh->getSkeleton();
	Ogre::Bone * b = skeleton->getBone( _name );
	Ogre::Vector3 p = b->getPosition();
	return p.ptr();
}
const char * OgreEntity::getName() const
{
	Ogre::MeshPtr mesh = m_entity->getMesh();
	//mesh
	return 0;
}
//////////////////////////////////////////////////////////////////////////
void OgreEntity::play( const char * _name )
{
	m_animState0 = m_entity->getAnimationState( _name );
	m_animState0->setTimePosition( 0 );
	m_animState0->setLoop( false );
	m_animState0->setEnabled( true );
}
//////////////////////////////////////////////////////////////////////////
void OgreEntity::play( 
	const char * _name1, float _weight1, 
	const char * _name2, float _weight2 )
{
	m_animState0 = m_entity->getAnimationState( _name1 );
	m_animState0->setTimePosition( 0 );
	m_animState0->setLoop( false );
	m_animState0->setWeight( _weight1 );

	m_animState1 = m_entity->getAnimationState( _name2 );
	m_animState1->setTimePosition( 0 );
	m_animState1->setLoop( false );
	m_animState1->setWeight( _weight2 );
}
//////////////////////////////////////////////////////////////////////////
void OgreEntity::stop()
{
	m_animState0->setEnabled( false );
}
//////////////////////////////////////////////////////////////////////////
void OgreEntity::setLooped( bool _looped )
{
	m_animState0->setLoop( _looped );
}
//////////////////////////////////////////////////////////////////////////
void OgreEntity::update( float _timing )
{
	if(m_animState0 != NULL )
	{
		m_animState0->addTime(_timing/ 1000.f);

		if(m_animState1 != NULL )
		{
			m_animState1->addTime(_timing / 1000.f);
		}
	}
}
//////////////////////////////////////////////////////////////////////////