#	include "Application.h"

#	include "InputEngine.h"
#	include "InputHandler.h"

#	include "FileEngine.h"
#	include "RenderEngine.h"
#	include "SoundEngine.h"
#	include "Amplifier.h"
#	include "ParticleEngine.h"
#	include "ScriptEngine.h"
#	include "PhysicEngine.h"
#	include "PhysicEngine2D.h"

#	include "MousePickerSystem.h"
#	include "LightSystem.h"
#	include "ResourceManager.h"

#	include "Game.h"

#	include "LogEngine.h"
#	include "ProfilerEngine.h"

#	include "XmlEngine.h"

#	include "SceneManager.h"
#	include "TextField.h"

#	include "Camera2D.h"

#	include "NodeFactory.h"
#	include "ScheduleManager.h"

#	include "Entity.h"
#	include "Animation.h"
#	include "Arrow.h"
#	include "Emitter.h"
#	include "HotSpot.h"
#	include "Light2D.h"
#	include "ShadowCaster2D.h"
#	include "Point.h"
#	include "RigidBody2D.h"
#	include "TilePolygon.h"
#	include "SoundEmitter.h"
#	include "Sprite.h"
#	include "TextField.h"
#	include "TileMap.h"
#	include "Track.h"
#	include "Video.h"
#	include "Layer2DLoop.h"
#	include "Layer2DAccumulator.h"
#	include "CapsuleController.h"
#	include "Layer2D.h"
#	include "Layer3D.h"
#	include "LayerScene.h"
#	include "RenderMesh.h"
#	include "Camera3D.h"

#	include "ResourceAnimation.h"
#	include "ResourceCapsuleController.h"
#	include "ResourceEmitterContainer.h"
#	include "ResourceFont.h"
#	include "ResourceTilePolygon.h"
#	include "ResourceImageAtlas.h"
#	include "ResourceImageCell.h"
#	include "ResourceImageDefault.h"
#	include "ResourceImageDynamic.h"
#	include "ResourceImageSet.h"
#	include "ResourceVideo.h"
#	include "ResourceMesh.h"
#	include "ResourceSkeleton.h"
#	include "ResourcePhysicBoxGeometry.h"
#	include "ResourcePhysicConcaveGeometry.h"
#	include "ResourcePhysicConvexGeometry.h"
#	include "ResourcePlaylist.h"
#	include "ResourceSound.h"
#	include "ResourceTileMap.h"
#	include "ResourceTileSet.h"
#	include "ResourceMeshMS3D.h"
#	include "ResourceMeshNoise.h"
#	include "ResourceMaterial.h"
#	include "Player.h"
#	include "Scene.h"

