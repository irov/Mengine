#	include "OgreEntity.h"
#	include "OgreSkeleton.h"
//////////////////////////////////////////////////////////////////////////
OgreEntity::OgreEntity( Ogre::Entity * _entity, Ogre::SceneManager* _sceneMgr )
: m_entity( _entity )
, m_skeleton(0)
, m_sceneMgr( _sceneMgr )
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
SkeletonInterface * OgreEntity::getSkeleton() const
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
void OgreEntity::createRenderToTexture( const char* _cameraName, int _width, int _height )
{
	Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton().createManual( Ogre::String("rtt") + Ogre::String( _cameraName ), "Default", Ogre::TEX_TYPE_2D,
		_width, _height, 0, 0, Ogre::PF_X8R8G8B8, Ogre::TU_RENDERTARGET, NULL );
	Ogre::Camera* rttCam = NULL;
	if( !m_sceneMgr->hasCamera( _cameraName ) )
	{
		rttCam = m_sceneMgr->createCamera( _cameraName );
	}
	else
	{
		rttCam = m_sceneMgr->getCamera( _cameraName );
	}
	texture->getBuffer()->getRenderTarget()->addViewport( rttCam );
	texture->getBuffer()->getRenderTarget()->setActive( false );
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create( Ogre::String("rttMat") + Ogre::String( _cameraName ), "Default" );
	Ogre::TextureUnitState* t = mat->getTechnique(0)->getPass(0)->createTextureUnitState( Ogre::String("rtt") + Ogre::String( _cameraName ) );
	t->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
	m_entity->setMaterialName( Ogre::String("rttMat") + Ogre::String( _cameraName ) );
}
//////////////////////////////////////////////////////////////////////////
void OgreEntity::getAABB( float * _min, float * _max ) const
{
	const Ogre::AxisAlignedBox & aabb = m_entity->getWorldBoundingBox();
	_min[0] = aabb.getMinimum().x;
	_min[1] = aabb.getMinimum().y;
	_min[2] = aabb.getMinimum().z;

	_max[0] = aabb.getMaximum().x;
	_max[1] = aabb.getMaximum().y;
	_max[2] = aabb.getMaximum().z;
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