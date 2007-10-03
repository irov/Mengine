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

}
//////////////////////////////////////////////////////////////////////////
bool OgreRenderSystem::init( Ogre::Root * _root, Ogre::RenderWindow * _renderWindow )
{
	m_root = _root;
	m_renderWindow = _renderWindow;

	m_sceneMgr = m_root->createSceneManager( Ogre::ST_GENERIC, "defaultSceneManager" );
	m_renderSys = m_root->getRenderSystem();

	m_spriteMgr = new OgreRenderSpriteManager();

	Ogre::Camera * camera = m_sceneMgr->createCamera("defaultCamera");
 	camera->setNearClipDistance(5);

 	// Create one viewport, entire window
	m_viewport = m_renderWindow->addViewport( camera );
 	// Alter the camera aspect ratio to match the viewport

 	camera->setAspectRatio(m_viewport->getActualWidth() / m_viewport->getActualWidth());

 	// Set default mipmap level (NB some APIs ignore this)
 	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

	m_spriteMgr->init( m_sceneMgr, m_renderSys, m_viewport, Ogre::RENDER_QUEUE_OVERLAY, true);


	return true;
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::render()
{
	m_root->renderOneFrame();
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::setProjectionMatrix(const /*mat4f*/ float * _projection)
{
	m_renderSys->_setProjectionMatrix( *(Ogre::Matrix4 *)_projection );
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::setViewMatrix(const /*mat4f*/ float * _view)
{
	m_renderSys->_setViewMatrix( *(Ogre::Matrix4 *)_view );
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::setWorldMatrix(const /*mat4f*/ float * _world)
{
	m_renderSys->_setWorldMatrix( *(Ogre::Matrix4 *)_world );
}
//////////////////////////////////////////////////////////////////////////
RenderImageInterface* OgreRenderSystem::loadImage( const TextureDesc&	_desc )
{
	return new OgreRenderImage( _desc );
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::releaseImage( RenderImageInterface* _rii )
{
	delete static_cast<OgreRenderImage*>( _rii );
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::renderImage(		
				 const /*mat3*/ float * _transform, 
				 const /*vec2f*/ float * _offset,
				 const /*vec4f*/ float * _uv,
				 const /*vec2f*/ float * _size,
				 unsigned int _mixedColor, 
				 RenderImageInterface* _rii)
{
	OgreRenderImage * image = static_cast<OgreRenderImage *>( _rii );
	Ogre::Texture * texture = image->getTexture();

	m_spriteMgr->spriteBltFull( texture
		, *(Ogre::Matrix3*)_transform
		, *(Ogre::Vector2*)_offset
		, *(Ogre::Vector4*)_uv
		, *(Ogre::Vector2*)_size );
}