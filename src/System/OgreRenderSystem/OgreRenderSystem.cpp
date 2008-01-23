#	include "OgreRenderSystem.h"

#	include "OgreRenderSpriteManager.h"
#	include "OgreRenderImage.h"

#	include "Ogre3dCamera.h"
#	include "Ogre3dEntity.h"

#	include "OgreRenderVideoStream.h"
#	include "OgreExternalTextureSourceManager.h"



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

Camera3dInterface * OgreRenderSystem::createCamera(const char * _name)
{
	Ogre::Camera * camera = m_sceneMgr->createCamera("defaultCamera");
	Camera3dInterface * ogre3dcam = new Ogre3dCamera( camera );
	m_viewport = m_renderWindow->addViewport( camera );
	return  ogre3dcam;
}

Entity3dInterface * OgreRenderSystem::create3dEntity(const char * _name, const char * _meshName)
{
	Ogre::Entity * entity = m_sceneMgr->createEntity(_name, _meshName);
	Ogre::SceneNode * newNode = m_sceneMgr->getRootSceneNode()->createChildSceneNode();
	newNode->attachObject( entity );

	Entity3dInterface * ogre3dEntity = new Ogre3dEntity( newNode/*entity*/ );
	return 	ogre3dEntity;
}
//Ogre::Entity * e = m_sceneMgr->createEntity("head", "E:\\ZombieTest\\test.mesh");

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


	//Ogre::DataStreamPtr data = Ogre::ArchiveManager::getSingleton().open( "E:/ogrehead.mesh" );
	//Ogre::MeshPtr m = Ogre::MeshManager::getSingleton ().lo.load("E:/ogrehead.mesh", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	//e->setMaterialName("Examples/OffsetMapping/Specular");

	Ogre::ResourceGroupManager::getSingleton().addResourceLocation( "E:\\Menge\\bin\\Game\\ZombieTest", "FileSystem", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, true );
//	Ogre::ResourceGroupManager::getSingleton().addResourceLocation( "E:\\Menge\\bin\\Game\\ZombieTest\\", "FileSystem", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, true );

	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	//Ogre::MaterialManager::getSingleton().initialise();
	//Ogre::MeshPtr m = Ogre::MeshManager::getSingleton ().load("E:\\ZombieTest\\test.mesh", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	m_sceneMgr->setAmbientLight(Ogre::ColourValue(1.0, 1.0, 1.0));
	//pass->setSceneBlending(SBT_TRANSPARENT_ALPHA);

    Ogre::Light* l = m_sceneMgr->createLight("MainLight");
    l->setPosition(0,0,0);

	//l->setAttenuation(8000,1,0,0);
/*	Ogre::Entity * e = m_sceneMgr->createEntity("sobako", "E:\\ZombieTest\\robot.mesh");
	Ogre::SceneNode* headNode = m_sceneMgr->getRootSceneNode()->createChildSceneNode();
	headNode->attachObject(e);

	Ogre::AnimationState*anim=e->getAnimationState("Walk");
	anim->setTimePosition(0);
	anim->setEnabled(true);
	anim->setLoop(true);
	anim->addTime(78);
			*/
/////////////////////////
/*	Ogre::Entity * e = m_sceneMgr->createEntity("head", "E:\\ZombieTest\\barrel.mesh");

	Ogre::SceneNode* headNode = m_sceneMgr->getRootSceneNode()->createChildSceneNode();
	headNode->attachObject(e);

	Ogre::SubMesh * sm = e->getMesh()->getSubMesh(0);
	Ogre::VertexDeclaration * vd = sm->vertexData->vertexDeclaration;

	Ogre::HardwareVertexBufferSharedPtr hvb = sm->vertexData->vertexBufferBinding->getBuffer(0);

	float * vertices = (float*)hvb->lock(0,0,Ogre::HardwareBuffer::HBL_NORMAL);

	FILE * f = fopen("E:\\ZombieTest\\converted.txt","wb");

	int nverts = sm->vertexData->vertexCount;
	fwrite(&nverts,sizeof(int),1,f);

	int ninds = sm->indexData->indexCount;
	fwrite(&ninds,sizeof(int),1,f);

	m_viewport->setBackgroundColour( Ogre::ColourValue::Green );
	
	float x, y ,z;
	for(int i = 0, k = 0; i < sm->vertexData->vertexCount; i++)
	{
		x = vertices[k+0];
		y = vertices[k+1];
		z = vertices[k+2];
		k = k + 3 + 3 + 2;

		fwrite(&x,sizeof(float),1,f);
		fwrite(&y,sizeof(float),1,f);
		fwrite(&z,sizeof(float),1,f);
	}

	hvb->unlock();


	unsigned short*  indecies = (unsigned short*)sm->indexData->indexBuffer->lock(0,0,Ogre::HardwareBuffer::HBL_NORMAL);
	for(int i = 0; i < sm->indexData->indexCount; i++)
	{
		unsigned short ind = indecies[i];
		fwrite(&ind,sizeof(unsigned short),1,f);
	}

	m_videoControl = static_cast<Ogre::TheoraVideoController*>
		(Ogre::ExternalTextureSourceManager::getSingleton().
		getExternalTextureSource("ogg_video"));

	// еще надо приатачить к ноде, и настроить камеру
	Ogre::Entity * e = m_sceneMgr->createEntity("ent", Ogre::SceneManager::PrefabType::PT_CUBE );
	m_sceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject( e );

	sm->indexData->indexBuffer->unlock();

	fclose(f);
*/
/////////////////////////

//	e->setMaterialName("Material_#0");

//	Ogre::Entity * e = m_sceneMgr->createEntity("head", "E:\\ZombieTest\\ogrehead.mesh");
//	e->setMaterialName("Examples/EnvMappedRustySteel");

//	Ogre::SceneNode* headNode = m_sceneMgr->getRootSceneNode()->createChildSceneNode();
	
//	m_sceneMgr->setShadowTechnique(Ogre::SHADOWDETAILTYPE_TEXTURE);
//	m_sceneMgr->setShadowFarDistance(1000);
        

//	headNode->attachObject(e);
//	headNode->setScale(1,1,1);
//	headNode->setPosition(0,0,0);
//	e->setNormaliseNormals(true);
//	e->setCastShadows(true);

	return true;
}
//////////////////////////////////////////////////////////////////////////
bool OgreRenderSystem::frameStarted( const Ogre::FrameEvent &evt)
{
	//m_renderSys->_setSceneBlending(Ogre::SBF_SOURCE_ALPHA, Ogre::SBF_ONE_MINUS_SOURCE_ALPHA);
	//m_spriteMgr->Start();
	return true;
}
//////////////////////////////////////////////////////////////////////////
bool OgreRenderSystem::frameEnded( const Ogre::FrameEvent &evt)
{
	//m_spriteMgr->End();
	return true;
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