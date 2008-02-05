#	include "OgreRenderSystem.h"

#	include "OgreRenderSpriteManager.h"
#	include "OgreRenderImage.h"

#	include "OgreCamera.h"
#	include "OgreEntity.h"
#	include "OgreLight.h"
#	include "OgreMesh.h"
#	include "OgreSceneNode.h"


#	include "OgreRenderVideoStream.h"
#	include "OgreExternalTextureSourceManager.h"

/*#include "CEGUI/CEGUISystem.h"
#include "CEGUI/CEGUILogger.h"
#include "CEGUI/CEGUISchemeManager.h"
#include "CEGUI/CEGUIWindowManager.h"
#include "CEGUI/CEGUIWindow.h"
#include "OgreCEGUIRenderer.h"*/

#define DRAW2D
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
	if(m_GUISystem)
	{
		delete m_GUISystem;
		m_GUISystem = 0;
	}
	if(m_GUIRenderer)
	{
		delete m_GUIRenderer;
		m_GUIRenderer = 0;
	}
}
//////////////////////////////////////////////////////////////////////////
CameraInterface * OgreRenderSystem::createCamera(const char * _name)
{
	Ogre::Camera * camera = m_sceneMgr->createCamera(_name);
	CameraInterface * ogre3dcam = new Ogre3dCamera( camera );
	m_viewport = m_renderWindow->addViewport( camera );
	return  ogre3dcam;
}
//////////////////////////////////////////////////////////////////////////
SceneNodeInterface * OgreRenderSystem::attachSceneNodeToRoot( const char * _name )
{
	Ogre::SceneNode * sceneNode = 
		m_sceneMgr->getRootSceneNode()->createChildSceneNode( _name );

//	return new OgreSceneNode( sceneNode );
	return NULL;
} 
//////////////////////////////////////////////////////////////////////////
LightInterface * OgreRenderSystem::createLight(const char * _name)
{
	Ogre::Light * light = m_sceneMgr->createLight(_name);
	LightInterface * ogreLight = new OgreLight(light);
	return ogreLight;   
}
//////////////////////////////////////////////////////////////////////////
MeshInterface * OgreRenderSystem::createMesh(const char * _name)
{
	Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().load(
		_name, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	MeshInterface * ogremesh = new OgreMesh(mesh);
	return ogremesh;
}
Ogre::AnimationState* anim0;
Ogre::AnimationState* anim1;
Ogre::Entity * e;
//////////////////////////////////////////////////////////////////////////
bool OgreRenderSystem::init( Ogre::Root * _root, Ogre::RenderWindow * _renderWindow )
{
	m_root = _root;
	m_renderWindow = _renderWindow;

	m_sceneMgr = m_root->createSceneManager( Ogre::ST_GENERIC, "defaultSceneManager" );
	m_renderSys = m_root->getRenderSystem();

	#ifdef  DRAW2D
	
	
	m_spriteMgr = new OgreRenderSpriteManager();
	m_spriteMgr->init( m_sceneMgr, m_renderSys, m_viewport, Ogre::RENDER_QUEUE_OVERLAY, true);
	//m_sceneMgr->setAmbientLight( Ogre::ColourValue(0.9f, 0.9f, 0.9f));
	//Ogre::Camera * camera = m_sceneMgr->createCamera("defaultCamera");
 	//camera->setNearClipDistance(5);
	// Position it at 500 in Z direction
	//camera->setPosition(Ogre::Vector3(0,0,500));
	// Look back along -Z
	//camera->lookAt(Ogre::Vector3(0,0,-300));

	#endif

	//Ogre::ResourceGroupManager::getSingleton().addResourceLocation( "D:\\Development\\Menge\\bin\\Game\\GUITest", "FileSystem", "Default", true );

	//Ogre::ResourceGroupManager::getSingleton().addResourceLocation( "E:\\ZombieTest\\Models\\", "FileSystem", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME );
	//Ogre::ResourceGroupManager::getSingleton().addResourceLocation( "E:\\ZombieTest\\Materials\\", "FileSystem", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME );
	//Ogre::ResourceGroupManager::getSingleton().addResourceLocation( "E:\\ZombieTest\\Materials\\Scripts", "FileSystem", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME );
	//Ogre::ResourceGroupManager::getSingleton().addResourceLocation( "E:\\ZombieTest\\Materials\\Textures", "FileSystem", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME );

	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(0);
	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

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
	//l->setAttenuation(8000,1,0,0);
	//Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().load("E:\\ZombieTest\\Models\\robot.mesh", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	//
	//Ogre::Entity * e = m_sceneMgr->createEntity("hh", "E:\\ZombieTest\\Models\\robot.mesh");
	////Ogre::SceneNode* headNode = m_sceneMgr->getRootSceneNode()->createChildSceneNode();
	////headNode->attachObject(e);

	//Ogre::SceneNode* headNode = m_sceneMgr->getRootSceneNode()->createChildSceneNode();
	//headNode->attachObject(e);




/*	e = m_sceneMgr->createEntity("hh", "E:\\ZombieTest\\Models\\robot.mesh");
	Ogre::SceneNode* headNode = m_sceneMgr->getRootSceneNode()->createChildSceneNode();
	headNode->attachObject(e);

	class	SkeletalAnimation
	{
		//////////////////////////////////////////////////////////////////////////
		void SkeletalAnimation::update( float _timing )
		{
			m_animState0->addTime(_timing / 1000.f);

			if(m_animState1 != NULL )
			{
				m_animState1->addTime(_timing / 1000.f);
			}
		}
		//////////////////////////////////////////////////////////////////////////
		void SkeletalAnimation::play( const std::string& _name )
		{
			m_animState0 = e->getAnimationState(_name);
			m_animState0->setTimePosition(0);
			m_animState0->setLoop(false);
		}
		//////////////////////////////////////////////////////////////////////////
		void SkeletalAnimation::play( const std::string& _name1, float _weight1, const std::string& _name2, float _weight2 )
		{
			m_animState0 = e->getAnimationState(_name1);
			m_animState0->setTimePosition(0);
			m_animState0->setLoop(false);
			m_animState0->setWeight(_weight1);

			m_animState1 = e->getAnimationState(_name2);
			m_animState1->setTimePosition(0);
			m_animState1->setLoop(false);
			m_animState1->setWeight(_weight2);
		}
		//////////////////////////////////////////////////////////////////////////
		void SkeletalAnimation::stop()
		{
			m_animState0->setEnabled(false);
		}
		//////////////////////////////////////////////////////////////////////////
		void SkeletalAnimation::setLooped( bool _looped )
		{
			m_animState0->setLoop(_looped);
		}
		//////////////////////////////////////////////////////////////////////////
	private:

		Ogre::AnimationState * m_animState0;
		Ogre::AnimationState * m_animState1;
		Ogre::AnimationState * m_currentState;
	};

	headNode->setScale(0.5f,0.5f,0.5f);

	anim0=e->getAnimationState("Walk");
	anim0->setTimePosition(0);
	anim0->setEnabled(true);
	anim0->setLoop(false);


	//anim1=e->getAnimationState("Shoot");
	//anim1->setTimePosition(0);
	//anim1->setEnabled(true);
	//anim1->setLoop(true);

	anim0->setWeight(0.9f);
	anim1->setWeight(0.7f);
*/
	//Ogre::AnimationStateSet *sset = e->getAllAnimationStates();

//	Ogre::SkeletonInstance *  sc = e->getSkeleton();

	m_sceneMgr->setAmbientLight(Ogre::ColourValue(1.0, 1.0, 1.0));

	/*m_root = _root;
	m_renderWindow = _renderWindow;

	m_sceneMgr = m_root->createSceneManager( Ogre::ST_GENERIC, "defaultSceneManager" );
	m_renderSys = m_root->getRenderSystem();

	//m_sceneMgr->setAmbientLight( Ogre::ColourValue(0.9f, 0.9f, 0.9f));
	Ogre::Camera * camera = m_sceneMgr->createCamera("defaultCamera");
	camera->setNearClipDistance(5);
	// Position it at 500 in Z direction
	camera->setPosition(Ogre::Vector3(0,0,500));
	// Look back along -Z
	camera->lookAt(Ogre::Vector3(0,0,-300));

	m_viewport = m_renderWindow->addViewport( camera );

	m_viewport->setBackgroundColour( Ogre::ColourValue::Green );

	float width = m_viewport->getActualWidth();
	float height = m_viewport->getActualHeight();
	float aspect = width / height;

	camera->setAspectRatio( aspect );

	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(0);

	m_spriteMgr = new OgreRenderSpriteManager();
	m_spriteMgr->init( m_sceneMgr, m_renderSys, m_viewport, Ogre::RENDER_QUEUE_OVERLAY, true);


	m_videoControl = static_cast<Ogre::TheoraVideoController*>
		(Ogre::ExternalTextureSourceManager::getSingleton().
		getExternalTextureSource("ogg_video"));

	// еще надо приатачить к ноде, и настроить камеру
	//Ogre::Entity * e = m_sceneMgr->createEntity("ent", Ogre::SceneManager::PrefabType::PT_CUBE );
	//m_sceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject( e );*/


	return true;
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::update(float _timing)
{
/*	anim0->addTime(_timing / 1000.f);
	float t = anim0->getTimePosition();

	printf("%f \n", t);

	anim1->addTime(_timing / 1000.f);

	t = anim1->getTimePosition();
*/
//	anim1->setEnabled(false);

//	printf("%f \n", t);
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
	#ifdef  DRAW2D
	if( Ogre::TextureManager::getSingleton().getByName("__shot__").isNull() == false ) return;

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

	m_renderWindow->getViewport(0)->setClearEveryFrame(false);
	Ogre::Root::getSingleton().renderOneFrame();
	m_renderWindow->getViewport(0)->setClearEveryFrame(true);

	if( _image )
	{
		Ogre::HardwarePixelBufferSharedPtr pixb = rtt->getBuffer();
		Ogre::Image::Box imagebox( wrect.left, wrect.top, wrect.right, wrect.bottom );
		static_cast<OgreRenderImage*>( _image )->m_texture->getBuffer()->blit(pixb, imagebox, Ogre::Image::Box(0, 0, _image->getWidth(), _image->getHeight() ));
	}
	
	Ogre::TextureManager::getSingleton().remove("__shot__");
#endif

}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::render()
{
	Ogre::Root::getSingleton().renderOneFrame();
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
	return new OgreRenderImage( _name, _width, _height, Ogre::TU_DEFAULT, this );
}
//////////////////////////////////////////////////////////////////////////
RenderImageInterface* OgreRenderSystem::createRenderTargetImage( const char* _name, unsigned int _width, unsigned int _height )
{
	return new OgreRenderImage( _name, _width, _height, Ogre::TU_RENDERTARGET, this );
}
//////////////////////////////////////////////////////////////////////////
RenderImageInterface* OgreRenderSystem::loadImage( const TextureDesc&	_desc )
{
	return new OgreRenderImage( _desc );
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
		material = Ogre::MaterialManager::getSingleton().create( _filename, "Default" );

	material->getTechnique(0)->getPass(0)->createTextureUnitState();

	m_videoControl->setSeekEnabled( false );

	m_videoControl->createDefinedTexture( _filename, "Default" );
	Ogre::TheoraMovieClip* clip = m_videoControl->getMaterialNameClip( _filename );
	
	clip->changePlayMode( Ogre::TextureEffectPlay_Looping );
	OgreRenderVideoStream* video = new OgreRenderVideoStream( clip, _filename );
	video->m_texture = clip->getVideoDriver()->getTexture();
	m_videoTexturesMap.insert( std::make_pair( video, clip ) );

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
				 const RenderImageInterface* _image,
				 EBlendFactor _src,
				 EBlendFactor _dst)
{
	#ifdef  DRAW2D

	TMovieMap::iterator it = m_videoTexturesMap.find( _image );
	if( it != m_videoTexturesMap.end() )
	{
		it->second->blitFrameCheck();
	}

	if( const OgreRenderImage * image = static_cast<const OgreRenderImage *>( _image ) )
	{
		Ogre::Texture * texture = image->getTexture();
		float z = m_spriteMgr->getCurrentZ();
		m_spriteMgr->addQuad1(m_contentResolution,*(Ogre::Vector4*)_uv,*(Ogre::Matrix3*)_transform,*(Ogre::Vector2*)_offset,*(Ogre::Vector2*)_size, z,image, _color, (Ogre::SceneBlendFactor)_src, (Ogre::SceneBlendFactor)_dst);
	}
#endif
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
		const RenderImageInterface * _image,
		EBlendFactor _src,
		EBlendFactor _dst)
{
#ifdef  DRAW2D
	TMovieMap::iterator it = m_videoTexturesMap.find( _image );
	if( it != m_videoTexturesMap.end() )
	{
		it->second->blitFrameCheck();
	}
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
			_color,
			(Ogre::SceneBlendFactor)_src,
			(Ogre::SceneBlendFactor)_dst);
	}
#endif
}

//////////////////////////////////////////////////////////////////////////
void	OgreRenderSystem::beginLayer()
{
}
//////////////////////////////////////////////////////////////////////////
void	OgreRenderSystem::endLayer()
{
	#ifdef  DRAW2D
	m_spriteMgr->diffZ();
	#endif
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::setFullscreenMode(  bool _fullscreen )
{
	m_renderWindow->setFullscreen( _fullscreen, m_renderWindow->getWidth(), m_renderWindow->getHeight() );
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::loadResource( Ogre::Resource* _resource )
{
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::setViewportDimensions( float _width, float _height, float _renderFactor )
{
	m_renderWindow->update();
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
	m_viewport->setDimensions( 0.5f - width / 2.0f, 0.5f - height / 2.0f, width, height );
}
