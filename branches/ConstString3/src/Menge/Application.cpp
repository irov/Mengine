#	include "Application.h"

#	include "InputEngine.h"
#	include "InputHandler.h"

#	include "ServiceProvider.h"
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

#	include "Consts.h"

#	include "LogEngine.h"

#	include "LoaderEngine.h"
#	include "BinParser.h"

#	include "NodeManager.h"

#	include "TextManager.h"
#	include "TextField.h"

#	include "ThreadManager.h"
#	include "TaskManager.h"

#	include "CodecEngine.h"

#	include "ResourceManager.h"
#	include "AlphaChannelManager.h"
#	include "ArrowManager.h"
#	include "AccountManager.h"
#	include "SceneManager.h"
#	include "EntityManager.h"
#	include "ParamManager.h"

#	include "Player.h"
#	include "Scene.h"

#	include "FactoryDefault.h"
#	include "FactoryPool.h"

#	include "FileLogger.h"

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
#	include "Movie.h"
#	include "Model.h"
#	include "Video.h"
#	include "Layer2D.h"
#	include "Layer2DLoop.h"
#	include "RenderMesh.h"
#	include "Camera2D.h"
#	include "Camera3D.h"
#	include "Layer2DAccumulator.h"
#	include "Layer3D.h"
#	include "Window.h"
#	include "HotSpotImage.h"
#	include "Mesh_40_30.h"
#	include "Layer2DTexture.h"

// All Resource type
#	include "ResourceSequence.h"
#	include "ResourceAnimation.h"
#	include "ResourceEmitterContainer.h"
#	include "ResourceFont.h"
#	include "ResourceGlyph.h"
#	include "ResourceTilePolygon.h"
#	include "ResourceImageAtlas.h"
#	include "ResourceImageCell.h"
#	include "ResourceImageDefault.h"
#	include "ResourceImageDynamic.h"
#	include "ResourceImageSet.h"
#	include "ResourceBinary.h"
#	include "ResourceMovie.h"
#	include "ResourceVideo.h"
#	include "ResourceMesh.h"
#	include "ResourceSkeleton.h"
#	include "ResourcePlaylist.h"
#	include "ResourceSound.h"
#	include "ResourceTileMap.h"
#	include "ResourceTileSet.h"
#	include "ResourceModel.h"
//#	include "ResourceMeshMS3D.h"
//#	include "ResourceMeshNoise.h"
#	include "ResourceMaterial.h"
#	include "ResourceWindow.h"
#	include "ResourceHotspotImage.h"
#	include "ResourceCursorICO.h"
#	include "ResourceInternalObject.h"

//extern "C"
//{
//	#	include <iniparser/src/iniparser.h>
//}

#	include <locale.h>
#	include <ctime>
#	include <sstream>
#	include <iomanip>

#	include "VersionInfo.h"

