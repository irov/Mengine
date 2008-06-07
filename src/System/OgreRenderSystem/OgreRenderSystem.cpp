#	include "OgreRenderSystem.h"

#	include "OgreRenderSpriteManager.h"
#	include "OgreRenderImage.h"

#	include "OgreCamera.h"
#	include "OgreEntity.h"
#	include "OgreLight.h"
#	include "OgreSceneNode.h"


#	include "OgreRenderVideoStream.h"
#	include "OgreExternalTextureSourceManager.h"

#	include "Config/Config.h"


#if	RENDER_SYSTEM == RS_D3D9
	#include "OgreD3D9Plugin.h"
	#ifdef _DEBUG
		#pragma comment( lib, "RenderSystem_Direct3D9Static_d.lib" )
	#else
		#pragma comment( lib, "RenderSystem_Direct3D9Static.lib" )
	#endif
#elif RENDER_SYSTEM == RS_D3D7
	#include "OgreD3D7Plugin.h"
	#ifdef _DEBUG
		#pragma comment( lib, "RenderSystem_Direct3D7Static_d.lib" )
	#else
		#pragma comment( lib, "RenderSystem_Direct3D7Static.lib" )
	#endif
#elif RENDER_SYSTEM == RS_OGL
	#include "OgreGLPlugin.h"
	#ifdef _DEBUG
		#pragma comment( lib, "RenderSystem_GLStatic_d.lib" )
	#else
		#pragma comment( lib, "RenderSystem_GLStatic.lib" )
	#endif
#endif

