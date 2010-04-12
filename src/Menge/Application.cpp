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

#	include "Game.h"

#	include "Logger/Logger.h"

#	include "XmlEngine.h"

#	include "NodeManager.h"

#	include "TextManager.h"
#	include "TextField.h"

#	include "ThreadManager.h"
#	include "TaskManager.h"

#	include "CodecEngine.h"

#	include "ResourceManager.h"
#	include "AlphaChannelManager.h"

#	include "Player.h"
#	include "Scene.h"

#	include "Factory/FactoryDefault.h"
#	include "Factory/FactoryPool.h"

// All Node type
#	include "Entity.h"
#	include "Animation.h"
#	include "Arrow.h"
#	include "ParticleEmitter.h"
#	include "HotSpot.h"
#	include "Light2D.h"
#	include "ShadowCaster2D.h"
#	include "TilePolygon.h"
#	include "Point.h"
#	include "SoundEmitter.h"
#	include "Sprite.h"
#	include "TextField.h"
#	include "TileMap.h"
#	include "Track.h"
#	include "Video.h"
#	include "Layer2D.h"
#	include "Layer2DLoop.h"
#	include "LayerScene.h"
#	include "RenderMesh.h"
#	include "Camera2D.h"
#	include "Camera3D.h"
#	include "Layer2DAccumulator.h"
#	include "Layer3D.h"
#	include "Window.h"
#	include "HotSpotImage.h"
#	include "Mesh_40_30.h"
#	include "Layer2DTexture.h"
#	include "Collision.h"

// All Resource type
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
#	include "ResourceWindow.h"
#	include "ResourceHotspotImage.h"

#	include <locale.h>
#	include <ctime>
#	include <sstream>
#	include <iomanip>

