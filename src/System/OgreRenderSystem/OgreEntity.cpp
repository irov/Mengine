#	include "OgreEntity.h"
#	include "OgreSkeleton.h"
//////////////////////////////////////////////////////////////////////////
OgreEntity::OgreEntity( Ogre::Entity * _entity )
: m_entity( _entity )
, m_skeleton(0)
{
}
//////////////////////////////////////////////////////////////////////////
OgreEntity::~OgreEntity()
{
	delete m_skeleton;
}
//////////////////////////////////////////////////////////////////////////
Ogre::Entity * OgreEntity::getOgreEntity()
{
	return m_entity;
}
//////////////////////////////////////////////////////////////////////////
SkeletonInterface* OgreEntity::getSkeleton() const
{
	if ( m_entity->hasSkeleton() == false )
	{
		return NULL;
	}

	if ( m_skeleton == NULL )
	{
		m_skeleton = new OgreSkeleton( *m_entity );
	}
		
	return m_skeleton;
}
//////////////////////////////////////////////////////////////////////////
void OgreEntity::setCastsShadow( bool _castsShadow )
{
	if ( m_entity )
	{
		m_entity->setCastShadows( _castsShadow );
	}
}
//////////////////////////////////////////////////////////////////////////
void OgreEntity::setVisible( bool _visible )
{
	if ( m_entity )
	{
		m_entity->setVisible( _visible );
	}
}
//////////////////////////////////////////////////////////////////////////
void OgreEntity::setMaterial( const std::string & _material )
{
	if ( m_entity )
	{
		m_entity->setMaterialName( _material );
	}
}
//////////////////////////////////////////////////////////////////////////
void OgreEntity::setSubEntityMaterial( const std::string & _subEntity, const std::string & _material )
{
	if ( m_entity )
	{
		Ogre::SubEntity * ogreSubEntity = m_entity->getSubEntity( _subEntity );

		if ( ogreSubEntity )
		{
            ogreSubEntity->setMaterialName( _material );
		}
	}
}
//////////////////////////////////////////////////////////////////////////
/*bool OgreEntity::hasSkeleton() const 
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
*/