extern bool initPluginMengeImageCodec( Menge::PluginInterface ** _plugin );

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Application::Application( ApplicationInterface* _interface, LogSystemInterface * _logSystem, const String & _applicationPath, const String& _userPath, const String & _platformName )
		: m_interface(_interface)
		, m_logSystem(_logSystem)
		, m_particles(true)
		, m_sound(true)
		, m_debugMask(0)
		, m_phycisTiming(0.f)
		, m_resetTiming(false)
		, m_maxTiming(100.f)
		, m_logEngine(0)
		, m_fileEngine(0)
		, m_renderEngine(0)
		, m_soundEngine(0)
		, m_particleEngine(0)
		, m_inputEngine(0)
		//, m_physicEngine2D(0)
		, m_loaderEngine(0)
		, m_mouseBounded(false)
		, m_game(0)
		, m_focus(true)
		, m_update(true)
		, m_enableDebug(false)
		, m_applicationPath(_applicationPath)
		, m_userPath(_userPath)
		, m_console(NULL)
		, m_scriptEngine(NULL)
		, m_threadManager(NULL)
		, m_taskManager(NULL)
		, m_alreadyRunningPolicy(0)
		, m_allowFullscreenSwitchShortcut(true)
		, m_resourceManager(NULL)
		, m_alphaChannelManager(NULL)
		, m_codecEngine(NULL)
		, m_textManager(NULL)
		, m_createRenderWindow(false)
		, m_cursorMode(false)
		, m_invalidateVsync(false)
		, m_invalidateCursorMode(false)
		, m_fullscreen(false)
		, m_fileLog(NULL)
		, m_nodeManager(0)
		, m_debugCRT(false)
		, m_inputMouseButtonEventBlock(false)
		, m_platformName(_platformName)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Application::~Application()
	{
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
	bool Application::initialize( const String& _applicationFile, const String & _args )
	{
		parseArguments_( _args );

		m_consts = new Consts;
		Consts::keep(m_consts);

		m_serviceProvider = new ServiceProvider;

		ExecuteInitialize exinit( this );
		
		exinit.add( &Application::initializeLogEngine_);
		exinit.add( &Application::initializeFileEngine_);		
		exinit.add( &Application::initializeThreadManager_);
		exinit.add( &Application::initializeParticleEngine_);
		exinit.add( &Application::initializePhysicEngine2D_);
		exinit.add( &Application::initializeRenderEngine_);
		exinit.add( &Application::initializeSoundEngine_);
		exinit.add( &Application::initializeTaskManager_);
		exinit.add( &Application::initializeNodeManager_);
		exinit.add( &Application::initializeLoaderEngine_);
		exinit.add( &Application::initializeScriptEngine_);
		exinit.add( &Application::initializeCodecEngine_);
		exinit.add( &Application::initializeResourceManager_);
		exinit.add( &Application::initializeArrowManager_);
		exinit.add( &Application::initializeSceneManager_);
		exinit.add( &Application::initializeEntityManager_);		
		exinit.add( &Application::initializeAlphaChannelManager_);
		exinit.add( &Application::initializeTextManager_);
		exinit.add( &Application::initializeParamManager_);

		if( exinit.run() == false )
		{
			return false;
		}

		//extern initPlugin initPluginMengeImageCodec;
		{
			MENGE_LOG_INFO( "load Image Codec..." );

			PluginInterface * plugin;
			initPluginMengeImageCodec( &plugin );

			TMapParam param;
			plugin->initialize( m_serviceProvider, param );

			m_plugins.push_back( plugin );
		}

#	ifndef MENGE_MASTER_RELEASE
		{
			MENGE_LOG_INFO( "load Xml Codec..." );

			TMapParam param;
			this->loadPlugin("MengeXmlCodecPlugind.dll", param);
		}
#	endif


		//if( m_console != NULL )
		//{
		//	m_console->inititalize( m_logSystem );
		//}

		MENGE_LOG_INFO( "load Application %s ..."
			, _applicationFile.c_str()
			);

		bool exist = false;
		if( m_loaderEngine
			->load( Consts::get()->c_builtin_empty, _applicationFile, this, exist ) == false )
		{
			MENGE_LOG_ERROR( "parse application xml failed '%s'"
				, _applicationFile.c_str() 
				);

			showMessageBox( "'Application' file missing or corrupt", "Critical Error", 0 );

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeThreadManager_()
	{
		MENGE_LOG_INFO( "Initializing Thread System..." );
		m_threadManager = new ThreadManager();
		
		ThreadManager::keep(m_threadManager);

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
		MENGE_LOG_INFO( "Inititalizing File System..." );
		m_fileEngine = new FileEngine();
		
		FileEngine::keep( m_fileEngine );

		if( m_fileEngine->initialize() == false )
		{
			MENGE_LOG_ERROR("Fatal error: (Application::initialize) Failed to initialize FileEngine");
			return false;
		}

		// mount root
		if( m_fileEngine->mountFileSystem( Consts::get()->c_builtin_empty, m_applicationPath, Consts::get()->c_dir, false ) == false )
		{
			MENGE_LOG_ERROR( "Error: failed to mount root directory" );
			return false;
		}

		// mount user directory
		if( m_fileEngine->mountFileSystem( Consts::get()->c_user, m_userPath, Consts::get()->c_dir, true ) == false )
		{
			MENGE_LOG_ERROR( "Error: failed to mount user directory" );
			//return false; //WTF???
		}

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

		FileOutputInterface* fileLogInterface = m_fileEngine->openOutputFile( Consts::get()->c_user, logFilename );
		m_fileLog = new FileLogger();
		m_fileLog->setFileInterface( fileLogInterface );

		if( fileLogInterface != NULL )
		{
			m_logEngine->registerLogger( m_fileLog );

			MENGE_LOG_INFO("Starting log to %s"
				, logFilename.c_str()
				);
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeLogEngine_()
	{
		m_logEngine = new LogEngine( m_logSystem );

		LogEngine::keep( m_logEngine );

		m_serviceProvider->registryService( "Log", m_logEngine );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeParticleEngine_()
	{
#	if	MENGE_PARTICLES	== (1)
		MENGE_LOG_INFO( "Initializing Particle System..." );
		m_particleEngine = new ParticleEngine();

		ParticleEngine::keep( m_particleEngine );

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
		//MENGE_LOG_INFO( "Inititalizing Physics2D System..." );
		//m_physicEngine2D = new PhysicEngine2D();

		//PhysicEngine2D::keep( m_physicEngine2D );

		//if( m_physicEngine2D->initialize() == false )
		//{
		//	MENGE_LOG_ERROR("Fatal error: (Application::initialize) Failed to initialize PhysicEngine2D");
		//	return false;
		//}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeRenderEngine_()
	{
		MENGE_LOG_INFO( "Initializing Render System..." );
		m_renderEngine = new RenderEngine();

		RenderEngine::keep( m_renderEngine );

		if( m_renderEngine->initialize( 32000 ) == false )
		{
			MENGE_LOG_ERROR("Fatal error: (Application::initialize) Failed to initialize RenderEngine");
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeSoundEngine_()
	{
		MENGE_LOG_INFO( "Initializing Sound System..." );
		m_soundEngine = new SoundEngine();

		SoundEngine::keep( m_soundEngine );

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

		TaskManager::keep(m_taskManager);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeNodeManager_()
	{		
		m_nodeManager = new NodeManager();

		NodeManager::keep(m_nodeManager);

#	define NODE_FACTORY( Type )\
	do{\
		ConstString c_##Type(#Type);\
		m_nodeManager->registerFactory( c_##Type, Helper::createFactoryDefault<Type>() );\
	} while(false)

		MENGE_LOG_INFO( "Creating Object Factory..." );

		NODE_FACTORY( Node );

		NODE_FACTORY( Entity );
		NODE_FACTORY( Arrow );
		NODE_FACTORY( Scene );

		NODE_FACTORY( Animation );
		
		NODE_FACTORY( ParticleEmitter );
		NODE_FACTORY( HotSpot );
		//NODE_FACTORY( Light2D );
		//NODE_FACTORY( ShadowCaster2D );
		//NODE_FACTORY( TilePolygon );
		NODE_FACTORY( Point );
		//NODE_FACTORY( RigidBody2D );
		NODE_FACTORY( SoundEmitter );
		NODE_FACTORY( Sprite );
		NODE_FACTORY( TextField );
		//NODE_FACTORY( TileMap );
		NODE_FACTORY( Track );
		NODE_FACTORY( Movie );
		NODE_FACTORY( Model );
		NODE_FACTORY( Video );
		NODE_FACTORY( Layer2D );
		//NODE_FACTORY( Layer2DLoop );
		//NODE_FACTORY( Layer2DAccumulator );
		//NODE_FACTORY( Layer2DTexture );
		//NODE_FACTORY( LayerScene );
		//NODE_FACTORY( RenderMesh );
		NODE_FACTORY( Camera2D );
		//NODE_FACTORY( Camera3D );
		//NODE_FACTORY( SceneNode3D );
		NODE_FACTORY( Window );
		NODE_FACTORY( HotSpotImage );
		//NODE_FACTORY( Mesh_40_30 );
#	undef NODE_FACTORY

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeLoaderEngine_()
	{
		MENGE_LOG_INFO( "Initializing Loader Engine..." );

		m_loaderEngine = new LoaderEngine();

		LoaderEngine::keep( m_loaderEngine );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeScriptEngine_()
	{
		MENGE_LOG_INFO( "Initializing Script Engine..." );

		m_scriptEngine = new ScriptEngine();

		ScriptEngine::keep( m_scriptEngine );

		m_scriptEngine->initialize();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeCodecEngine_()
	{
		MENGE_LOG_INFO( "Inititalizing Codecs..." );

		m_codecEngine = new CodecEngine();

		CodecEngine::keep( m_codecEngine );

		m_serviceProvider->registryService( "Codec", m_codecEngine );
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeSceneManager_()
	{
		MENGE_LOG_INFO( "initialize Scene Manager..." );

		m_sceneManager = new SceneManager();

		SceneManager::keep(m_sceneManager);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeEntityManager_()
	{
		MENGE_LOG_INFO( "initialize Entity Manager..." );

		m_entityManager = new EntityManager();

		EntityManager::keep(m_entityManager);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeResourceManager_()
	{
		m_resourceManager = new ResourceManager();

		ResourceManager::keep( m_resourceManager );
		
#	define RESOURCE_FACTORY( Type ) \
	do{\
		ConstString c_##Type(#Type);\
		m_resourceManager->registerFactory( c_##Type, Helper::createFactoryDefault<Type>() );\
	} while(false)


		MENGE_LOG_INFO( "Creating Resource Factory..." );
		RESOURCE_FACTORY( ResourceSequence );
		RESOURCE_FACTORY( ResourceAnimation );
		RESOURCE_FACTORY( ResourceEmitterContainer );
		RESOURCE_FACTORY( ResourceFont );
		RESOURCE_FACTORY( ResourceBinary );
		RESOURCE_FACTORY( ResourceImageAtlas );
		RESOURCE_FACTORY( ResourceTilePolygon );
		RESOURCE_FACTORY( ResourceImageCell );
		RESOURCE_FACTORY( ResourceImageDefault );
		RESOURCE_FACTORY( ResourceImageDynamic );
		RESOURCE_FACTORY( ResourceImageSet );
		RESOURCE_FACTORY( ResourceMovie );
		RESOURCE_FACTORY( ResourceVideo );
		//RESOURCE_FACTORY( ResourceMesh );
		RESOURCE_FACTORY( ResourcePlaylist );
		RESOURCE_FACTORY( ResourceSound );
		RESOURCE_FACTORY( ResourceGlyph );
		RESOURCE_FACTORY( ResourceModel );
		//RESOURCE_FACTORY( ResourceTileMap );
		//RESOURCE_FACTORY( ResourceTileSet );
		//RESOURCE_FACTORY( ResourceMeshMS3D );
		//RESOURCE_FACTORY( ResourceMeshNoise );
		//RESOURCE_FACTORY( ResourceMaterial );
		RESOURCE_FACTORY( ResourceWindow );
		RESOURCE_FACTORY( ResourceHotspotImage );
		RESOURCE_FACTORY( ResourceCursorICO );

		RESOURCE_FACTORY( ResourceInternalObject );

#	undef RESOURCE_FACTORY

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeParamManager_()
	{
		MENGE_LOG_INFO( "initialize Param Manager..." );

		m_paramManager = new ParamManager();

		ParamManager::keep( m_paramManager );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeArrowManager_()
	{
		MENGE_LOG_INFO( "initialize Arrow Manager..." );

		m_arrowManager = new ArrowManager();

		ArrowManager::keep(m_arrowManager);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeAlphaChannelManager_()
	{
		MENGE_LOG_INFO( "initialize Alpha Channel Manager..." );

		m_alphaChannelManager = new AlphaChannelManager();

		AlphaChannelManager::keep(m_alphaChannelManager);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeTextManager_()
	{
		MENGE_LOG_INFO( "initialize Text Manager..." );

		m_textManager = new TextManager();

		TextManager::keep(m_textManager);
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const String & Application::getPathGameFile() const
	{
		return m_gameDescription;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setBaseDir( const String & _dir )
	{
		m_baseDir = m_applicationPath + _dir;

		if( m_fileEngine )
		{
			m_fileEngine->setBaseDir( m_baseDir );
		}

		if( m_game )
		{
			m_game->setBaseDir( m_baseDir );
		}

		if( m_scriptEngine )
		{
			ScriptEngine::TListModulePath paths;
			paths.push_back(m_baseDir);

			m_scriptEngine->addModulePath( paths );
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
		m_game = new Game( m_baseDir );

		Game::keep( m_game );

		MENGE_LOG_INFO( "Create game file '%s'"
			, m_gameDescription.c_str() );

		//m_game->setBaseDir( m_baseDir );

		//m_fileEngine->loadPak( m_gamePack );
		String fullGamePackPath = m_baseDir + m_gamePackPath;
		if( m_fileEngine->mountFileSystem( m_gamePackName, fullGamePackPath, m_gamePackType, false ) == false )
		{
			MENGE_LOG_ERROR( "Error: (Application::loadGame) failed to mount GamePak '%s'",
				m_gamePackPath.c_str() 
				);

			return false;
		}

		bool exist = false;
		if( m_loaderEngine
			->load( m_gamePackName, m_gameDescription, m_game, exist ) == false )
		{
			MENGE_LOG_ERROR( "Invalid game file '%s'"
				, m_gameDescription.c_str()
				);

			showMessageBox( "'Application' files missing or corrupt", "Critical Error", 0 );

			return false;
		}

		if( m_baseDir.empty() )	// current dir
		{
			m_baseDir = ".";
		}

		if( m_languagePackOverride.empty() == false )
		{
			m_game->setLanguagePack( m_languagePackOverride );
		}

		m_game->loadConfigPaks();
		//m_game->registerResources( m_baseDir );
		
		m_fullscreen = m_game->getFullscreen();

		if( _loadPersonality )
		{
			if( m_game->loadPersonality() == false )
			{
				return false;
			}
		}

		m_game->applyConfigPaks();

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

		const Resolution & contentResolution = m_game->getContentResolution();

		Viewport renderViewport;
		this->calcRenderViewport_( renderViewport, m_currentResolution );

		m_createRenderWindow = m_renderEngine->createRenderWindow( m_currentResolution, contentResolution, renderViewport, bits, m_fullscreen,
														_renderWindowHandle, FSAAType, FSAAQuality );

		if( m_createRenderWindow == false )
		{
			showMessageBox( "Failed to create render window", "Critical Error", 0 );
			return false;
		}

		

		if( m_fullscreen == true )
		{
			m_interface->notifyCursorClipping( renderViewport );
		}
		else
		{
			m_interface->notifyCursorUnClipping();
		}

		//m_renderEngine->setRenderViewport( renderViewport );

		m_renderEngine->enableTextureFiltering( textureFiltering );

		MENGE_LOG_INFO( "Initializing Input Engine..." );
		m_inputEngine = new InputEngine();

		InputEngine::keep( m_inputEngine );

		bool result = m_inputEngine->initialize();
		if( result == false )
		{
			MENGE_LOG_ERROR( "Input Engine initialization failed!" );
			return false;
		}

		m_inputEngine->setDimentions( contentResolution, renderViewport );
			
			
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initGame( const String & _scriptInitParams )
	{
		if( m_game->initialize( _scriptInitParams, m_platformName ) == false )
		{
			return false;
		}

		m_game->initializeRenderResources();

		//m_game->tick( 0.0f );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::loader( BinParser * _parser )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_NODE_PARSE_METHOD( Protocol::Application, this, &Application::loaderApplication_ );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::loaderApplication_( BinParser * _parser )
	{
		m_gamePackType = Consts::get()->c_dir;

		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::BaseDir_Value, &Application::setBaseDir );

			BIN_CASE_ATTRIBUTE( Protocol::GamePack_Name, m_gamePackName );
			BIN_CASE_ATTRIBUTE( Protocol::GamePack_Path, m_gamePackPath );
			//BIN_CASE_ATTRIBUTE( Protocol::GamePack_Type, m_gamePackType );
			BIN_CASE_ATTRIBUTE( Protocol::GamePack_Description, m_gameDescription );
			BIN_CASE_ATTRIBUTE( Protocol::AlreadyRunningPolicy_Value, m_alreadyRunningPolicy );
			BIN_CASE_ATTRIBUTE( Protocol::AllowFullscreenSwitchShortcut_Value, m_allowFullscreenSwitchShortcut );
			
			//Load Plugins
			BIN_CASE_NODE( Protocol::Plugin )
			{
				Menge::String pluginName;

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::Plugin_Name, pluginName );
				}
				
				TMapParam param;
				this->loadPlugin( pluginName, param );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::_loaded()
	{
		//Empty
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
	void Application::registerConsole( ConsoleInterface * _console )
	{
		m_console = _console;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::onKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown )
	{
		if( m_console != NULL )
		{
			m_console->proccessInput( _key, _char, _isDown );
		}

#	ifndef MENGE_MASTER_RELEASE
		if( _key == KC_F6 && _isDown && m_enableDebug )
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

		//if( _key == KC_F4 && _isDown && m_enableDebug )
		//{
		//	m_debugCRT = !m_debugCRT;

		//	if( m_debugCRT == true )
		//	{
		//		int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );

		//		// Turn on leak-checking bit.
		//		tmpFlag |= _CRTDBG_LEAK_CHECK_DF;

		//		// Set flag to the new value.
		//		_CrtSetDbgFlag( tmpFlag );
		//	}
		//	else
		//	{
		//		int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );

		//		// Turn off CRT block checking bit.
		//		tmpFlag &= ~_CRTDBG_CHECK_CRT_DF;

		//		// Set flag to the new value.
		//		_CrtSetDbgFlag( tmpFlag );
		//	}
		//}

		if( _key == KC_F11 && _isDown && m_enableDebug )
		{
			Player::get()
				->toggleDebugText();
		}

#	endif

		return m_game->handleKeyEvent( _point, _key, _char, _isDown );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::onMouseButtonEvent( const mt::vec2f & _point, int _button, bool _isDown )
	{
		if( m_inputMouseButtonEventBlock == true )
		{
			return false;
		}

		m_game->handleMouseButtonEventBegin( _point, _button, _isDown );
		bool result = m_game->handleMouseButtonEvent( _point, _button, _isDown );
		m_game->handleMouseButtonEventEnd( _point, _button, _isDown );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::onMouseMove( const mt::vec2f & _point, float _dx, float _dy, int _whell )
	{
		return m_game->handleMouseMove( _point, _dx, _dy, _whell );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::onAppMouseLeave()
	{
		m_game->onAppMouseLeave();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::onAppMouseEnter()
	{
		m_game->onAppMouseEnter();
		//m_game->handleMouseMove( 0, 0, 0 );
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
	void Application::setInputMouseButtonEventBlock( bool _block )
	{
		m_inputMouseButtonEventBlock = _block;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::getInputMouseButtonEventBlock() const
	{
		return m_inputMouseButtonEventBlock;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::isFocus() const
	{
		return m_focus;
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

			if( m_focus == false )
			{
				this->onAppMouseLeave();
			}
			else
			{
				this->onAppMouseEnter();
			}
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

		m_inputEngine->update();

		if( m_game->update() == false )
		{
			this->quit();
			return false;
		}

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

		//if( m_physicEngine2D )
		//{
		//	m_physicEngine2D->update( timing );
		//}
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
		//Holder<Console>::get()->render();

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
		if( m_game )
		{
			m_game->finalizeRenderResources();
			m_game->finalize();

			delete m_game;
		}

		this->unloadPlugins_();

		delete m_paramManager;
		delete m_arrowManager;

		delete m_textManager;

		delete m_taskManager;

		delete m_alphaChannelManager;
		delete m_resourceManager;
		delete m_sceneManager;
		delete m_entityManager;

		if( m_scriptEngine )
		{
			m_scriptEngine->finalize();
			delete m_scriptEngine;
		}

		//delete m_physicEngine2D;
		delete m_particleEngine;
		delete m_renderEngine;
		delete m_inputEngine;
		delete m_soundEngine;

		m_serviceProvider->unregistryService( "Codec" );
		delete m_codecEngine;

		delete m_loaderEngine;

		if( m_fileLog != NULL )
		{
			m_logEngine->unregisterLogger( m_fileLog );

			FileOutputInterface * fileLogInterface = m_fileLog->getFileInterface();
			fileLogInterface->close();

			delete m_fileLog;
			m_fileLog = NULL;
		}

		delete m_fileEngine;
		delete m_threadManager;
		delete m_serviceProvider;

		delete m_nodeManager;

		delete m_logEngine;
		delete m_consts;
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
				m_interface->notifyCursorClipping( m_gameViewport );
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
	void Application::calcRenderViewport_( Viewport & _viewport, const Resolution & _resolution )
	{
		float rw = float( _resolution.getWidth());
		float rh = float( _resolution.getHeight());

		bool resolutionFixed = Game::get()
			->isContentResolutionFixed();

		if( resolutionFixed == true )
		{
			const Resolution & contentResolution = Game::get()
				->getContentResolution();

			if( _resolution == contentResolution )
			{
				_viewport.begin.x = 0.0f;
				_viewport.begin.y = 0.0f;
				_viewport.end.x = rw;
				_viewport.end.y = rh;
			}
			else
			{
				float one_div_width = 1.f / rw;
				float one_div_height = 1.f / rh;

				float crx = float( contentResolution.getWidth() );
				float cry = float( contentResolution.getHeight() );

				float contentAspect = crx / cry;
				float aspect = rw * one_div_height;

				float dw = 1.0f;
				float dh = rw / contentAspect * one_div_height;

				if( dh > 1.0f )
				{
					dh = 1.0f;
					dw = rh * contentAspect * one_div_width;
				}

				float areaWidth = dw * rw;
				float areaHeight = dh * rh;

				_viewport.begin.x = ( rw - areaWidth ) * 0.5f;
				_viewport.begin.y = ( rh - areaHeight ) * 0.5f;
				_viewport.end.x = _viewport.begin.x + areaWidth;
				_viewport.end.y = _viewport.begin.y + areaHeight;
			}

			return;
		}
		else
		{
			_viewport.begin.x = 0;
			_viewport.begin.y = 0;

			_viewport.end.x = rw;
			_viewport.end.y = rh;
		}
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
		
		Viewport renderViewport;
		this->calcRenderViewport_( renderViewport, m_currentResolution );
		//m_renderEngine->applyRenderViewport( renderViewport );

		if( m_fullscreen == true )
		{
			m_interface->notifyCursorClipping( renderViewport );
		}
		else
		{
			//m_interface->notifyCursorUnClipping();
			this->setMouseBounded( m_mouseBounded );
		}

		const Resolution & contentResolution = 
			m_game->getContentResolution();

		m_renderEngine->changeWindowMode( m_currentResolution, contentResolution, renderViewport, _fullscreen );

		m_inputEngine->setDimentions( contentResolution, renderViewport );

		m_game->onFullscreen( m_currentResolution, m_fullscreen );

		if( !m_mouseBounded && m_renderEngine->isWindowCreated() )
		{
			//if( _fullscreen == false )
			//{
			//	m_currentResolution = m_game->getResolution();
			//}
			//else
			//{
			//	m_currentResolution = m_desktopResolution;
			//}
			
			m_game->onAppMouseEnter();	
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
	const String & Application::getProjectTitle() const
	{
		return m_game->getTitle();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setDesktopResolution( const Resolution& _resolution )
	{
		m_desktopResolution = _resolution;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::ansiToUtf8( const String& _ansi, String & _utf8 )
	{
		m_interface->ansiToUtf8( _ansi, _utf8 );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::utf8ToAnsi( const String& _utf8, String & _ansi )
	{
		m_interface->utf8ToAnsi( _utf8, _ansi );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::utf8Count( const String& _utf8, std::size_t & _size )
	{
		m_interface->utf8Count( _utf8, _size );
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
	bool Application::isDebugCRT() const
	{
		return m_debugCRT;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::loadPlugin( const String& _pluginName, const TMapParam & _params )
	{
		TDynamicLibraries::iterator it_found = m_dynamicLibraries.find( _pluginName );
		
		if( it_found == m_dynamicLibraries.end() )
		{
			DynamicLibraryInterface * lib = m_interface->loadDynamicLibrary( _pluginName );

			if( lib == NULL )
			{
				return false;
			}

			it_found = m_dynamicLibraries.insert( std::make_pair(_pluginName, lib) ).first;
		}

        DynamicLibraryInterface * lib = it_found->second;

		TDynamicLibraryFunction function =
			lib->getSymbol("dllCreatePlugin");

		if( function == NULL )
		{
			MENGE_LOG_WARNING( "Can not load %s plugin ",_pluginName.c_str() );
			return false;
		}

		TPluginCreate create = (TPluginCreate)function;

		PluginInterface * plugin;
		if( create( &plugin ) == false )
		{
			return false;
		}

		plugin->initialize( m_serviceProvider, _params );
		
		m_plugins.push_back( plugin );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::unloadPlugins_()
    {
		for( TVectorPlugins::iterator 
			it = m_plugins.begin(),
			it_end = m_plugins.end();
		it != it_end;
		++it )
		{
			(*it)->finalize();
		}

		m_plugins.clear();

		for( TDynamicLibraries::reverse_iterator 
			it = m_dynamicLibraries.rbegin(),
			it_end = m_dynamicLibraries.rend();
		it != it_end;
		++it )
		{
			TDynamicLibraryFunction function =
				it->second->getSymbol("dllFinializePlugin");
			
			if( function != NULL )
			{
				function( this );
			}					

			m_interface->unloadDynamicLibrary( it->second );			
		}

        m_dynamicLibraries.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setCursorPosition( const mt::vec2f & _point )
	{
		if( m_inputEngine )
		{
			m_inputEngine->setCursorPosition( _point );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setLanguagePack( const ConstString& _packName )
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
	void Application::setMaxClientAreaSize( std::size_t _maxWidth, std::size_t _maxHeight )
	{
		m_maxClientAreaSize.setWidth( _maxWidth );
		m_maxClientAreaSize.setHeight( _maxHeight );
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
	void Application::setCursorIcon(const String& _fileName)
	{
		m_interface->notifyCursorIconSetup(_fileName);
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::pushKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown )
	{
		if( m_inputEngine != NULL )
		{
			m_inputEngine->pushKeyEvent( _point, _key, _char, _isDown );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::pushMouseButtonEvent( const mt::vec2f & _point, int _button, bool _isDown )
	{
		if( m_inputEngine != NULL )
		{
			m_inputEngine->pushMouseButtonEvent( _point, _button, _isDown );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::pushMouseMoveEvent( const mt::vec2f & _point, int _x, int _y, int _z )
	{
		if( m_inputEngine != NULL )
		{
			m_inputEngine->pushMouseMoveEvent( _point, _x, _y, _z );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setAsScreensaver( bool _set )
	{
		m_interface->setAsScreensaver( _set );
	}	
	//////////////////////////////////////////////////////////////////////////
	void Application::showKeyboard()
	{
		m_interface->showKeyboard();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::hideKeyboard()
	{
		m_interface->hideKeyboard();
	}
	//////////////////////////////////////////////////////////////////////////
	const String & Application::getScreensaverName() const
	{
		return m_game->getScreensaverName();
	}
}