#	include "VersionInfo.h"
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Application::Application( ApplicationInterface* _interface
								, Logger * _logger
								, const String& _userPath
								, const String& _scriptInitParams )
		: m_interface(_interface)
		, m_logger(_logger)
		, m_scriptInitParams(_scriptInitParams)
		, m_particles( true )
		, m_sound( true )
		, m_debugMask( 0 )
		, m_phycisTiming(0.f)
		, m_resetTiming( false )
		, m_maxTiming( 100.0f )
		, m_fileEngine( 0 )
		, m_renderEngine( 0 )
		, m_soundEngine( 0 )
		, m_particleEngine(0)
		, m_inputEngine(0)
		, m_physicEngine2D( 0 )
		, m_physicEngine( 0 )
		, m_xmlEngine( 0 )
		, m_mouseBounded( false )
		, m_game( 0 )
		, m_focus( true )
		, m_update( true )
		, m_enableDebug( false )
		, m_userPath( _userPath )
		, m_console(NULL)
		, m_scriptEngine(NULL)
		, m_fileLog( NULL )
		, m_threadManager( NULL )
		, m_taskManager( NULL )
		, m_alreadyRunningPolicy( 0 )
		, m_allowFullscreenSwitchShortcut( true )
		, m_resourceManager( NULL )
		, m_alphaChannelManager( NULL )
		, m_codecEngine( NULL )
		, m_textManager( NULL )
		, m_createRenderWindow( false )
		, m_cursorMode( false )
		, m_invalidateVsync( false )
		, m_invalidateCursorMode( false )
		, m_fullscreen(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Application::~Application()
	{
		finalize();

	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		class ExecuteInitialize
		{
		protected:
			typedef bool (Application::*TInitializeMethod)();
			typedef std::list<TInitializeMethod> TListInitializeMethods;

		public:
			ExecuteInitialize( Application * _app )
				: m_app(_app)
			{

			}

		public:
			void add( TInitializeMethod _method )
			{
				m_initializators.push_back( _method );
			}

		public:
			bool run() const
			{
				TListInitializeMethods::const_iterator it_found = 
					std::find_if( m_initializators.begin(), m_initializators.end(), *this );

				return it_found == m_initializators.end();
			}

		public:
			bool operator() ( TInitializeMethod _method ) const
			{
				bool result = (m_app->*_method)();

				return result == false;
			}

		protected:
			Application * m_app;
			TListInitializeMethods m_initializators;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initialize( const String& _applicationFile, const String & _args, bool _loadPersonality )
	{
		parseArguments_( _args );

		m_applicationFile = _applicationFile;

		ExecuteInitialize exinit( this );
		
		exinit.add( &Application::initializeThreadManager_);
		exinit.add( &Application::initializeFileEngine_);
		exinit.add( &Application::initializeLogEngine_);
		exinit.add( &Application::initializeParticleEngine_);
		exinit.add( &Application::initializePhysicEngine2D_);
		exinit.add( &Application::initializeRenderEngine_);
		exinit.add( &Application::initializeSoundEngine_);
		exinit.add( &Application::initializeTaskManager_);
		exinit.add( &Application::initializeNodeManager_);
		exinit.add( &Application::initializeXmlEngine_);
		exinit.add( &Application::initializeScriptEngine_);
		exinit.add( &Application::initializeCodecEngine_);
		exinit.add( &Application::initalizeResourceManager_);
		exinit.add( &Application::initializeAlphaChannelManager_);
		exinit.add( &Application::initializeTextManager_);

		if( exinit.run() == false )
		{
			return false;
		}

		loadGame( _loadPersonality );

		//if( m_console != NULL )
		//{
		//	m_console->inititalize( m_logSystem );
		//}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeThreadManager_()
	{
		MENGE_LOG( "Initializing Thread System..." );
		m_threadManager = new ThreadManager();
		if( m_threadManager->initialize() == false )
		{
			MENGE_LOG_ERROR("Fatal error: (Application::initialize) Failed to initialize TreadManager");
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeFileEngine_()
	{
		MENGE_LOG( "Inititalizing File System..." );
		m_fileEngine = new FileEngine();
		if( m_fileEngine->initialize() == false )
		{
			MENGE_LOG_ERROR("Fatal error: (Application::initialize) Failed to initialize FileEngine");
			return false;
		}

		// mount root
		if( m_fileEngine->mountFileSystem( "", "./", false ) == false )
		{
			MENGE_LOG_ERROR( "Error: failed to mount root directory" );
			return false;
		}

		// mount user directory
		if( m_fileEngine->mountFileSystem( "user", m_userPath, true ) == false )
		{
			MENGE_LOG_ERROR( "Error: failed to mount user directory" );
			//return false; //WTF???
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeLogEngine_()
	{
		String logFilename = "Game";

		if( m_enableDebug == true )
		{
			std::stringstream dateStream;
			std::time_t ctTime; 
			std::time(&ctTime);
			std::tm* sTime = std::localtime( &ctTime );
			dateStream << 1900 + sTime->tm_year << "_" << std::setw(2) << std::setfill('0') <<
				(sTime->tm_mon+1) << "_" << std::setw(2) << std::setfill('0') << sTime->tm_mday << "_"
				<< std::setw(2) << std::setfill('0') << sTime->tm_hour << "_" 
				<< std::setw(2) << std::setfill('0') << sTime->tm_min << "_"
				<< std::setw(2) << std::setfill('0') << sTime->tm_sec;

			String dateString = dateStream.str();
			logFilename += "_";
			logFilename += dateString;
		}
		logFilename += ".log";

		m_fileLog = m_fileEngine->openFileOutput( "user", logFilename );
		if( m_fileLog != NULL )
		{
			m_logger->registerLogger( m_fileLog );
			m_logger->logMessage( "Starting log to Menge.log\n" );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeParticleEngine_()
	{
#	if	MENGE_PARTICLES	== (1)
		MENGE_LOG( "Initializing Particle System..." );
		m_particleEngine = new ParticleEngine();
		if( m_particleEngine->initialize() == false )
		{
			MENGE_LOG_ERROR("Fatal error: (Application::initialize) Failed to initialize ParticleEngine");
			return false;
		}
#	endif

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializePhysicEngine2D_()
	{
		MENGE_LOG( "Inititalizing Physics2D System..." );
		m_physicEngine2D = new PhysicEngine2D();
		if( m_physicEngine2D->initialize() == false )
		{
			MENGE_LOG_ERROR("Fatal error: (Application::initialize) Failed to initialize PhysicEngine2D");
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeRenderEngine_()
	{
		MENGE_LOG( "Initializing Render System..." );
		m_renderEngine = new RenderEngine();
		if( m_renderEngine->initialize( 4000 ) == false )
		{
			MENGE_LOG_ERROR("Fatal error: (Application::initialize) Failed to initialize RenderEngine");
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeSoundEngine_()
	{
		MENGE_LOG( "Initializing Sound System..." );
		m_soundEngine = new SoundEngine();
		if( m_soundEngine->initialize() == false )
		{
			MENGE_LOG_ERROR("Error: (Application::initialize) Failed to initialize SoundEngine");
			m_sound = false;
		}

		if( m_sound == false )
		{
			m_soundEngine->mute( true );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeTaskManager_()
	{
		m_taskManager = new TaskManager();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeNodeManager_()
	{
		m_nodeManager = new NodeManager();

#	define NODE_FACTORY( Type ) m_nodeManager->registerFactory( #Type, Helper::createFactoryPool<Type>() )

		MENGE_LOG( "Creating Object Factory..." );
		NODE_FACTORY( Node );
		NODE_FACTORY( Entity );
		NODE_FACTORY( Animation );
		NODE_FACTORY( Arrow );
		NODE_FACTORY( ParticleEmitter );
		NODE_FACTORY( HotSpot );
		NODE_FACTORY( Light2D );
		NODE_FACTORY( ShadowCaster2D );
		NODE_FACTORY( TilePolygon );
		NODE_FACTORY( Point );
		NODE_FACTORY( RigidBody2D );
		NODE_FACTORY( SoundEmitter );
		NODE_FACTORY( Sprite );
		NODE_FACTORY( TextField );
		NODE_FACTORY( TileMap );
		NODE_FACTORY( Track );
		NODE_FACTORY( Video );
		NODE_FACTORY( Layer2D );
		NODE_FACTORY( Layer2DLoop );
		NODE_FACTORY( Layer2DAccumulator );
		NODE_FACTORY( Layer3D );
		NODE_FACTORY( LayerScene );
		NODE_FACTORY( RenderMesh );
		NODE_FACTORY( Camera2D );
		NODE_FACTORY( Camera3D );
		NODE_FACTORY( SceneNode3D );
		NODE_FACTORY( Window );
		NODE_FACTORY( HotSpotImage );
		NODE_FACTORY( Mesh_40_30 );
		NODE_FACTORY( Layer2DTexture );
		NODE_FACTORY( Collision );
#	undef NODE_FACTORY

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeXmlEngine_()
	{
		MENGE_LOG( "Initializing Xml Engine..." );
		m_xmlEngine = new XmlEngine();

		if( m_xmlEngine->parseXmlFileM( "", m_applicationFile, this, &Application::loader ) == false )
		{
			MENGE_LOG_ERROR( "parse application xml failed '%s'"
				, m_applicationFile.c_str() 
				);

			showMessageBox( "Application files missing or corrupt", "Critical Error", 0 );
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeScriptEngine_()
	{
		MENGE_LOG( "Initializing Script Engine..." );

		m_scriptEngine = new ScriptEngine();
		m_scriptEngine->initialize();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeCodecEngine_()
	{
		MENGE_LOG( "Inititalizing Codecs..." );

		m_codecEngine = new CodecEngine();

		bool result = m_codecEngine->initialize();
		
		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initalizeResourceManager_()
	{
		m_resourceManager = new ResourceManager();
		
#	define RESOURCE_FACTORY( Type )\
	m_resourceManager->registerFactory( #Type , Helper::createFactoryPool<Type>() )

		MENGE_LOG( "Creating Resource Factory..." );
		RESOURCE_FACTORY( ResourceAnimation );
		//RESOURCE_FACTORY( ResourceCapsuleController );
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
		//RESOURCE_FACTORY( ResourceSkeleton );
		//RESOURCE_FACTORY( ResourcePhysicBoxGeometry );
		//RESOURCE_FACTORY( ResourcePhysicConcaveGeometry );
		//RESOURCE_FACTORY( ResourcePhysicConvexGeometry );
		RESOURCE_FACTORY( ResourcePlaylist );
		RESOURCE_FACTORY( ResourceSound );
		RESOURCE_FACTORY( ResourceTileMap );
		RESOURCE_FACTORY( ResourceTileSet );
		RESOURCE_FACTORY( ResourceMeshMS3D );
		RESOURCE_FACTORY( ResourceMeshNoise );
		//RESOURCE_FACTORY( ResourceMaterial );
		RESOURCE_FACTORY( ResourceWindow );
		RESOURCE_FACTORY( ResourceHotspotImage );

#	undef RESOURCE_FACTORY

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeAlphaChannelManager_()
	{
		m_alphaChannelManager = new AlphaChannelManager();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeTextManager_()
	{
		m_textManager = new TextManager();
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const String& Application::getPathGameFile() const
	{
		return m_gameInfo;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setBaseDir( const String & _dir )
	{
		m_baseDir = _dir;

		if( m_fileEngine )
		{
			m_fileEngine->setBaseDir( m_baseDir );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const String& Application::getBaseDir() const
	{
		return m_baseDir;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::loadGame( bool _loadPersonality )
	{
		m_game = new Game();

		MENGE_LOG( "Create game file '%s'"
			, m_gameInfo.c_str() );

		//m_fileEngine->loadPak( m_gamePack );
		if( m_fileEngine->mountFileSystem( m_gamePackName, m_gamePackPath, false ) == false )
		{
			MENGE_LOG_ERROR( "Error: (Application::loadGame) failed to mount GamePak '%s'",
				m_gamePackPath.c_str() 
				);

			return false;
		}

		if( m_xmlEngine->parseXmlFileM( m_gamePackName, m_gameInfo, m_game, &Game::loader ) == false )
		{
			MENGE_LOG_ERROR( "Invalid game file '%s'"
				, m_gameInfo.c_str()
				);

			showMessageBox( "Application files missing or corrupt", "Critical Error", 0 );
			return false;
		}

		if( m_baseDir.empty() )	// current dir
		{
			m_baseDir = ".";
		}

		m_game->setBaseDir( m_baseDir );
		if( m_languagePackOverride.empty() == false )
		{
			m_game->setLanguagePack( m_languagePackOverride );
		}
		m_game->loadConfigPaks();
		//m_game->registerResources( m_baseDir );
		
		String title = m_game->getTitle();
		
		m_fullscreen = m_game->getFullscreen();

		if( _loadPersonality )
		{
			if( m_game->loadPersonality() == false )
			{
				return false;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::createRenderWindow( WindowHandle _renderWindowHandle, WindowHandle _inputWindowHandle )
	{
		const String & title = m_game->getTitle();

		m_currentResolution = ( m_fullscreen == true )
			? this->getDesktopResolution() 
			: m_game->getResolution();

		int bits = m_game->getBits();
		int FSAAType = m_game->getFSAAType();
		int FSAAQuality = m_game->getFSAAQuality();
		bool textureFiltering = m_game->getTextureFiltering();

		m_createRenderWindow = m_renderEngine->createRenderWindow( m_currentResolution, bits, m_fullscreen,
														_renderWindowHandle, FSAAType, FSAAQuality );

		if( m_createRenderWindow == false )
		{
			showMessageBox( "Failed to create render window", "Critical Error", 0 );
			return false;
		}

		Viewport renderViewport = calcRenderViewport_( m_currentResolution );

		if( m_fullscreen == true )
		{
			m_interface->notifyCursorClipping( renderViewport );
		}
		else
		{
			m_interface->notifyCursorUnClipping();
		}

		m_renderEngine->setRenderViewport( renderViewport );

		m_renderEngine->enableTextureFiltering( textureFiltering );

		MENGE_LOG( "Initializing Input Engine..." );
		m_inputEngine = new InputEngine();
		bool result = m_inputEngine->initialize();
		if( result == true )
		{
			MENGE_LOG( "Input Engine successfully!" );
		}
		else
		{
			MENGE_LOG( "Input Engine initialization failed!" );
		}

		const Resolution & contentResolution = m_game->getContentResolution();

		m_inputEngine->setResolution( contentResolution );
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initGame()
	{
		if( m_game->init( m_scriptInitParams ) == false )
		{
			return false;
		}

		m_game->tick( 0.0f );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::loader( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "Application" )
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
			XML_CASE_ATTRIBUTE_NODE_METHOD( "BaseDir", "Value", &Application::setBaseDir );
			XML_CASE_NODE( "GamePack" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Name", m_gamePackName );
					XML_CASE_ATTRIBUTE( "Path", m_gamePackPath );
					XML_CASE_ATTRIBUTE( "Description", m_gameInfo );
				}
			}
			XML_CASE_ATTRIBUTE_NODE( "AlreadyRunningPolicy", "Value", m_alreadyRunningPolicy );
			XML_CASE_ATTRIBUTE_NODE( "AllowFullscreenSwitchShortcut", "Value", m_allowFullscreenSwitchShortcut );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::parseArguments_( const String& _arguments )
	{
		String::size_type idx = _arguments.find( "-sound" );
		if( idx != String::npos )
		{
			m_sound = false;
		}

		idx = _arguments.find( "-particles" );
		if( idx != String::npos )
		{
			m_particles = false;
		}

	}
	//////////////////////////////////////////////////////////////////////////
	void Application::exec( const Menge::String & _text )
	{
		m_scriptEngine->exec( _text );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::registerConsole( ConsoleInterface * _console )
	{
		m_console = _console;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::onKeyEvent( unsigned int _key, unsigned int _char, bool _isDown )
	{
		if( m_console != NULL )
		{
			m_console->proccessInput( _key, _char, _isDown );
		}

#	ifndef MENGE_MASTER_RELEASE
		if( _key == KC_F12 && _isDown && m_enableDebug )
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

		if( _key == KC_F10 && _isDown && m_enableDebug)
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

		if( _key == KC_F9 && _isDown && m_enableDebug)
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

		if( _key == KC_F8 && _isDown && m_enableDebug)
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

		if( _key == KC_F5 && _isDown && m_enableDebug )
		{
			m_resourceManager->dumpResources("Application");
		}

		if( _key == KC_F11 && _isDown && m_enableDebug )
		{
			Player::hostage()
				->toggleDebugText();
		}

#	endif

		return m_game->handleKeyEvent( _key, _char, _isDown );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::onMouseButtonEvent( int _button, bool _isDown )
	{
		bool result = m_game->handleMouseButtonEvent( _button, _isDown );
		m_game->handleMouseButtonEventEnd( _button, _isDown );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::onMouseMove( float _dx, float _dy, int _whell )
	{
		return m_game->handleMouseMove( _dx, _dy, _whell );
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
		m_renderEngine->onWindowClose();
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
	bool Application::isFocus() const
	{
		return m_focus;
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
		//return;
		if( m_focus == _focus ) 
		{
			return;
		}

		m_focus = _focus;

		m_soundEngine->onFocus( _focus );

		if( m_game != NULL )
		{
			m_game->onFocus( m_focus );
		}
		/*if( m_focus == true )
		{
			m_update = true;
		}*/
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::minimizeWindow()
	{
		m_interface->minimizeWindow();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::onUpdate()
	{		
		if( !m_update && !m_focus ) 
		{
			m_taskManager->update();
			return false;
		}

		m_game->update();

		m_inputEngine->update();

		m_taskManager->update();

		if( !m_focus && m_update )
		{
			m_update = false;
		}
		else if( m_focus && !m_update )
		{
			m_update = true;
		}

		this->updateNotification();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::onTick( float _timing )
	{
		float timing = _timing;

		if( _timing > m_maxTiming )
		{
			timing = m_maxTiming;
		}

		m_game->tick( timing );

		m_soundEngine->update( _timing );


		//if( m_physicEngine )
		//{
		//	m_physicEngine->update( _timing );
		//}

		if( m_physicEngine2D )
		{
			m_physicEngine2D->update( timing );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::onRender()
	{
		if( m_renderEngine->beginScene() == false )
		{
			return false;
		}

		bool immediatelyFlush = m_game->render( m_debugMask );

		if( m_console != NULL )
		{
			m_console->render();
		}
		//Holder<Console>::hostage()->render();

		m_renderEngine->endScene();

		if( immediatelyFlush == true )
		{
			this->onFlush();

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::onFlush()
	{
		m_renderEngine->swapBuffers();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::onClose()
	{
		bool needQuit = true;
		if( m_game != NULL )
		{
			needQuit = m_game->onClose();
		}
		if( needQuit == true )
		{
			quit();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::finalize()
	{
		unloadPlugins_();

		delete m_game;

		delete m_textManager;
		delete m_nodeManager;

		delete m_taskManager;

		delete m_alphaChannelManager;
		delete m_resourceManager;
		delete m_scriptEngine;

		delete m_physicEngine;
		delete m_physicEngine2D;
		delete m_particleEngine;
		delete m_renderEngine;
		delete m_inputEngine;
		delete m_soundEngine;
		
		delete m_codecEngine;

		delete m_xmlEngine;

		if( m_fileLog != NULL )
		{
			m_logger->unregisterLogger( m_fileLog );
			m_fileEngine->closeFileOutput( m_fileLog );
			m_fileLog = NULL;
		}

		delete m_fileEngine;
		delete m_threadManager;
		//		releaseInterfaceSystem( m_profilerSystem );
	}
	//////////////////////////////////////////////////////////////////////////
	const Resolution & Application::getCurrentResolution() const
	{
		return m_currentResolution;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setMouseBounded( bool _bounded )
	{
		m_mouseBounded = _bounded;
		if( m_fullscreen == false )	// don't override fullscreen behavior
		{
			if( m_mouseBounded == true )
			{
				const Viewport& viewport = m_renderEngine->getRenderViewport();
				m_interface->notifyCursorClipping( viewport );
			}
			else
			{
				m_interface->notifyCursorUnClipping();
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::getMouseBounded() const
	{
		return m_mouseBounded;
	}
	//////////////////////////////////////////////////////////////////////////
	Viewport Application::calcRenderViewport_( const Resolution & _resolution )
	{
		const Resolution & contentResolution = Game::hostage()
			->getContentResolution();

		Viewport renderViewport;

		float rx = float( _resolution.getWidth());
		float ry = float( _resolution.getHeight());

		if( _resolution == contentResolution )
		{
			renderViewport.begin.x = 0.0f;
			renderViewport.begin.y = 0.0f;
			renderViewport.end.x = rx;
			renderViewport.end.y = ry;
		}
		else
		{
			float one_div_width = 1.f / rx;
			float one_div_height = 1.f / ry;

			float crx = float( contentResolution.getWidth() );
			float cry = float( contentResolution.getHeight() );

			float contentAspect = crx / cry;
			float aspect = rx * one_div_height;

			float dw = 1.0f;
			float dh = rx / contentAspect * one_div_height;

			if( dh > 1.0f )
			{
				dh = 1.0f;
				dw = ry * contentAspect * one_div_width;
			}

			float areaWidth = dw * rx;
			float areaHeight = dh * ry;

			renderViewport.begin.x = ( rx - areaWidth ) * 0.5f;
			renderViewport.begin.y = ( ry - areaHeight ) * 0.5f;
			renderViewport.end.x = renderViewport.begin.x + areaWidth;
			renderViewport.end.y = renderViewport.begin.y + areaHeight;
		}

		return renderViewport;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setFullscreenMode( bool _fullscreen )
	{
		if( m_fullscreen == _fullscreen )
		{
			return;
		}

		m_fullscreen = _fullscreen;

		m_currentResolution = ( m_fullscreen == true )
			? this->getDesktopResolution() 
			: m_game->getResolution();

		m_interface->notifyWindowModeChanged( m_currentResolution, m_fullscreen );
		
		Viewport renderViewport = calcRenderViewport_( m_currentResolution );
		m_renderEngine->setRenderViewport( renderViewport );

		if( m_fullscreen == true )
		{
			m_interface->notifyCursorClipping( renderViewport );
		}
		else
		{
			//m_interface->notifyCursorUnClipping();
			setMouseBounded( m_mouseBounded );
		}

		m_renderEngine->changeWindowMode( m_currentResolution, _fullscreen );

		m_game->onFullscreen( m_fullscreen );

		if( !m_mouseBounded && m_renderEngine->isWindowCreated() )
		{
			if( _fullscreen == false )
			{
				m_currentResolution = m_game->getResolution();
			}
			else
			{
				m_currentResolution = m_desktopResolution;
			}
			m_game->handleMouseEnter();	
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::screenshot( Texture* _renderTargetImage, const mt::vec4f & _rect )
	{
		const Resolution & contentResolution = m_game->getContentResolution();

		mt::vec4f res = _rect;
		res.x *= static_cast<float>( m_currentResolution.getWidth() ) / contentResolution.getWidth();
		res.y *= static_cast<float>( m_currentResolution.getHeight() ) / contentResolution.getHeight();
		res.z *= static_cast<float>( m_currentResolution.getWidth() ) / contentResolution.getWidth();
		res.w *= static_cast<float>( m_currentResolution.getHeight() ) / contentResolution.getHeight();
		
		m_renderEngine->screenshot( _renderTargetImage, res );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::getFullscreenMode() const
	{
		return m_fullscreen;
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
	void Application::onPaint()
	{
		if( /*m_focus == true &&*/ m_renderEngine && m_game && m_createRenderWindow == true )
		{
			if( m_renderEngine->beginScene() == true )
			{
				m_game->render( m_debugMask );
				m_renderEngine->endScene();
				m_renderEngine->swapBuffers();
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	String Application::getProjectTitle() const
	{
		return m_game->getTitle();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setDesktopResolution( const Resolution& _resolution )
	{
		m_desktopResolution = _resolution;
	}
	//////////////////////////////////////////////////////////////////////////
	Menge::String Application::ansiToUtf8( const String& _ansi )
	{
		return m_interface->ansiToUtf8( _ansi );
	}
	//////////////////////////////////////////////////////////////////////////
	Menge::String Application::utf8ToAnsi( const String& _utf8 )
	{
		return m_interface->utf8ToAnsi( _utf8 );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::getHasWindowPanel() const
	{
		return m_game->getHasWindowPanel();
	}
	//////////////////////////////////////////////////////////////////////////
	const char* Application::getVersionInfo()
	{
		return s_versionInfo;
	}
	//////////////////////////////////////////////////////////////////////////
	const Resolution & Application::getResolution() const
	{
		return m_game->getResolution();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::enableDebug( bool _enable )
	{
		m_enableDebug = _enable;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::loadPlugins_( const String& _pluginsFolder )
	{
		loadPlugin_("DebugConsole.dll");
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::loadPlugin_( const String& _pluginName )
	{
        DynamicLibraryInterface * lib = m_interface->load( _pluginName );

		m_plugins.push_back( lib );

		TFunctionPtr function =
			lib->getSymbol("dllStartPlugin");

		if ( function )
		{
			function( this );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::unloadPlugins_()
    {
		for ( TPluginVec::reverse_iterator it = m_plugins.rbegin(); it != m_plugins.rend(); ++it )
		{
			TFunctionPtr function =
				(*it)->getSymbol("dllShutdownPlugin");

			if ( function )
			{
				function( this );
			}

			m_interface->unload( *it );			
		}

        m_plugins.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setMousePosition( int _x, int _y )
	{
		if( m_inputEngine )
		{
			m_inputEngine->setMousePosition( _x, _y );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setLanguagePack( const String& _packName )
	{
		m_languagePackOverride = _packName;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::getVSync() const
	{
		if( m_renderEngine )
		{
			return m_renderEngine->getVSync();
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setMaxClientAreaSize( size_t _maxWidth, size_t _maxHeight )
	{
		m_maxClientAreaSize[0] = _maxWidth;
		m_maxClientAreaSize[1] = _maxHeight;
	}
	//////////////////////////////////////////////////////////////////////////
	const Resolution& Application::getMaxClientAreaSize() const
	{
		return m_maxClientAreaSize;
	}
	//////////////////////////////////////////////////////////////////////////
	int Application::getAlreadyRunningPolicy() const
	{
		return m_alreadyRunningPolicy;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::getAllowFullscreenSwitchShortcut() const
	{
		return m_allowFullscreenSwitchShortcut;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setLoggingLevel( EMessageLevel _level )
	{
		m_logger->setVerboseLevel( _level );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::logMessage( const String& _message, EMessageLevel _level )
	{
		m_logger->logMessage( _message, _level );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setVSync( bool _vsync )
	{
		if( m_vsync == _vsync )
		{
			return;
		}

		m_vsync = _vsync;
		m_invalidateVsync = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::updateNotification()
	{
		if( m_invalidateVsync == true )
		{
			if( m_renderEngine != NULL )
			{
				m_renderEngine->setVSync( m_vsync );

			}
			m_interface->notifyVsyncChanged( m_vsync );
			m_invalidateVsync = false;
		}

		if( m_invalidateCursorMode == true )
		{
			m_interface->notifyCursorModeChanged( m_cursorMode );
			m_invalidateCursorMode = false;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setCursorMode( bool _mode )
	{
		m_cursorMode = _mode;
		if( m_game != NULL )
		{
			m_game->setCursorMode( _mode );
		}
		m_invalidateCursorMode = true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::getCursorMode() const
	{
		return m_cursorMode;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::pushKeyEvent( unsigned int _key, unsigned int _char, bool _isDown )
	{
		if( m_inputEngine != NULL )
		{
			m_inputEngine->pushKeyEvent( _key, _char, _isDown );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::pushMouseButtonEvent( int _button, bool _isDown )
	{
		if( m_inputEngine != NULL )
		{
			m_inputEngine->pushMouseButtonEvent( _button, _isDown );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::pushMouseMoveEvent( int _x, int _y, int _z )
	{
		if( m_inputEngine != NULL )
		{
			m_inputEngine->pushMouseMoveEvent( _x, _y, _z );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}

