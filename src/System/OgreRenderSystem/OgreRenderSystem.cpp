#	include "OgreRenderSystem.h"

#	include "OgreRenderSpriteManager.h"
#	include "OgreRenderImage.h"
#	include "OgreImageCodec.h"

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
	if( m_spriteMgr )
	{
		m_spriteMgr->end();
		delete m_spriteMgr;
	}
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

	//Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton().createManual("Shot", "Default", Ogre::TEX_TYPE_2D, 1024, 768, 0, Ogre::PF_X8R8G8B8, Ogre::TU_RENDERTARGET);
	
	//rt = texture->getBuffer()->getRenderTarget();
	//rt->addViewport(m_sceneMgr->createCamera("ShotCam"));

	return true;
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::setContentResolution( const float * _resolution )
{
	m_contentResolution.x = _resolution[0];
	m_contentResolution.y = _resolution[1];
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::render( RenderImageInterface* _outImage, const char* _imageName, const int* rect )
{
	Ogre::Rect wrect( 0, 0, m_renderWindow->getWidth(), m_renderWindow->getHeight() );
	if(rect)
	{
		wrect.left = rect[0];
		wrect.top = rect[1];
		wrect.right = rect[2];
		wrect.bottom = rect[3];
	}

	OgreRenderImage* shotImage = new OgreRenderImage( _imageName, wrect.width(), wrect.height() );

	//Ogre::TextureManager::getSingleton().getByName( _imageName );

	Ogre::TexturePtr rtt = Ogre::TextureManager::getSingleton().createManual("__shot__", "Default", Ogre::TEX_TYPE_2D, m_renderWindow->getWidth(),
		 m_renderWindow->getHeight(), 1, 0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET);

	Ogre::RenderTarget* rtgt = rtt->getBuffer()->getRenderTarget();

	
	Ogre::Root::getSingleton().renderOneFrame();

	//rtgt->writeContentsToFile("shot.png");

	Ogre::HardwarePixelBufferSharedPtr pixb = rtt->getBuffer();
	
	Ogre::Image::Box imagebox( wrect.left, wrect.top, wrect.right, wrect.bottom );

	unsigned char* data = new unsigned char[ rtt->getBuffer()->getSizeInBytes() ];

	Ogre::PixelBox pb( wrect.width(), wrect.height(), 1, rtt->getFormat(), data );
	rtt->getBuffer()->blitToMemory( imagebox, pb );
	shotImage->m_texture->getBuffer()->blitFromMemory(pb);

	m_renderSys->detachRenderTarget(rtgt->getName());
	Ogre::TextureManager::getSingleton().remove("__shot__");

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
	if( const OgreRenderImage * image = static_cast<const OgreRenderImage *>( _image ) )
	{
		Ogre::Texture * texture = image->getTexture();
		float z = m_spriteMgr->getCurrentZ();
		m_spriteMgr->addQuad1(m_contentResolution,*(Ogre::Vector4*)_uv,*(Ogre::Matrix3*)_transform,*(Ogre::Vector2*)_offset,*(Ogre::Vector2*)_size, z,image, _color);
	}
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::renderImage(		
		const float * _transform, 
		const float * _a,
		const float * _b,
		const float * _c,
		const float * _d,
		const float * _uv,
		unsigned int _color, 
		const RenderImageInterface * _image)
{
	if( const OgreRenderImage * image = static_cast<const OgreRenderImage *>( _image ) )
	{
		Ogre::Texture * texture = image->getTexture();
		float z = m_spriteMgr->getCurrentZ();
		m_spriteMgr->addQuad2(
			m_contentResolution,
			*(Ogre::Vector4*)_uv,
			*(Ogre::Matrix3*)_transform,
			*(Ogre::Vector2*)_a,
			*(Ogre::Vector2*)_b,
			*(Ogre::Vector2*)_c,
			*(Ogre::Vector2*)_d, 
			z,
			image, 
			_color);
	}
}

//////////////////////////////////////////////////////////////////////////
void	OgreRenderSystem::beginLayer()
{
}
//////////////////////////////////////////////////////////////////////////
void	OgreRenderSystem::endLayer()
{
	m_spriteMgr->diffZ();
}
//////////////////////////////////////////////////////////////////////////