/*#include "CEGUI/CEGUISystem.h"
#include "CEGUI/CEGUILogger.h"
#include "CEGUI/CEGUISchemeManager.h"
#include "CEGUI/CEGUIWindowManager.h"
#include "CEGUI/CEGUIWindow.h"
#include "OgreCEGUIRenderer.h"*/

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
	, m_GUIRenderer(0)
	, m_GUISystem(0)
	, m_rootSceneNode(0)
	, m_eventListener( NULL )
	, m_renderPlugin( NULL )
	, m_pixelTexture( NULL )
{
}
//////////////////////////////////////////////////////////////////////////
OgreRenderSystem::~OgreRenderSystem()
{

	delete m_rootSceneNode;

	// destroy RenderSystem depended objects
	m_sceneMgr->destroyAllCameras();
	// unload all resources
	Ogre::ResourceGroupManager::getSingleton().shutdownAll();

	if( m_renderWindow )
	{
		m_renderWindow->removeListener( m_spriteMgr );
	}

	if( m_spriteMgr )
	{
		m_spriteMgr->end();
		delete m_spriteMgr;
	}

	//if(m_GUISystem)
	//{
	//	delete m_GUISystem;
	//	m_GUISystem = 0;
	//}

	//if(m_GUIRenderer)
	//{
	//	delete m_GUIRenderer;
	//	m_GUIRenderer = 0;
	//}

	if( m_pixelTexture )
	{
		delete m_pixelTexture;
		m_pixelTexture = NULL;
	}
	if( m_renderPlugin )
	{
		m_root->uninstallPlugin( m_renderPlugin );
		delete m_renderPlugin;
	}

	Ogre::Root* root = Ogre::Root::getSingletonPtr();
	delete root;
}
//////////////////////////////////////////////////////////////////////////
CameraInterface * OgreRenderSystem::createCamera(const char * _name)
{
	/*Ogre::Camera * camera = m_sceneMgr->createCamera(_name);
	CameraInterface * ogre3dcam = new Ogre3dCamera( camera );
	m_renderWindow->removeViewport(0);
	m_viewport = m_renderWindow->addViewport( camera );*/
	CameraInterface* ogre3dcam = new Ogre3dCamera( m_sceneMgr->getCamera("defaultCamera") );
	return  ogre3dcam;
}
//////////////////////////////////////////////////////////////////////////
SceneNodeInterface * OgreRenderSystem::createSceneNode( const std::string & _name )
{
	//Ogre::SceneNode * sceneNode = m_sceneMgr->getRootSceneNode()->createChildSceneNode( _name );
	Ogre::SceneNode * sceneNode = m_sceneMgr->getRootSceneNode()->createChildSceneNode();

	OgreSceneNode * ogreNode = new OgreSceneNode( sceneNode, m_rootSceneNode );
	//m_rootSceneNode->addChild( ogreNode ); //не имеет смысла, т.к. на уровне root level - 1 все и так удалитс€

	return ogreNode;
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::releaseSceneNode( SceneNodeInterface * _interface )
{
	OgreSceneNode * node = static_cast<OgreSceneNode*>(_interface);

	if( node == NULL )
	{
		return;
	}

	Ogre::SceneNode * sceneNode = node->getOgreSceneNode();

	delete node;

	if ( sceneNode )
	{
		if ( sceneNode->getParent() )
		{
			sceneNode->getParent()->removeChild( sceneNode );
		}

		sceneNode->detachAllObjects();
		sceneNode->removeAndDestroyAllChildren(); 
		
		m_sceneMgr->destroySceneNode( sceneNode->getName() );
		sceneNode = 0;
	}
} 
//////////////////////////////////////////////////////////////////////////
LightInterface * OgreRenderSystem::createLight( const char * _name )
{
	Ogre::Light * light = m_sceneMgr->createLight(_name);
	LightInterface * ogreLight = new OgreLight(light);
	return ogreLight;   
}
//////////////////////////////////////////////////////////////////////////
EntityInterface * OgreRenderSystem::createEntity( const char * _name, const char * _mesh )
{
	Ogre::Entity * entity = m_sceneMgr->createEntity( _name, _mesh );
	EntityInterface * ent = new OgreEntity( this, entity, m_sceneMgr );
	return ent;
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::releaseEntity( EntityInterface * _entity )
{
	Ogre::Entity * entity = static_cast<OgreEntity*>(_entity)->getOgreEntity();

	#if OGRE_VERSION_MINOR >= 1 
		m_sceneMgr->destroyEntity( entity );
	#else
		m_sceneMgr->removeEntity( entity );
	#endif

	delete static_cast<EntityInterface*>( _entity );
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::releaseLight( LightInterface * _light )
{
	Ogre::Light * light = static_cast<OgreLight*>(_light)->getOgreLight();

	#if OGRE_VERSION_MINOR >= 1 
		m_sceneMgr->destroyLight( light );
	#else
		m_sceneMgr->removeLight( light );
	#endif

	delete static_cast<LightInterface*>( _light );
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::releaseCamera( CameraInterface * _camera )
{
	delete static_cast<CameraInterface*>( _camera );
}
//////////////////////////////////////////////////////////////////////////
SceneNodeInterface * OgreRenderSystem::getRootSceneNode() const
{
	return m_rootSceneNode;
}
//////////////////////////////////////////////////////////////////////////
bool OgreRenderSystem::initialize( Menge::LogSystemInterface* _logSystem )
{
	m_root = new Ogre::Root( "","", "Ogre.log" );
	Ogre::LogManager::getSingleton().setLogDetail( Ogre::LL_LOW );
#if	RENDER_SYSTEM == RS_D3D9
	m_renderPlugin = new Ogre::D3D9Plugin();
#elif	RENDER_SYSTEM == RS_D3D7
	m_renderPlugin = new Ogre::D3D7Plugin();
#elif RENDER_SYSTEM == RS_OGL
	m_renderPlugin = new Ogre::GLPlugin();
#endif

	m_root->installPlugin( m_renderPlugin );
	//m_root->loadPlugin( _driver );

	m_renderSys = m_root->getAvailableRenderers()->at( 0 );
	m_root->setRenderSystem( m_renderSys );
	m_root->initialise( false );
	
	m_renderSys->addListener( this );

	m_pixelTexture = new OgreRenderImage();
	return true;
}
//////////////////////////////////////////////////////////////////////////
bool OgreRenderSystem::createRenderWindow( float _width, float _height, int _bits, bool _fullscreen, WINDOW_HANDLE _winHandle,
											int _FSAAType, int _FSAAQuality )
{
	Ogre::NameValuePairList params;
	params.insert( std::make_pair("Colour Depth", Ogre::StringConverter::toString( _bits ) ) );
	//params.insert( std::make_pair("vsync", Ogre::StringConverter::toString( true ) ) );
	params.insert( std::make_pair( "externalWindowHandle", Ogre::StringConverter::toString( ( (unsigned int)_winHandle)  ) ) );
	params.insert( std::make_pair( "FSAA", Ogre::StringConverter::toString( _FSAAType ) ) );
	params.insert( std::make_pair( "FSAAQuality", Ogre::StringConverter::toString( _FSAAQuality ) ) );

	m_renderWindow = m_root->createRenderWindow( "Menge", _width, _height, _fullscreen, &params );

	m_sceneMgr = m_root->createSceneManager( Ogre::ST_GENERIC, "defaultSceneManager" );
	Ogre::Camera* sceneCam = m_sceneMgr->createCamera("defaultCamera");
	//sceneCam->setNearClipDistance( 5 );
	m_viewport = m_renderWindow->addViewport( sceneCam );

	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(0);
	m_spriteMgr = new OgreRenderSpriteManager();
	m_spriteMgr->init( m_sceneMgr, m_renderSys, m_viewport, Ogre::RENDER_QUEUE_OVERLAY, true);
	m_renderWindow->addListener( m_spriteMgr );
	//sceneCam->setPosition( 0.0f, 340.0f, 0.01f );
	//sceneCam->lookAt( 0.0f, 0.0f, 0.0f );
	/*sceneCam->setPosition( 0.0f, 10.0f, 10.0f );
	sceneCam->lookAt( 0.0f, 0.0f, 0.0f );
	sceneCam->setFarClipDistance( 1000.0f );
	sceneCam->setNearClipDistance( 0.2f );*/
	//sceneCam->setFOVy(90);
#ifdef _DEBUG
	m_viewport->setBackgroundColour( Ogre::ColourValue::Blue );
#endif
	//m_rootSceneNode = new OgreSceneNode( m_sceneMgr->getRootSceneNode(), 0 );*/

	// setup GUI system
	/*m_GUIRenderer = new CEGUI::OgreCEGUIRenderer(m_renderWindow, Ogre::RENDER_QUEUE_OVERLAY, false, 3000, m_sceneMgr);
	m_GUISystem = new CEGUI::System(m_GUIRenderer);
	CEGUI::Logger::getSingleton().setLoggingLevel(CEGUI::Informative);

	// load scheme and set up defaults
	CEGUI::SchemeManager::getSingleton().loadScheme( (CEGUI::utf8*)"TaharezLookSkin.scheme" );
	m_GUISystem->setDefaultMouseCursor( (CEGUI::utf8*)"TaharezLook", (CEGUI::utf8*)"MouseArrow");
	m_GUISystem->setDefaultFont((CEGUI::utf8*)"BlueHighway-12");

	CEGUI::Window* sheet = CEGUI::WindowManager::getSingleton().loadWindowLayout( (CEGUI::utf8*)"ogregui.layout"); 
	m_GUISystem->setGUISheet(sheet);*/

	//m_sceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE);

	//m_sceneMgr->setAmbientLight(Ogre::ColourValue(1.0, 1.0, 1.0));

	//m_renderWindow->setActive( true );
	return true;
}
//////////////////////////////////////////////////////////////////////////
std::size_t OgreRenderSystem::getResolutionList( float ** _list )
{
	Ogre::ConfigOptionMap& configMap = m_renderSys->getConfigOptions();
	Ogre::StringVector res = configMap["Video Mode"].possibleValues;
	for( Ogre::StringVector::size_type
		it = 0,
		it_end = res.size();
	it != it_end; 
	++it )
	{
		float p1 = res[it].find('x');
		float p2 = res[it].find('@');
		float w = Ogre::StringConverter::parseReal( res[it].substr( 0, p1 - 1 ) );
		float h = Ogre::StringConverter::parseReal( res[it].substr( p1 + 2, p2 - p1 - 2) );
		m_resolutions.push_back( w );
		m_resolutions.push_back( h );
	}

	*_list = &m_resolutions[0];

	return m_resolutions.size();
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::setContentResolution( const float * _resolution )
{
	m_contentResolution.x = _resolution[0];
	m_contentResolution.y = _resolution[1];
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::render( RenderImageInterface* _image, const int* rect )
{
	if( Ogre::TextureManager::getSingleton().getByName("__shot__").isNull() == false ) 
	{
		return;
	}

	Ogre::Rect wrect( 0, 0, m_renderWindow->getWidth(), m_renderWindow->getHeight() );

	if(rect)
	{
		wrect.left = rect[0];
		wrect.top = rect[1];
		wrect.right = rect[2];
		wrect.bottom = rect[3];
	}

	Ogre::TexturePtr rtt = Ogre::TextureManager::getSingleton().createManual("__shot__", "Default", Ogre::TEX_TYPE_2D, 1024, 768, 0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET);

	Ogre::RenderTarget* rtgt = rtt->getBuffer()->getRenderTarget();
	
	Ogre::Camera* sceneCam = m_sceneMgr->getCamera("defaultCamera");
	
	rtgt->addViewport( sceneCam );
	//rtgt->setActive( false );

	Ogre::Root::getSingleton().renderOneFrame();

	if( _image )
	{
		Ogre::HardwarePixelBufferSharedPtr pixb = rtt->getBuffer();
		Ogre::Image::Box imagebox( wrect.left, wrect.top, wrect.right, wrect.bottom );

		// наху€ доху€ наху€рили ху€рей? расху€ривайте нахуй!
		// ху€ри - функции.
		// јвотхуй.
		static_cast<OgreRenderImage*>( _image )->m_texture->
			getBuffer()->blit( pixb, imagebox,
			Ogre::Image::Box(0, 0, _image->getWidth(), _image->getHeight() )
			);
	}
	rtgt->removeAllViewports();
	m_renderWindow->removeViewport(0);
	m_viewport = m_renderWindow->addViewport( sceneCam );
	sceneCam->getViewport()->setDimensions( m_viewportDimensions[0], m_viewportDimensions[1], m_viewportDimensions[2], m_viewportDimensions[3] );
	Ogre::TextureManager::getSingleton().remove("__shot__");
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::render()
{
	Ogre::Root::getSingleton().renderOneFrame();
	//m_spriteMgr->doRender();
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
RenderImageInterface* OgreRenderSystem::createImage( const char* _name, unsigned int _width, unsigned int _height )
{
	RenderImageInterface * renderImage = new OgreRenderImage( _name, _width, _height, Ogre::TU_DEFAULT, this );
	return renderImage;
}
//////////////////////////////////////////////////////////////////////////
RenderImageInterface* OgreRenderSystem::createRenderTargetImage( const char* _name, unsigned int _width, unsigned int _height, const char* _camera  )
{
	OgreRenderImage* image = new OgreRenderImage( _name, _width, _height, Ogre::TU_RENDERTARGET, this );
	//Ogre::Camera* rttCamera = m_sceneMgr->createCamera( _name );
	//m_renderWindow->addViewport( rttCamera );
	//rttCamera->
	//image->getTexture()->getBuffer()->getRenderTarget()->addViewport( rttCamera );
	return image;
}
//////////////////////////////////////////////////////////////////////////
RenderImageInterface* OgreRenderSystem::loadImage( const TextureDesc& _desc )
{
	RenderImageInterface * renderImage = new OgreRenderImage( _desc );
	return renderImage; 
}
//////////////////////////////////////////////////////////////////////////
RenderImageInterface * OgreRenderSystem::getImage( const char * _desc ) const
{ 
	Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton().getByName(_desc);

	if( texture.isNull() == true ) 
	{
		return 0;
	}

	RenderImageInterface * renderImage = new OgreRenderImage( _desc );

	return renderImage;
}
//////////////////////////////////////////////////////////////////////////
RenderVideoStreamInterface* OgreRenderSystem::loadImageVideoStream( const char* _filename )
{
	//Sets an input file name - needed by plugin
	m_videoControl->setInputName( _filename );
	//Start paused so we can have audio
	m_videoControl->setPlayMode( Ogre::TextureEffectPause );
	//! Used for attaching texture to Technique, State, and texture unit layer
	m_videoControl->setTextureTecPassStateLevel( 0, 0, 0 );

	// Grab Our material, then add a new texture unit
	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName( _filename );

	//Create the material the first time through this method
	if( material.isNull() ) 
	{
		material = Ogre::MaterialManager::getSingleton().create( _filename, "Default" );
	}

	material->getTechnique(0)->getPass(0)->createTextureUnitState();

	m_videoControl->setSeekEnabled( false );

	m_videoControl->createDefinedTexture( _filename, "Default" );
	Ogre::TheoraMovieClip* clip = m_videoControl->getMaterialNameClip( _filename );
	
	clip->changePlayMode( Ogre::TextureEffectPlay_Looping );
	OgreRenderVideoStream* video = new OgreRenderVideoStream( clip, _filename );
	video->m_texture = clip->getVideoDriver()->getTexture();
	m_videoTexturesMap.insert( std::make_pair( video, clip ) );

	//return NULL; 
	return video;
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::releaseImageVideoStream( RenderVideoStreamInterface* _image )
{
	m_videoTexturesMap.erase( _image );
	m_videoControl->destroyAdvancedTexture( static_cast<OgreRenderVideoStream*>(_image)->getName(), "Default" );
	delete static_cast<OgreRenderVideoStream*>(_image);
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::releaseImage( RenderImageInterface * _image )
{
	delete static_cast<OgreRenderImage*>(_image);
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::renderLine(const char * _camera, unsigned int _color, const float * _begin, const float * _end)
{
	Ogre::uint8 renderQueue = Ogre::RENDER_QUEUE_OVERLAY;
	Ogre::Camera* camera = m_sceneMgr->getCamera( _camera );

	float z = m_spriteMgr->getCurrentZ();

	Ogre::Vector2 dir(_end[0]-_begin[0],_end[1]-_begin[1]);

	float L = std::max(fabs(dir.x),fabs(dir.y));

	Ogre::Vector2 size(L,1);

	dir.normalise();

	Ogre::Matrix3 matrix(dir.x,dir.y,0,-dir.y,dir.x,0,_begin[0],_begin[1],1);

	m_spriteMgr->addQuad1(camera->getViewport(), m_contentResolution,Ogre::Vector4(0,0,1,1),matrix,
		Ogre::Vector2(0,0),size, z,m_pixelTexture, _color, 
		Ogre::SBF_SOURCE_ALPHA, Ogre::SBF_ONE_MINUS_SOURCE_ALPHA, renderQueue );
	
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::renderImage(		
				 const char * _camera,
				 const float * _transform, 
				 const float * _offset,
				 const float * _uv,
				 const float * _size,
				 unsigned int _color, 
				 const RenderImageInterface* _image,
				 EBlendFactor _src,
				 EBlendFactor _dst)
{
	TMovieMap::iterator it = m_videoTexturesMap.find( _image );
	if( it != m_videoTexturesMap.end() )
	{
		it->second->blitFrameCheck();
	}

	if( const OgreRenderImage * image = static_cast<const OgreRenderImage *>( _image ) )
	{
		Ogre::uint8 renderQueue = Ogre::RENDER_QUEUE_OVERLAY;
		Ogre::Camera* camera = m_sceneMgr->getCamera( _camera );
		/*if( camera->getName() != "defaultCamera" )
		{
			renderQueue = Ogre::RENDER_QUEUE_MAIN;
		}*/
		Ogre::Texture * texture = image->getTexture();
		float z = m_spriteMgr->getCurrentZ();
		m_spriteMgr->addQuad1(camera->getViewport(), m_contentResolution,*(Ogre::Vector4*)_uv,*(Ogre::Matrix3*)_transform,
								*(Ogre::Vector2*)_offset,*(Ogre::Vector2*)_size, z,image, _color, 
								(Ogre::SceneBlendFactor)_src, (Ogre::SceneBlendFactor)_dst, renderQueue );
	}
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::renderImage(		
		const char * _camera,
		const float * _transform, 
		const float * _a,
		const float * _b,
		const float * _c,
		const float * _d,
		const float * _uv,
		unsigned int _color, 
		const RenderImageInterface * _image,
		EBlendFactor _src,
		EBlendFactor _dst)
{
	TMovieMap::iterator it = m_videoTexturesMap.find( _image );
	if( it != m_videoTexturesMap.end() )
	{
		it->second->blitFrameCheck();
	}
	if( const OgreRenderImage * image = static_cast<const OgreRenderImage *>( _image ) )
	{
		Ogre::Camera* camera = m_sceneMgr->getCamera( _camera );
		Ogre::Texture * texture = image->getTexture();
		float z = m_spriteMgr->getCurrentZ();
		m_spriteMgr->addQuad2(
			camera->getViewport(),
			m_contentResolution,
			*(Ogre::Vector4*)_uv,
			*(Ogre::Matrix3*)_transform,
			*(Ogre::Vector2*)_a,
			*(Ogre::Vector2*)_b,
			*(Ogre::Vector2*)_c,
			*(Ogre::Vector2*)_d, 
			z,
			image, 
			_color,
			(Ogre::SceneBlendFactor)_src,
			(Ogre::SceneBlendFactor)_dst);
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
void OgreRenderSystem::setFullscreenMode( float _width, float _height, bool _fullscreen )
{
	m_renderWindow->setFullscreen( _fullscreen, _width, _height );
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::loadResource( Ogre::Resource* _resource )
{
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::setViewportDimensions( float _width, float _height, float _renderFactor )
{
//	m_renderWindow->update();
	//m_renderWindow->getViewport(0)->update();
	float realWidth = m_renderWindow->getWidth();
	float realHeight = m_renderWindow->getHeight();
	float aspect = _width / _height;
	float width = _width / realWidth;
	float height = _height / realHeight;

	if( width > 1.0f )
	{
		width = 1.0f;
		height = realWidth / aspect / realHeight;
	}

	if( height > 1.0f )
	{
		height = 1.0f;
		width = realHeight * aspect / realWidth;
	}

	if( _renderFactor )
	{
		width += ( 1.0f - width ) * _renderFactor;
		height += ( 1.0f - height ) * _renderFactor;
	}

	m_viewportDimensions[0] = 0.5f - width / 2.0f;
	m_viewportDimensions[1] = 0.5f - height / 2.0f;
	m_viewportDimensions[2] = width;
	m_viewportDimensions[3] = height;

	m_viewport->setDimensions( m_viewportDimensions[0], m_viewportDimensions[1], m_viewportDimensions[2], m_viewportDimensions[3] );
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::setEventListener( RenderSystemListener* _listener )
{
	m_eventListener = _listener;
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::eventOccurred( const Ogre::String& eventName, const Ogre::NameValuePairList* parameters /* = 0 */ )
{
	if( eventName == "DeviceRestored" )
	{
		if( m_eventListener )
		{
			m_eventListener->onDeviceRestored();
		}
	}
}
//////////////////////////////////////////////////////////////////////////
OgreRenderSpriteManager* OgreRenderSystem::getRenderSpriteManager() const
{
	return m_spriteMgr;
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::onWindowMovedOrResized()
{
	m_renderWindow->windowMovedOrResized();
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::onWindowActive( bool _active )
{
	m_renderWindow->setActive( _active );
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::onWindowClose()
{
	m_renderWindow->destroy();
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::setTextureFiltering( bool _filter )
{
	m_spriteMgr->setTextureFiltering( _filter );
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::addResourceLocation( const char* _path )
{
	char dir[MAX_PATH];
	::GetCurrentDirectoryA( MAX_PATH, dir );

	Ogre::ResourceGroupManager * resourceGroupMgr = 
	Ogre::ResourceGroupManager::getSingletonPtr();

	resourceGroupMgr->addResourceLocation( Ogre::String(dir) + Ogre::String(_path), "FileSystem", "Default", false );
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::initResources()
{
	Ogre::ResourceGroupManager * resourceGroupMgr = 
	Ogre::ResourceGroupManager::getSingletonPtr();

	resourceGroupMgr->initialiseResourceGroup("Default");
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::beginScene()
{

}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::endScene()
{

}
//////////////////////////////////////////////////////////////////////////