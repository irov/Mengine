#	include "OgreRenderSystem.h"

#	include "OgreRenderSpriteManager.h"
#	include "OgreRenderImage.h"
#	include "OgreRenderFont.h"

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

	Ogre::Camera * camera = m_sceneMgr->createCamera("defaultCamera");
 	camera->setNearClipDistance(5);

 	m_viewport = m_renderWindow->addViewport( camera );
 	
 	camera->setAspectRatio(m_viewport->getActualWidth() / m_viewport->getActualHeight());

 	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

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
void OgreRenderSystem::releaseImage( RenderImageInterface* _rii )
{
	delete static_cast<OgreRenderImage*>( _rii );
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::renderImage(		
				 const float * _transform, 
				 const float * _offset,
				 const float * _uv,
				 const float * _size,
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
//////////////////////////////////////////////////////////////////////////
RenderFontInterface* OgreRenderSystem::loadFont( const FontDesc&	_desc )
{
	return new OgreRenderFont(_desc.name);
}
//////////////////////////////////////////////////////////////////////////
void	OgreRenderSystem::releaseRenderFont( RenderFontInterface* _fnt )
{
	delete static_cast<OgreRenderFont*>(_fnt);
}
//////////////////////////////////////////////////////////////////////////
void	OgreRenderSystem::renderText(const float * _pos, RenderFontInterface* _font, const char * _text)
{
	float width = m_viewport->getActualWidth();
	float heigth = m_viewport->getActualHeight();
	OgreRenderFont * font = static_cast<OgreRenderFont *>( _font );	
	Ogre::Font * fnt = font->getFont();
	m_spriteMgr->printText(fnt, 2 * _pos[0]/width - 1, -(_pos[1]/heigth*2 - 1) , _text );
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