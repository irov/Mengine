#	include "OgreEntity.h"
#	include "OgreSkeleton.h"
#	include "OgreRenderSystem.h"
#	include "OgreRenderSpriteManager.h"
//////////////////////////////////////////////////////////////////////////
OgreEntity::OgreEntity( OgreRenderSystem* _rendSys, Ogre::Entity * _entity, Ogre::SceneManager* _sceneMgr )
: m_entity( _entity )
, m_skeleton(0)
, m_sceneMgr( _sceneMgr )
, m_rendSys( _rendSys )
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
float OgreEntity::getBoundingRadius() const
{
	return m_entity->getBoundingRadius();
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
	Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton().getByName( Ogre::String("rtt") + Ogre::String( _cameraName ) );
	if( texture.isNull() )
	{
		texture = Ogre::TextureManager::getSingleton().createManual( Ogre::String("rtt") + Ogre::String( _cameraName ), "Default", Ogre::TEX_TYPE_2D,
			_width, _height, 0, 0, Ogre::PF_X8R8G8B8, Ogre::TU_RENDERTARGET, NULL );
	}
	
	Ogre::Camera* rttCam = NULL;
	if( !m_sceneMgr->hasCamera( _cameraName ) )
	{
		rttCam = m_sceneMgr->createCamera( _cameraName );
	}
	else
	{
		rttCam = m_sceneMgr->getCamera( _cameraName );
	}
	Ogre::RenderTarget* rtgt = texture->getBuffer()->getRenderTarget();
	rtgt->removeAllViewports();
	rtgt->addViewport( rttCam )->setBackgroundColour( Ogre::ColourValue::Green );
	rtgt->addListener( m_rendSys->getRenderSpriteManager() );
	//rtgt->setActive( false );
	//texture->getBuffer()->getRenderTarget()->getViewport(0)->setClearEveryFrame( false );
	Ogre::MaterialManager::ResourceCreateOrRetrieveResult mresult = Ogre::MaterialManager::getSingleton().createOrRetrieve( Ogre::String("rttMat") + Ogre::String( _cameraName ), "Default" );
	Ogre::MaterialPtr mat = mresult.first;
	if( mresult.second )
	{
		Ogre::TextureUnitState* t = mat->getTechnique(0)->getPass(0)->createTextureUnitState( Ogre::String("rtt") + Ogre::String( _cameraName ) );
		t->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
	}
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
void OgreEntity::setAnimationEnabled( const char* _animName, bool _enabled )
{
	m_entity->getAnimationState( _animName )->setEnabled( _enabled );
}
//////////////////////////////////////////////////////////////////////////
bool OgreEntity::getAnimationEnabled( const char* _animName )
{
	return m_entity->getAnimationState( _animName )->getEnabled();
}
//////////////////////////////////////////////////////////////////////////
void OgreEntity::setAnimationTimePosition( const char* _animName, float _timePos )
{
	m_entity->getAnimationState( _animName )->setTimePosition( _timePos );
}
//////////////////////////////////////////////////////////////////////////
float OgreEntity::getAnimationTimePosition( const char* _animName )
{
	return m_entity->getAnimationState( _animName )->getTimePosition();
}
//////////////////////////////////////////////////////////////////////////
void OgreEntity::setAnimationLength( const char* _animName, float _length )
{
	m_entity->getAnimationState( _animName )->setLength( _length );
}
//////////////////////////////////////////////////////////////////////////
float OgreEntity::getAnimationLength( const char* _animName )
{
	return m_entity->getAnimationState( _animName )->getLength();
}
//////////////////////////////////////////////////////////////////////////
void OgreEntity::setAnimationWeight( const char* _animName, float _weight )
{
	m_entity->getAnimationState( _animName )->setWeight( _weight );
}
//////////////////////////////////////////////////////////////////////////
float OgreEntity::getAnimationWeigth( const char* _animName )
{
	return m_entity->getAnimationState( _animName )->getWeight();
}
//////////////////////////////////////////////////////////////////////////
void OgreEntity::animationAddTime( const char* _animName, float _time )
{
	m_entity->getAnimationState( _animName )->addTime( _time / 1000.0f );
}
//////////////////////////////////////////////////////////////////////////
bool OgreEntity::animationHasEnded( const char* _animName )
{
	return m_entity->getAnimationState( _animName )->hasEnded();
}
//////////////////////////////////////////////////////////////////////////
void OgreEntity::animationSetLooped( const char* _animName, bool _looped )
{
	m_entity->getAnimationState( _animName )->setLoop( _looped );
}
//////////////////////////////////////////////////////////////////////////
bool OgreEntity::animationGetLooped( const char* _animName )
{
	return m_entity->getAnimationState( _animName )->getLoop();
}
//////////////////////////////////////////////////////////////////////////