#	include "Codec.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class ApplicationInputHandlerProxy
		: public InputHandler
	{
	public:
		ApplicationInputHandlerProxy( Application * _app )
			: m_app( _app )
		{}

	public:
		bool handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown ) override
		{
			return m_app->onKeyEvent( _key, _char, _isDown );
		}

		bool handleMouseButtonEvent( unsigned int _button, bool _isDown ) override
		{
			return m_app->onMouseButtonEvent( _button, _isDown );
		}

		bool handleMouseMove( float _x, float _y, int _whell ) override
		{
			return m_app->onMouseMove( _x, _y, _whell );
		}

	private:
		Application * m_app;
	};

	//////////////////////////////////////////////////////////////////////////
	Application::Application( ApplicationInterface* _interface )
		: m_interface( _interface )
		, m_commandLine( "" )
		, m_particles( true )
		, m_sound( true )
		, m_debugMask( 0 )
		, m_phycisTiming(0.f)
		, m_resetTiming( false )
		, m_maxTiming( 100.0f )
		, m_debugInfo( false )
		, m_logEngine( 0 )
		, m_profilerEngine( 0 )
		, m_fileEngine( 0 )
		, m_renderEngine( 0 )
		, m_soundEngine( 0 )
		, m_particleEngine( 0 )
		, m_physicEngine2D( 0 )
		, m_physicEngine( 0 )
		, m_xmlEngine( 0 )
		, m_mouseBounded( false )
		, m_game( 0 )
		, m_hasConsole( 0 )
		, m_verbose( false )
	{
		Holder<Application>::keep( this );
		m_handler = new ApplicationInputHandlerProxy( this );
	}
	//////////////////////////////////////////////////////////////////////////
	Application::~Application()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setLogSystem( LogSystemInterface * _interface )
	{
		m_logEngine = new LogEngine( _interface );
		Holder<LogEngine>::keep( m_logEngine );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setProfilerSystem( ProfilerSystemInterface * _interface )
	{
		m_profilerEngine = new ProfilerEngine( _interface );
		Holder<ProfilerEngine>::keep( m_profilerEngine );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setFileSystem( FileSystemInterface * _interface )
	{
		m_fileEngine = new FileEngine( _interface );
		Holder<FileEngine>::keep( m_fileEngine );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setInputSystem( InputSystemInterface * _interface )
	{
		m_inputEngine = new InputEngine( _interface );
		Holder<InputEngine>::keep( m_inputEngine );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setRenderSystem( RenderSystemInterface * _interface )
	{
		m_renderEngine = new RenderEngine( _interface );
		Holder<RenderEngine>::keep( m_renderEngine );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setSoundSystem( SoundSystemInterface * _interface )
	{
		m_soundEngine = new SoundEngine( _interface );
		Holder<SoundEngine>::keep( m_soundEngine );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setParticleSystem( ParticleSystemInterface * _interface )
	{
		 m_particleEngine = new ParticleEngine( _interface );
		 Holder<ParticleEngine>::keep( m_particleEngine );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setPhysicSystem2D( PhysicSystem2DInterface * _interface )
	{
		m_physicEngine2D = new PhysicEngine2D( _interface );
		Holder<PhysicEngine2D>::keep( m_physicEngine2D );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setPhysicSystem( PhysicSystemInterface * _interface )
	{
		m_physicEngine = new PhysicEngine( _interface );
		Holder<PhysicEngine>::keep( m_physicEngine );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::initPredefinedResources()
	{
		ResourceFactoryParam param = { MENGE_TEXT("WhitePixel") };

		ResourceImageDynamic * image = new ResourceImageDynamic( param );
		image->setSize( mt::vec2f( 1.0f, 1.0f ) );
		image->incrementReference();

		Holder<ResourceManager>::hostage()->registerResource( image );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::loadGame( bool _loadPersonality )
	{
		m_game = new Game();
		Holder<Game>::keep( m_game );
		Holder<ScheduleManager>::keep( new ScheduleManager );

		MENGE_LOG( MENGE_TEXT("Create game file [%s] ...\n")
			,m_gameInfo.c_str() );

		if( m_xmlEngine->parseXmlFileM( m_gameInfo, m_game, &Game::loader ) == false )
		{
			MENGE_LOG_ERROR( MENGE_TEXT("Invalid game file [%s] ...\n")
				,m_gameInfo.c_str() );
			MENGE_LOG_CRITICAL( MENGE_TEXT("Application files missing or corrupt") );
			return false;
		}

		const String & title = m_game->getTitle();
		bool fullscreen = m_game->getFullscreen();
		m_renderEngine->setFullscreenMode( fullscreen );

		if( !m_fileEngine->initAppDataPath( MENGE_TEXT("Menge\\") + title ) )
		{
			MENGE_LOG_ERROR( MENGE_TEXT("Warning: Can't initialize user's data path") );
		}

		for( TStringVector::iterator it = m_resourcePaths.begin(), 
			it_end = m_resourcePaths.end(); it != it_end; it++ )
		{
			m_game->readResourceFile( *it );
		}

		m_game->registerResources();

		if( _loadPersonality )
		{
			bool persolality = m_game->loadPersonality();
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::createRenderWindow( WindowHandle _handle )
	{
		const String & title = m_game->getTitle();

/*		if( !m_fileEngine->initAppDataPath( MENGE_TEXT("Menge\\") + title ) )
		{
			MENGE_LOG_ERROR( MENGE_TEXT("Warning: Can't initialize user's data path") );
		}
*/
		const Resolution & resourceResolution = m_game->getResourceResolution();

		bool res = m_renderEngine->initialize();
		if( res == false )
		{
			MENGE_LOG_CRITICAL( MENGE_TEXT("Failed to initialize Render System") );
			return false;
		}

		m_renderEngine->setContentResolution( resourceResolution );

		bool isFullscreen = m_renderEngine->getFullscreenMode();

		if( isFullscreen == true )
		{
			float aspect = m_desktopResolution.getRatio();
			m_currentResolution = m_renderEngine->getBestDisplayResolution( m_desktopResolution, aspect );
		}
		else
		{
			m_currentResolution = m_game->getResolution();
		}

		WindowHandle winHandle = _handle;

		if( _handle == NULL )
		{
			winHandle = m_interface->createWindow( 
				title, m_currentResolution.getWidth(), m_currentResolution.getHeight(), isFullscreen );
		}

		int bits = m_game->getBits();
		int FSAAType = m_game->getFSAAType();
		int FSAAQuality = m_game->getFSAAQuality();

		res = m_renderEngine->createRenderWindow( m_currentResolution, bits, isFullscreen, winHandle,
											FSAAType, FSAAQuality );
		if( res == false )
		{
			MENGE_LOG_CRITICAL( MENGE_TEXT("Failed to create render window") );
			return false;
		}

		bool isTextureFiltering = m_game->getTextureFiltering();

		m_renderEngine->setTextureFiltering( isTextureFiltering );

		m_inputEngine->initialize( winHandle );

		if( isFullscreen == true )
		{
			//setMouseBounded( true );
			m_inputEngine->setMouseBounded( true );
			m_interface->setHandleMouse( false );
		}

		initPredefinedResources();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initGame()
	{
		bool result = m_game->init();

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::loader( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( MENGE_TEXT("Application") )
			{				
				XML_PARSE_ELEMENT( this, &Application::loaderApplication );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::loaderApplication( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( MENGE_TEXT("Game"), MENGE_TEXT("File"), m_gameInfo );

			XML_CASE_NODE( MENGE_TEXT("Resource") )
			{
				String filename;
				XML_FOR_EACH_ATTRIBUTES()
				{					
					XML_CASE_ATTRIBUTE( MENGE_TEXT("File"), filename );
				}
				m_resourcePaths.push_back( filename );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::parseArguments( const StringA& _arguments )
	{
		StringA::size_type idx = _arguments.find( "-sound" );
		if( idx != StringA::npos )
		{
			m_sound = false;
		}

		idx = _arguments.find( "-particles" );
		if( idx != StringA::npos )
		{
			m_particles = false;
		}

		idx = _arguments.find( "-debuginfo" );
		if( idx != StringA::npos )
		{
			m_debugInfo = true;
		}

		idx = _arguments.find( "-console" );
		if( idx != StringA::npos )
		{
			m_hasConsole = true;
		}

		idx = _arguments.find( "-verbose" );
		if( idx != StringA::npos )
		{
			m_verbose = true;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initialize( const String& _applicationFile, const char* _args, bool _loadPersonality )
	{
		parseArguments( _args );

		initInterfaceSystem( &m_logSystem );
		this->setLogSystem( m_logSystem );
		m_logSystem->enableConsole( m_hasConsole );

		initInterfaceSystem( &m_profilerSystem );
		this->setProfilerSystem( m_profilerSystem );

		initInterfaceSystem( &m_fileSystem );
		m_fileSystem->inititalize( m_logSystem );
		this->setFileSystem( m_fileSystem );

		if( m_verbose )
		{
			m_logSystem->setVerboseLevel( 3 );
		}

		m_fileSystem->loadPath( MENGE_TEXT(".") );

		initInterfaceSystem( &m_inputSystem );
		this->setInputSystem( m_inputSystem );

#	if	MENGE_PARTICLES	== (1)
		initInterfaceSystem( &m_particleSystem );
		this->setParticleSystem( m_particleSystem );
#	endif
		
		initInterfaceSystem( &m_physicSystem2D );
		this->setPhysicSystem2D( m_physicSystem2D );
		
		initInterfaceSystem( &m_renderSystem );
		this->setRenderSystem( m_renderSystem );
		
		initInterfaceSystem( &m_soundSystem );
		this->setSoundSystem( m_soundSystem );

		OBJECT_FACTORY( Camera2D );
		OBJECT_FACTORY( Entity );
		OBJECT_FACTORY( Animation );
		OBJECT_FACTORY( Arrow );
		OBJECT_FACTORY( Emitter );
		OBJECT_FACTORY( HotSpot );
		OBJECT_FACTORY( Light2D );
		OBJECT_FACTORY( ShadowCaster2D );
		OBJECT_FACTORY( TilePolygon );
		OBJECT_FACTORY( Point );
		OBJECT_FACTORY( RigidBody2D );
		OBJECT_FACTORY( SoundEmitter );
		OBJECT_FACTORY( Sprite );
		OBJECT_FACTORY( TextField );
		OBJECT_FACTORY( TileMap );
		OBJECT_FACTORY( Track );
		OBJECT_FACTORY( Video );
		OBJECT_FACTORY( Layer2D );
		OBJECT_FACTORY( Layer2DLoop );
		OBJECT_FACTORY( Layer2DAccumulator );
		OBJECT_FACTORY( Layer3D );
		OBJECT_FACTORY( LayerScene );
		OBJECT_FACTORY( RenderMesh );
		OBJECT_FACTORY( Camera3D );
		OBJECT_FACTORY( SceneNode3D );

		RESOURCE_FACTORY( ResourceAnimation );
		RESOURCE_FACTORY( ResourceCapsuleController );
		RESOURCE_FACTORY( ResourceEmitterContainer );
		RESOURCE_FACTORY( ResourceFont );
		RESOURCE_FACTORY( ResourceImageAtlas );
		RESOURCE_FACTORY( ResourceTilePolygon );
		RESOURCE_FACTORY( ResourceImageCell );
		RESOURCE_FACTORY( ResourceImageDefault );
		RESOURCE_FACTORY( ResourceImageDynamic );
		RESOURCE_FACTORY( ResourceImageSet );
		RESOURCE_FACTORY( ResourceVideo );
		RESOURCE_FACTORY( ResourceMesh );
		RESOURCE_FACTORY( ResourceSkeleton );
		RESOURCE_FACTORY( ResourcePhysicBoxGeometry );
		RESOURCE_FACTORY( ResourcePhysicConcaveGeometry );
		RESOURCE_FACTORY( ResourcePhysicConvexGeometry );
		RESOURCE_FACTORY( ResourcePlaylist );
		RESOURCE_FACTORY( ResourceSound );
		RESOURCE_FACTORY( ResourceTileMap );
		RESOURCE_FACTORY( ResourceTileSet );
		RESOURCE_FACTORY( ResourceMeshMS3D );
		RESOURCE_FACTORY( ResourceMeshNoise );
		RESOURCE_FACTORY( ResourceMaterial );
	
		if( !m_interface->init( _applicationFile.c_str(), this ) )
		{
			return false;
		}

		m_desktopResolution[0] = m_interface->getDesktopWidth();
		m_desktopResolution[1] = m_interface->getDesktopHeight();

		bool res = m_soundEngine->initialize();
		if( m_sound == false )
		{
			m_soundEngine->mute( true );
		}
		if( res == false )
		{
			m_sound = false;
		}

		//MENGE_LOG_CRITICAL( MENGE_TEXT("BEGIN") );

		m_xmlEngine = new XmlEngine();
		Holder<XmlEngine>::keep( m_xmlEngine );

		m_xmlEngine->initialize();

		if( m_xmlEngine->parseXmlFileM( _applicationFile, this, &Application::loader ) == false )
		{
			MENGE_LOG_ERROR( MENGE_TEXT("parse application xml failed '%s'\n")
				, _applicationFile.c_str()
				);
			MENGE_LOG_CRITICAL( MENGE_TEXT("Application files missing or corrupt") );
			return false;
		}

		m_fileEngine->changeDir( MENGE_TEXT("../") );

		ScriptEngine * scriptEngine = new ScriptEngine();
		Holder<ScriptEngine>::keep( scriptEngine );
		
		MENGE_LOG( MENGE_TEXT("init scriptEngine ...\n") );
		scriptEngine->init();

		//strcpy( 0, "asdf" );

		m_inputEngine->regHandle( m_handler );

		CodecManager::initialize();

		Holder<ResourceManager>::keep( new ResourceManager() );

		loadGame( _loadPersonality );
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::finalize()
	{
		Holder<ScheduleManager>::destroy();
		if( Holder<Game>::empty() == false )
		{
			Holder<Game>::hostage()->release();
		}

		releaseInterfaceSystem( m_soundSystem );
		releaseInterfaceSystem( m_renderSystem );
		releaseInterfaceSystem( m_physicSystem2D );
#	if	MENGE_PARTICLES	== (1)
		releaseInterfaceSystem( m_particleSystem );
#	endif
		releaseInterfaceSystem( m_inputSystem );
		releaseInterfaceSystem( m_fileSystem );
		releaseInterfaceSystem( m_logSystem );
		releaseInterfaceSystem( m_profilerSystem );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::onKeyEvent( unsigned int _key, unsigned int _char, bool _isDown )
	{
#	ifndef MENGE_MASTER_RELEASE
		if( _key == 88 && _isDown ) // F12
		{
			if( ( m_debugMask & MENGE_DEBUG_HOTSPOTS ) != 0 )
			{
				m_debugMask ^= MENGE_DEBUG_HOTSPOTS;
			}
			else
			{
				m_debugMask |= MENGE_DEBUG_HOTSPOTS;
			}
		}

		if( _key == 87 && _isDown ) // F11
		{
			bool enabled = Holder<ProfilerEngine>::hostage()->isEnabled();
			Holder<ProfilerEngine>::hostage()->setEnabled(!enabled);
		}

		if( _key == 68 && _isDown ) // F10
		{
			if( ( m_debugMask & MENGE_DEBUG_NODES ) != 0 )
			{
				m_debugMask ^= MENGE_DEBUG_NODES;
			}
			else
			{
				m_debugMask |= MENGE_DEBUG_NODES;
			}
		}

		if( _key == 67 && _isDown ) // F9
		{
			if( ( m_debugMask & MENGE_DEBUG_PHYSICS ) != 0 )
			{
				m_debugMask ^= MENGE_DEBUG_PHYSICS;
			}
			else
			{
				m_debugMask |= MENGE_DEBUG_PHYSICS;
			}
		}

		if( _key == 66 && _isDown ) // F8
		{
			if( ( m_debugMask & MENGE_DEBUG_TILEPOLYGON ) != 0 )
			{
				m_debugMask ^= MENGE_DEBUG_TILEPOLYGON;
			}
			else
			{
				m_debugMask |= MENGE_DEBUG_TILEPOLYGON;
			}
		}

#	endif

		return m_game->handleKeyEvent( _key, _char, _isDown );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::onMouseButtonEvent( int _button, bool _isDown )
	{
		return m_game->handleMouseButtonEvent( _button, _isDown );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::onMouseMove( float _x, float _y, int _whell )
	{
		InputEngine * inputEngine = Holder<InputEngine>::hostage();

		float oldx = 0.f;
		float oldy = 0.f;

		if( !inputEngine->getMouseBounded() )
		{ 
			oldx = inputEngine->getMouseX();
			oldy = inputEngine->getMouseY();
			inputEngine->setMousePos( _x, _y );
		}
		return m_game->handleMouseMove( _x - oldx, _y - oldy, _whell );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::onMouseLeave()
	{
		m_game->handleMouseLeave();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::onMouseEnter()
	{
		m_game->handleMouseEnter();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::quit()	
	{
		m_interface->stop();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::frameStarted()
	{
		//Holder<RenderEngine>::hostage()->frameStarted();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setParticlesEnabled( bool _enabled )
	{
		m_particles = _enabled;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::getParticlesEnabled() const
	{
		return m_particles;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setSoundEnabled( bool _enabled )
	{
		m_sound = _enabled;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::getSoundEnabled() const
	{
		return m_sound;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::usePhysic() const
	{
		if( m_physicEngine )
		{
			return true;
		}
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::onFocus( bool _focus )
	{
		static float volume = 1.0f;
		static float avolume = 1.0f;

		if( !_focus )
		{
			volume = Holder<SoundEngine>::hostage()->getCommonVolume();
			avolume = Holder<Amplifier>::hostage()->getVolume();
			m_soundEngine->setCommonVolume( 0.0f );
			Holder<Amplifier>::hostage()->setVolume( 0.0f );
		}
		else
		{
			m_resetTiming = true;
			m_soundEngine->setCommonVolume( volume );
			Holder<Amplifier>::hostage()->setVolume( avolume );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::minimizeWindow()
	{
		m_interface->minimizeWindow();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::run()
	{
		m_interface->run();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::onUpdate( float _timing )
	{
		m_inputEngine->update();

		ProfilerEngine* profiler = Holder<ProfilerEngine>::hostage();
		if( _timing > m_maxTiming )
		{
			_timing = m_maxTiming;
		}

		if( m_resetTiming )
		{
			_timing = 0.0f;
			m_resetTiming = false;
			return;
		}

		//Holder<MousePickerSystem>::hostage()->clear();

		if( m_physicEngine )
		{
			m_physicEngine->update( 1.0f/30.0f );
		}

		profiler->beginProfile( MENGE_TEXT("Menge") );

		if( m_physicEngine2D )
		{
			profiler->beginProfile( MENGE_TEXT("Physic") );
			m_physicEngine2D->update( _timing );
			profiler->endProfile( MENGE_TEXT("Physic") );
		}

		profiler->beginProfile( MENGE_TEXT("Game Update") );
		m_game->update( _timing );

		profiler->endProfile( MENGE_TEXT("Game Update") );

		profiler->beginProfile( MENGE_TEXT("Sound Update") );
		m_soundEngine->update( _timing );
		profiler->endProfile( MENGE_TEXT("Sound Update") );

		m_renderEngine->beginScene();
		m_game->render( m_debugMask );
		m_renderEngine->endScene();

		profiler->endProfile( MENGE_TEXT("Menge") );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::onClose()
	{
		m_renderEngine->onWindowClose();
		quit();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::onDestroy()
	{
		delete m_handler;

		Holder<Game>::destroy();
		Holder<ResourceManager>::destroy();

		Holder<PhysicEngine>::destroy();
		Holder<PhysicEngine2D>::destroy();
		Holder<ParticleEngine>::destroy();
		Holder<RenderEngine>::destroy();
		Holder<FileEngine>::destroy();
		Holder<InputEngine>::destroy();
		Holder<SoundEngine>::destroy();
		Holder<XmlEngine>::destroy();
		Holder<LogEngine>::destroy();
		Holder<ProfilerEngine>::destroy();
		Holder<ScriptEngine>::destroy();

		CodecManager::cleanup();

		finalize();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::onWindowMovedOrResized()
	{
		m_renderEngine->onWindowMovedOrResized();
	}
	//////////////////////////////////////////////////////////////////////////
	const Resolution & Application::getCurrentResolution() const
	{
		return m_currentResolution;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::notifyWindowModeChanged( std::size_t _width, std::size_t _height, bool _fullscreen )
	{
		m_interface->notifyWindowModeChanged( _width, _height, _fullscreen );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::onActive( bool _active )
	{
		m_renderEngine->onWindowActive( _active );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setMouseBounded( bool _bounded )
	{
		if( m_mouseBounded != _bounded )
		{
			if( !m_renderEngine->getFullscreenMode() )
			{
				m_inputEngine->setMouseBounded( _bounded );
				m_interface->setHandleMouse( !_bounded );
			}
			m_mouseBounded = _bounded;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setFullscreenMode( bool _fullscreen )
	{
		if( !m_mouseBounded && m_renderEngine->isWindowCreated() )
		{
			if( !_fullscreen )
			{
				m_inputEngine->setMouseBounded( false );
				m_interface->setHandleMouse( true );
				m_game->handleMouseEnter();	
			}
			else
			{
				m_inputEngine->setMouseBounded( true );
				m_interface->setHandleMouse( false );
			}
		}

		m_renderEngine->setFullscreenMode( _fullscreen );
	}
	//////////////////////////////////////////////////////////////////////////
	const Resolution & Application::getDesktopResolution() const
	{
		return m_desktopResolution;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int Application::getDebugMask() const
	{
		return m_debugMask;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::showMessageBox( const String& _message, const String& _header, unsigned int _style )
	{
		m_interface->showMessageBox( _message, _header, _style );
	}
	//////////////////////////////////////////////////////////////////////////
	StringA Application::WToA( const StringW& _stringw ) const
	{
		return m_interface->WToA( _stringw );
	}
	//////////////////////////////////////////////////////////////////////////
	StringW Application::AToW( const StringA& _stringa ) const
	{
		return m_interface->AToW( _stringa );
	}
	//////////////////////////////////////////////////////////////////////////
}
