#	include "OgreRenderSystem.h"

#	include "OgreRenderSpriteManager.h"
#	include "OgreRenderImage.h"

//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem( RenderSystemInterface ** _ptrInterface )
{
	try
	{
		*_ptrInterface = new OgreRenderSystem();
	}
	catch (...)
	{
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
void releaseInterfaceSystem( RenderSystemInterface* _ptrInterface )
{
	delete static_cast<OgreRenderSystem*>(_ptrInterface);
}
//////////////////////////////////////////////////////////////////////////
OgreRenderSystem::OgreRenderSystem()
	: m_root(0)
	, m_renderWindow(0)
	, m_viewport(0)
	, m_sceneMgr(0)
	, m_spriteMgr(0)
{
}
//////////////////////////////////////////////////////////////////////////
OgreRenderSystem::~OgreRenderSystem()
{
	m_spriteMgr->end();
	delete m_spriteMgr;
}
//////////////////////////////////////////////////////////////////////////
bool OgreRenderSystem::init( Ogre::Root * _root, Ogre::RenderWindow * _renderWindow )
{
	m_root = _root;
	m_renderWindow = _renderWindow;

	m_sceneMgr = m_root->createSceneManager( Ogre::ST_GENERIC, "defaultSceneManager" );
	m_renderSys = m_root->getRenderSystem();

	Ogre::Camera * camera = m_sceneMgr->createCamera("defaultCamera");
 	camera->setNearClipDistance(5);

 	m_viewport = m_renderWindow->addViewport( camera );
 	
	float width = m_viewport->getActualWidth();
	float height = m_viewport->getActualHeight();
	float aspect = width / height;

 	camera->setAspectRatio( aspect );

 	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(0);

	m_spriteMgr = new OgreRenderSpriteManager();
	m_spriteMgr->init( m_sceneMgr, m_renderSys, m_viewport, Ogre::RENDER_QUEUE_OVERLAY, true);

	return true;
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::render()
{
	//m_renderWindow->update();
	//m_root->renderOneFrame();
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::setProjectionMatrix( const float * _projection )
{
	m_renderSys->_setProjectionMatrix( *(Ogre::Matrix4 *)_projection );
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::setViewMatrix( const float * _view )
{
	m_renderSys->_setViewMatrix( *(Ogre::Matrix4 *)_view );
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::setWorldMatrix( const float * _world )
{
	m_renderSys->_setWorldMatrix( *(Ogre::Matrix4 *)_world );
}
//////////////////////////////////////////////////////////////////////////
RenderImageInterface* OgreRenderSystem::loadImage( const TextureDesc&	_desc )
{
	return new OgreRenderImage( _desc );
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::releaseImage( RenderImageInterface* _image )
{
	delete static_cast<OgreRenderImage*>( _image );
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::renderImage(		
				 const float * _transform, 
				 const float * _offset,
				 const float * _uv,
				 const float * _size,
				 unsigned int _color, 
				 const RenderImageInterface* _image)
{
	const OgreRenderImage * image = static_cast<const OgreRenderImage *>( _image );
	Ogre::Texture * texture = image->getTexture();

	m_spriteMgr->spriteBltFull( texture
		, *(Ogre::Matrix3*)_transform
		, *(Ogre::Vector2*)_offset
		, *(Ogre::Vector4*)_uv
		, *(Ogre::Vector2*)_size
		, _color );
}
//////////////////////////////////////////////////////////////////////////
void	OgreRenderSystem::beginLayer()
{
	m_spriteMgr->beginLayer();
}
//////////////////////////////////////////////////////////////////////////
void	OgreRenderSystem::endLayer()
{
	m_spriteMgr->endLayer();
}
//////////////////////////////////////////////////////////////////////////
