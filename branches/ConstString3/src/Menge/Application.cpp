#	include "Application.h"

#	include "InputEngine.h"
#	include "InputHandler.h"

#	include "ServiceProvider.h"
#	include "FileEngine.h"
#	include "RenderEngine.h"
#	include "SoundEngine.h"

#	include "ParticleEngine.h"
#	include "ScriptEngine.h"
#	include "PhysicEngine.h"
#	include "PhysicEngine2D.h"

#	include "StringizeService.h"

#	include "MousePickerSystem.h"
#	include "LightSystem.h"

#	include "Game.h"

#	include "Consts.h"

#	include "LogEngine.h"

#	include "LoaderEngine.h"
//#	include "BinParser.h"

#	include "NodeManager.h"

#	include "TextManager.h"
#	include "TextField.h"

#	include "ThreadEngine.h"
#	include "ThreadTaskManager.h"

#	include "CodecEngine.h"

#	include "ConverterEngine.h"
#	include "MovieKeyFrameManager.h"

#	include "ResourceManager.h"
#	include "AlphaChannelManager.h"
#	include "ArrowManager.h"
#	include "AccountManager.h"
#	include "SceneManager.h"
#	include "EntityManager.h"

#	include "Watchdog.h"

#	include "Player.h"
#	include "Scene.h"

#	include "FactoryDefault.h"
#	include "FactoryPool.h"

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
#	include "MovieInternalObject.h"
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
#	include "Layer2DTexture.h"
#	include "PhysicalBody2D.h"
#	include "Layer2DPhysic.h"
// All Resource type
//#	include "ResourceSequence.h"
#	include "ResourceAnimation.h"
#	include "ResourceEmitterContainer.h"
#	include "ResourceFont.h"
#	include "ResourceGlyph.h"
//#	include "ResourceTilePolygon.h"
#	include "ResourceImageSolid.h"
//#	include "ResourceImageAtlas.h"
//#	include "ResourceImageCell.h"
//#	include "ResourceImageDynamic.h"
//#	include "ResourceImageSet.h"

#	include "ResourceImageDefault.h"
#	include "ResourceImageInAtlas.h"
#	include "ResourceImageAtlas.h"
#	include "ResourceImageCombineRGBAndAlpha.h"
//#	include "ResourceBinary.h"
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

#	include "ConfigFile/ConfigFile.h"

#	include "Interface/UnicodeInterface.h"

//extern "C"
//{
//	#	include <iniparser/src/iniparser.h>
//}

#	include <locale.h>
#	include <ctime>
#	include <sstream>
#	include <iomanip>

#	include "VersionInfo.h"

#	include "Config/Config.h"

extern bool initPluginMengeImageCodec( Menge::PluginInterface ** _plugin );
extern bool initPluginMengeVideoCodec( Menge::PluginInterface ** _plugin );

//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem( Menge::ApplicationInterface** _interface )
{
	if( _interface == 0 )
	{
		return false;
	}

	Menge::Application * app = new Menge::Application();

	Menge::Application::keep( app );

	*_interface = app;

	return true;
}
//////////////////////////////////////////////////////////////////////////
void releaseInterfaceSystem( Menge::ApplicationInterface * _interface )
{
	if( _interface )
	{
		delete static_cast<Menge::Application*>( _interface );
	}
}

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Application::Application()
		: m_platform(NULL)
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
		, m_physicEngine2D(0)
		, m_loaderEngine(0)
		, m_mouseBounded(false)
		, m_game(0)
		, m_focus(true)
		, m_update(true)				
		, m_console(NULL)
		, m_scriptEngine(NULL)
		, m_threadEngine(NULL)
		, m_taskManager(NULL)
		, m_alreadyRunningPolicy(0)
		, m_allowFullscreenSwitchShortcut(true)
		, m_resourceManager(NULL)
		, m_alphaChannelManager(NULL)
		, m_codecEngine(NULL)
		, m_textManager(NULL)
		, m_unicodeInterface(NULL)
		, m_createRenderWindow(false)
		, m_cursorMode(false)
		, m_invalidateVsync(false)
		, m_invalidateCursorMode(false)
		, m_fullscreen(false)
		, m_nodeManager(0)
		, m_debugCRT(false)
		, m_inputMouseButtonEventBlock(false)
		, m_countThreads(2)
		, m_mouseEnter(false)
		, m_developmentMode(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Application::~Application()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const String & Application::getPlatformName() const
	{
		return m_platformName;
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
	bool Application::initialize( PlatformInterface* _platform, const String & _platformName, const String& _args )
	{
		m_platform = _platform;
		m_platformName = _platformName;

		this->parseArguments_( _args );

		m_consts = new Consts;
		Consts::keep(m_consts);

		m_serviceProvider = new ServiceProvider;

		ServiceProvider::keep( m_serviceProvider );

		m_serviceProvider->registryService( "ApplicationService", this );

		m_platform->notifyServiceProviderReady( m_serviceProvider );

		m_stringizeService = new StringizeService;

		m_serviceProvider->registryService("StringizeService", m_stringizeService);

		m_watchdog = new Watchdog;
		
		Watchdog::keep( m_watchdog );
		
		ExecuteInitialize exinit( this );
		
		exinit.add( &Application::initializeLogEngine_);
		exinit.add( &Application::initializeUnicodeEngine_);
		exinit.add( &Application::initializeFileEngine_);
		exinit.add( &Application::initializeThreadEngine_);
		exinit.add( &Application::initializeParticleEngine_);
		exinit.add( &Application::initializePhysicEngine2D_);
		exinit.add( &Application::initializeRenderEngine_);
		exinit.add( &Application::initializeSoundEngine_);
		exinit.add( &Application::initializeTaskManager_);
		exinit.add( &Application::initializeNodeManager_);
		exinit.add( &Application::initializeLoaderEngine_);
		exinit.add( &Application::initializeScriptEngine_);
		exinit.add( &Application::initializeCodecEngine_);
		exinit.add( &Application::initializeConverterEngine_);
		exinit.add( &Application::initializeMovieKeyFrameManager_);
		exinit.add( &Application::initializeResourceManager_);
		exinit.add( &Application::initializeArrowManager_);
		exinit.add( &Application::initializeSceneManager_);
		exinit.add( &Application::initializeEntityManager_);		
		exinit.add( &Application::initializeAlphaChannelManager_);
		exinit.add( &Application::initializeTextManager_);

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

		//extern initPlugin initPluginMengeVideoCodec;
		//{
		//	MENGE_LOG_INFO( "load Video Codec..." );

		//	PluginInterface * plugin;
		//	initPluginMengeVideoCodec( &plugin );

		//	TMapParam param;
		//	plugin->initialize( m_serviceProvider, param );

		//	m_plugins.push_back( plugin );
		//}

//#	ifndef MENGE_MASTER_RELEASE
//		{
//			MENGE_LOG_INFO( "load Xml Codec..." );
//
//			TMapParam param;
//			this->loadPlugin(L"MengeXmlCodecPlugin.dll", param);
//		}
//#	endif


		//if( m_console != NULL )
		//{
		//	m_console->inititalize( m_logSystem );
		//}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::loadConfig( const WString& _configFile, const WString& _iniFile )
	{
		MENGE_LOG_INFO( "load config %S ..."
			, _configFile.c_str()
			);

		//bool exist = false;
		//if( m_loaderEngine
		//	->load( Consts::get()->c_builtin_empty, _configFile, this, exist ) == false )
		//{
		//	MENGE_LOG_ERROR( "Application::loadConfig parse '%S' failed '%d'"
		//		, _configFile.c_str() 
  //              , exist
		//		);

		//	//showMessageBox( "'Application' file missing or corrupt", "Critical Error", 0 );

		//	return false;
		//}

		if( FileEngine::get()
			->existFile( Consts::get()->c_builtin_empty, _iniFile ) == false )
		{
			MENGE_LOG_ERROR("Application::loadConfig: invalid open iniFile %S"
				, _iniFile.c_str()
				);

			return false;
		}

		FileInputStreamInterface * file = FileEngine::get()
			->openInputFile( Consts::get()->c_builtin_empty, _iniFile );

		if( file == NULL )
		{
			MENGE_LOG_ERROR("Application::loadConfig: not found ini file %S"
				, _iniFile.c_str()
				);

			return false;
		}

		ConfigFile cfg;

		if( cfg.load( file ) == false )
		{
			return false;
		}

		WString locale_default_setting;
		if( cfg.getSetting( L"LOCALE", L"Default", locale_default_setting ) == true )
		{
			bool u_locale_default_setting_successful;
			String locale_default = m_unicodeInterface->unicodeToUtf8( locale_default_setting, u_locale_default_setting_successful );

			ConstString locale_default_const(locale_default);
			this->setLanguagePack( locale_default_const );
		}

		WString gamepack_name_setting;
		if( cfg.getSetting( L"GamePack", L"Name", gamepack_name_setting ) == true )
		{
			bool u_gamepack_name_setting_successful;
			String gamepack_name = m_unicodeInterface->unicodeToUtf8( gamepack_name_setting, u_gamepack_name_setting_successful );

			m_gamePackName = ConstString(gamepack_name);
		}

		WString gamepack_path_setting;
		if( cfg.getSetting( L"GamePack", L"Path", gamepack_path_setting ) == true )
		{				
			m_gamePackPath = gamepack_path_setting;
		}

		WString gamepack_description_setting;
		if( cfg.getSetting( L"GamePack", L"Description", gamepack_description_setting ) == true )
		{				
			m_gameDescription = gamepack_description_setting;
		}

		m_gamePackType = Consts::get()->c_dir;
		
		WString gamepack_type_setting;
		if( cfg.getSetting( L"GamePack", L"Type", gamepack_type_setting ) == true )
		{			
			bool u_gamepack_type_setting_successful;
			String gamepack_type = m_unicodeInterface->unicodeToUtf8( gamepack_type_setting, u_gamepack_type_setting_successful );

			m_gamePackType = ConstString(gamepack_type);
		}

		TVectorWString plugins;
		cfg.getSettings( L"Plugins", L"Name", plugins );

		for( TVectorWString::iterator
			it = plugins.begin(),
			it_end = plugins.end();
		it != it_end;
		++it )
		{
			const WString & pluginName = *it;

			TMapParam param;
			this->loadPlugin( pluginName, param );
		}
				
		//if( m_baseDir.empty() )	// current dir
		//{
		//	m_baseDir = MENGE_DEFAULT_BASE_DIR;
		//}

		//this->setBaseDir( m_baseDir );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	PlatformInterface * Application::getPlatform() const
	{
		return m_platform;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * Application::getServiceProvider() const
	{
		return m_serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeThreadEngine_()
	{
		MENGE_LOG_INFO( "Initializing Thread System..." );
		m_threadEngine = new ThreadEngine();

		ThreadEngine::keep(m_threadEngine);

		if( m_threadEngine->initialize() == false )
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

		m_serviceProvider->registryService( "FileService", m_fileEngine );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeLogEngine_()
	{
		m_logEngine = new LogEngine();

		LogEngine::keep( m_logEngine );

		m_serviceProvider->registryService( "LogService", m_logEngine );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeUnicodeEngine_()
	{
		initInterfaceSystem( &m_unicodeInterface );

		m_serviceProvider->registryService("Unicode", m_unicodeInterface);

		m_unicodeInterface->initialize( m_logEngine );

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
		MENGE_LOG_INFO( "Inititalizing Physics2D System..." );
		m_physicEngine2D = new PhysicEngine2D();

		PhysicEngine2D::keep( m_physicEngine2D );

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
		MENGE_LOG_INFO( "Initializing Render System..." );
		m_renderEngine = new RenderEngine();

		RenderEngine::keep( m_renderEngine );

		if( m_renderEngine->initialize( 32000 ) == false )
		{
			MENGE_LOG_ERROR("Fatal error: (Application::initialize) Failed to initialize RenderEngine");
			return false;
		}

		m_serviceProvider->registryService( "RenderService", m_renderEngine );
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


		m_serviceProvider->registryService( "SoundService", m_soundEngine );

		//m_platform->notifySoundInitialize();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeTaskManager_()
	{
		ThreadEngine * threadEngine = ThreadEngine::get();
		if( threadEngine == NULL )
		{
			MENGE_LOG_ERROR("Error: (Application::initialize) Failed to initialise TaskManager, because ThreadEngine not initialised ");
			return false;
		}

		ThreadSystemInterface * threadSystem = threadEngine->getInterface();
			
		m_taskManager = new ThreadTaskManager( threadSystem, m_countThreads );
		m_taskManager->initialize();
		
		ThreadTaskManager::keep(m_taskManager);

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
		//NODE_FACTORY( PhysicalBody2D );
		NODE_FACTORY( SoundEmitter );
		NODE_FACTORY( Sprite );
		NODE_FACTORY( TextField );
		//NODE_FACTORY( TileMap );
		//NODE_FACTORY( Track );
		NODE_FACTORY( Movie );
		NODE_FACTORY( MovieInternalObject );
		//NODE_FACTORY( Model );
		NODE_FACTORY( Video );
		NODE_FACTORY( Layer2D );
		NODE_FACTORY( Layer2DPhysic );
		//NODE_FACTORY( Layer2DLoop );
		//NODE_FACTORY( Layer2DAccumulator );
		//NODE_FACTORY( Layer2DTexture );
		//NODE_FACTORY( LayerScene );
		//NODE_FACTORY( RenderMesh );
		NODE_FACTORY( Camera2D );
		NODE_FACTORY( Camera3D );
		//NODE_FACTORY( SceneNode3D );
		NODE_FACTORY( Window );
		NODE_FACTORY( HotSpotImage );
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

		m_serviceProvider->registryService( "ScriptService", m_scriptEngine );
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeCodecEngine_()
	{
		MENGE_LOG_INFO( "Inititalizing Codecs..." );

		m_codecEngine = new CodecEngine();

		CodecEngine::keep( m_codecEngine );

		m_serviceProvider->registryService( "CodecService", m_codecEngine );

		m_codecEngine->registerCodecExt(L"png", ConstString("pngImage"));
		m_codecEngine->registerCodecExt(L"jpg", ConstString("jpegImage"));
		m_codecEngine->registerCodecExt(L"jpeg", ConstString("jpegImage"));
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeConverterEngine_()
	{
		MENGE_LOG_INFO( "Inititalizing Converter..." );

		m_converterEngine = new ConverterEngine();

		ConverterEngine::keep( m_converterEngine );

		m_serviceProvider->registryService( "ConverterService", m_converterEngine );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeMovieKeyFrameManager_()
	{
		MENGE_LOG_INFO( "Inititalizing MovieKeyFrameManager..." );
		
		m_movieKeyFrameManager = new MovieKeyFrameManager();
		MovieKeyFrameManager::keep( m_movieKeyFrameManager );
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
		RESOURCE_FACTORY( ResourceAnimation );
		RESOURCE_FACTORY( ResourceEmitterContainer );
		RESOURCE_FACTORY( ResourceFont );
		//RESOURCE_FACTORY( ResourceBinary );
		//RESOURCE_FACTORY( ResourceTilePolygon );
		
		//RESOURCE_FACTORY( ResourceSequence );
		//RESOURCE_FACTORY( ResourceImageAtlas );
		//RESOURCE_FACTORY( ResourceImageCell );
		//RESOURCE_FACTORY( ResourceImageDynamic );
		//RESOURCE_FACTORY( ResourceImageSet );
		//RESOURCE_FACTORY( ResourceMesh );

		RESOURCE_FACTORY( ResourceImageDefault );
		RESOURCE_FACTORY( ResourceImageCombineRGBAndAlpha );
		RESOURCE_FACTORY( ResourceImageInAtlas );
		RESOURCE_FACTORY( ResourceImageSolid );
		RESOURCE_FACTORY( ResourceImageAtlas );
		RESOURCE_FACTORY( ResourceMovie );
		RESOURCE_FACTORY( ResourceVideo );
		RESOURCE_FACTORY( ResourcePlaylist );
		RESOURCE_FACTORY( ResourceSound );
		RESOURCE_FACTORY( ResourceGlyph );
		//RESOURCE_FACTORY( ResourceModel );
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
	const WString & Application::getPathGameFile() const
	{
		return m_gameDescription;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setBaseDir( const WString & _dir )
	{
		m_baseDir = _dir;

		MENGE_LOG_WARNING( "setBaseDir '%S' fullpath '%S'"
			, _dir.c_str()
			, m_baseDir.c_str()
			);

		if( m_scriptEngine )
		{
			TVectorWString paths;
			paths.push_back( m_baseDir );

			m_scriptEngine->addModulePath( paths );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const WString& Application::getBaseDir() const
	{
		return m_baseDir;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::loadGame()
	{
		m_game = new Game(m_baseDir, m_developmentMode, m_platformName);

		Game::keep( m_game );

		MENGE_LOG_INFO( "Application::loadGame mount game pak '%s'"
			, m_gamePackName.c_str() 
			);

		//m_game->setBaseDir( m_baseDir );

		//m_fileEngine->loadPak( m_gamePack );
		WString fullGamePackPath = m_baseDir;

		if( m_gamePackPath.empty() == false )
		{
			fullGamePackPath += m_gamePackPath;
			fullGamePackPath += MENGE_FOLDER_DELIM;
		}

		if( m_fileEngine->mountFileSystem( m_gamePackName, fullGamePackPath, m_gamePackType, false ) == false )
		{
			MENGE_LOG_ERROR( "Application:loadGame: failed to mount GamePak '%S' [%S]"
				, m_gamePackPath.c_str() 
				, fullGamePackPath.c_str()
				);

			return false;
		}
        
        MENGE_LOG_INFO( "Application:loadGame load game pak '%s' desc '%S'"
                       , m_gamePackName.c_str() 
                       , m_gameDescription.c_str()
                       );

		if( m_game->loadDescription( m_gamePackName, m_gameDescription ) == false )
		{
			MENGE_LOG_ERROR( "Application:loadGame invalid load game file '%s' '%S'"
				, m_gamePackName.c_str()
				, m_gameDescription.c_str()
				);

			//showMessageBox( "'Application' files missing or corrupt", "Critical Error", 0 );

			return false;
		}

		MENGE_LOG_INFO( "Application:loadGame load game pak successful"
			);

		if( m_languagePackOverride.empty() == false )
		{
			m_game->setLanguagePack( m_languagePackOverride );
		}

		m_game->loadConfigPaks();
		//m_game->registerResources( m_baseDir );
		
		m_fullscreen = m_game->getFullscreen();

		m_game->applyConfigPaks();

		m_game->setCursorMode( m_cursorMode );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::loadPersonality()
	{
		MENGE_LOG_INFO( "Application:loadGame load personality"
			);
            
		if( m_game->loadPersonality() == false )
		{
			return false;
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::createRenderWindow( WindowHandle _renderWindowHandle, WindowHandle _inputWindowHandle )
	{
		m_currentResolution = ( m_fullscreen == true )
			? this->getDesktopResolution() 
			: m_game->getResolution();

		int bits = m_game->getBits();
		int FSAAType = m_game->getFSAAType();
		int FSAAQuality = m_game->getFSAAQuality();
		bool textureFiltering = m_game->getTextureFiltering();

		const Resolution & contentResolution = m_game->getContentResolution();

		MENGE_LOG_INFO( "Application::createRenderWindow %d Current Resolution %d %d"
			, m_fullscreen
			, m_currentResolution.getWidth()
			, m_currentResolution.getHeight()
			);

		Viewport renderViewport;
		this->calcRenderViewport_( renderViewport, m_currentResolution );

		MENGE_LOG_INFO( "Application::createRenderWindow Render Viewport %f %f - %f %f"
			, renderViewport.begin.x
			, renderViewport.begin.y
			, renderViewport.getWidth()
			, renderViewport.getHeight()
			);

		m_createRenderWindow = m_renderEngine->createRenderWindow( m_currentResolution, contentResolution, renderViewport, bits, m_fullscreen,
														_renderWindowHandle, FSAAType, FSAAQuality );

		if( m_createRenderWindow == false )
		{
            MENGE_LOG_ERROR( "Failed to create render window" 
                            );
            
			//showMessageBox( "Failed to create render window", "Critical Error", 0 );
			return false;
		}
		
		if( m_fullscreen == true )
		{
			m_platform->notifyCursorClipping( renderViewport );
		}
		else
		{
			m_platform->notifyCursorUnClipping();
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
		if( m_game->initialize( _scriptInitParams ) == false )
		{
			MENGE_LOG_ERROR("Application::initGame invalid initialize %s"
				, _scriptInitParams.c_str()
				);

			return false;
		}

		m_game->initializeRenderResources();

		//m_game->tick( 0.0f );

		return true;
	}
	////////////////////////////////////////////////////////////////////////////
	//void Application::loader( BinParser * _parser )
	//{
	//	BIN_SWITCH_ID( _parser )
	//	{
	//		BIN_CASE_NODE_PARSE_METHOD( Protocol::Application, this, &Application::loaderApplication_ );
	//	}
	//}
	////////////////////////////////////////////////////////////////////////////
	//void Application::loaderApplication_( BinParser * _parser )
	//{
	//	m_gamePackType = Consts::get()->c_dir;

	//	BIN_SWITCH_ID( _parser )
	//	{
	//		BIN_CASE_ATTRIBUTE( Protocol::BaseDir_Value, m_baseDir );

	//		BIN_CASE_ATTRIBUTE( Protocol::GamePack_Name, m_gamePackName );
	//		BIN_CASE_ATTRIBUTE( Protocol::GamePack_Path, m_gamePackPath );
	//		//BIN_CASE_ATTRIBUTE( Protocol::GamePack_Type, m_gamePackType );
	//		BIN_CASE_ATTRIBUTE( Protocol::GamePack_Description, m_gameDescription );
	//		BIN_CASE_ATTRIBUTE( Protocol::AlreadyRunningPolicy_Value, m_alreadyRunningPolicy );
	//		BIN_CASE_ATTRIBUTE( Protocol::AllowFullscreenSwitchShortcut_Value, m_allowFullscreenSwitchShortcut );
	//		
	//		//Load Plugins
	//		BIN_CASE_NODE( Protocol::Plugin )
	//		{
	//			Menge::String pluginName;

	//			BIN_FOR_EACH_ATTRIBUTES()
	//			{
	//				BIN_CASE_ATTRIBUTE( Protocol::Plugin_Name, pluginName );
	//			}
	//			
	//			TMapParam param;
	//			this->loadPlugin( pluginName, param );
	//		}
	//	}
	//}
	////////////////////////////////////////////////////////////////////////////
	//void Application::_loaded()
	//{
	//	//Empty
	//}
	//////////////////////////////////////////////////////////////////////////
	void Application::parseArguments_( const String& _arguments )
	{
		String::size_type idx_sound = _arguments.find( "-sound" );
		if( idx_sound != String::npos )
		{
			m_sound = false;
		}

		String::size_type idx_particles = _arguments.find( "-particles" );
		if( idx_particles != String::npos )
		{
			m_particles = false;
		}

		String::size_type idx_dev = _arguments.find( "-dev" );
		if( idx_dev != String::npos )
		{
			m_developmentMode = true;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::onKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown )
	{
		if( m_console != NULL )
		{
			m_console->proccessInput( _key, _char, _isDown );
		}

//#	ifndef MENGE_MASTER_RELEASE
		if( m_developmentMode == true )
		{
			if( _key == KC_F6 && _isDown )
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

			if( _key == KC_F10 && _isDown )
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

			if( _key == KC_F9 && _isDown )
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

			if( _key == KC_F8 && _isDown )
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

			if( _key == KC_F5 && _isDown )
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

			if( _key == KC_F11 && _isDown )
			{
				Player::get()
					->toggleDebugText();
			}
		}

//#	endif

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
		if( m_inputEngine->validCursorPosition( _point ) == false )
		{
			m_mouseEnter = false;

			return false;
		}

		if( m_mouseEnter == false )
		{
			this->onAppMouseEnter( _point );
		}

		return m_game->handleMouseMove( _point, _dx, _dy, _whell );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::onAppMouseLeave()
	{
		m_mouseEnter = false;

		m_game->onAppMouseLeave();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::onAppMouseEnter( const mt::vec2f & _point )
	{
		if( InputEngine::get()
			->validCursorPosition( _point ) == false )
		{
			return;
		}

		m_mouseEnter = true;

		m_game->onAppMouseEnter( _point );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::quit()	
	{
		m_platform->stop();
		
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
	void Application::onFocus( bool _focus, const mt::vec2f & _point )
	{
		//return;
        MENGE_LOG_INFO("Application::onFocus from %d to %d"
                          , m_focus
                          , _focus
                          );
        
		if( m_focus == _focus ) 
		{
			return;
		}

		m_focus = _focus;

		if( m_game != NULL )
		{
			m_game->onFocus( m_focus );

			if( m_focus == false )
			{
				this->onAppMouseLeave();
			}
			else
			{
				this->onAppMouseEnter( _point );
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
		m_platform->minimizeWindow();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::onUpdate()
	{	
		if( m_taskManager )
		{
			m_taskManager->update();
		}

		if( !m_update && !m_focus ) 
		{						
			return false;
		}

		m_inputEngine->update();

		if( m_game->update() == false )
		{
			this->quit();
			return false;
		}

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
    void Application::onTurnSound( bool _turn )
    {
		if( _turn == false )
		{
			if( m_soundEngine )
			{
				m_soundEngine->onTurnStream( _turn );
			}

			if( m_game )
			{
				m_game->onTurnSound( _turn );
			}

			if( m_soundEngine )
			{
				m_soundEngine->onTurnSound( _turn );
			}
		}
		else
		{
			if( m_soundEngine )
			{
				m_soundEngine->onTurnSound( _turn );
			}

			if( m_game )
			{
				m_game->onTurnSound( _turn );
			}

			//if( m_soundEngine )
			//{
			//	m_soundEngine->onTurnStream( _turn );
			//}
		}

    }
	//////////////////////////////////////////////////////////////////////////
	void Application::finalize()
	{		
		if( m_game )
		{
			m_game->finalizeRenderResources();
			m_game->finalize();
		}

		if( m_scriptEngine )
		{
			m_scriptEngine->finalize();
		}

		if( m_taskManager )
		{
			m_taskManager->finalize();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::destroy()
	{
		this->unloadPlugins_();

		delete m_game;

		//delete m_paramManager;
		delete m_arrowManager;

		delete m_textManager;
		
		delete m_alphaChannelManager;
		delete m_resourceManager;
		delete m_sceneManager;
		delete m_entityManager;

		delete m_scriptEngine;

		delete m_physicEngine2D;
		delete m_particleEngine;
		
		m_serviceProvider->unregistryService( "RenderService" );
		
		if( m_renderEngine != NULL )
		{
			m_renderEngine->finalize();
			delete m_renderEngine;
		}
		
		delete m_inputEngine;
		delete m_soundEngine;

		delete m_taskManager;

		delete m_threadEngine;

		m_serviceProvider->unregistryService( "ConverterService" );
		delete m_converterEngine;
		delete m_movieKeyFrameManager;
		
		m_serviceProvider->unregistryService( "CodecService" );
		delete m_codecEngine;

		delete m_loaderEngine;

		m_serviceProvider->unregistryService( "FileService" );
		delete m_fileEngine;
		
		delete m_nodeManager;

		m_serviceProvider->unregistryService( "LogService" );
		delete m_logEngine;

		m_serviceProvider->unregistryService( "StringizeService" );
		delete m_stringizeService;

		delete m_watchdog;

		delete m_serviceProvider;
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
				m_platform->notifyCursorClipping( m_gameViewport );
			}
			else
			{
				m_platform->notifyCursorUnClipping();
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

		m_platform->notifyWindowModeChanged( m_currentResolution, m_fullscreen );
		
		MENGE_LOG_INFO( "Application::setFullscreenMode %d Current Resolution %d %d"
			, _fullscreen
			, m_currentResolution.getWidth()
			, m_currentResolution.getHeight()
			);

		Viewport renderViewport;
		this->calcRenderViewport_( renderViewport, m_currentResolution );
		//m_renderEngine->applyRenderViewport( renderViewport );

		MENGE_LOG_INFO( "Application::createRenderWindow Render Viewport %f %f - %f %f"
			, renderViewport.begin.x
			, renderViewport.begin.y
			, renderViewport.getWidth()
			, renderViewport.getHeight()
			);

		if( m_fullscreen == true )
		{
			m_platform->notifyCursorClipping( renderViewport );
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
			
			//m_game->onAppMouseEnter();	
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::screenshot( RenderTextureInterface* _renderTargetImage, const mt::vec4f & _rect )
	{
		const Resolution & contentResolution = m_game->getContentResolution();

		mt::vec4f res = _rect;
		res.x *= static_cast<float>( m_currentResolution.getWidth() ) / contentResolution.getWidth();
		res.y *= static_cast<float>( m_currentResolution.getHeight() ) / contentResolution.getHeight();
		res.z *= static_cast<float>( m_currentResolution.getWidth() ) / contentResolution.getWidth();
		res.w *= static_cast<float>( m_currentResolution.getHeight() ) / contentResolution.getHeight();
		
		m_renderEngine->screenshot( _renderTargetImage, _rect );
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
	void Application::showMessageBox( const WString& _message, const WString& _header, unsigned int _style )
	{
		m_platform->showMessageBox( _message, _header, _style );
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
	const WString & Application::getProjectTitle() const
	{
		return m_game->getTitle();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setDesktopResolution( const Resolution& _resolution )
	{
		m_desktopResolution = _resolution;
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
	bool Application::isDebugCRT() const
	{
		return m_debugCRT;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::loadPlugin( const WString& _pluginName, const TMapParam & _params )
	{
		TDynamicLibraries::iterator it_found = m_dynamicLibraries.find( _pluginName );
		
		if( it_found == m_dynamicLibraries.end() )
		{
			MENGE_LOG_INFO( "Application::loadPlugin %S"
				, _pluginName.c_str() 
				);

			DynamicLibraryInterface * lib = m_platform->loadDynamicLibrary( _pluginName );

			if( lib == NULL )
			{
				MENGE_LOG_ERROR( "Can not load %S plugin [invalid load]"
					, _pluginName.c_str() 
					);

				return false;
			}

			it_found = m_dynamicLibraries.insert( std::make_pair(_pluginName, lib) ).first;
		}

        DynamicLibraryInterface * lib = it_found->second;

		TDynamicLibraryFunction function =
			lib->getSymbol("dllCreatePlugin");

		if( function == NULL )
		{
			MENGE_LOG_ERROR( "Can not load %S plugin [dllCreatePlugin]"
				, _pluginName.c_str() 
				);

			return false;
		}

		TPluginCreate create = (TPluginCreate)function;

		PluginInterface * plugin;
		if( create( &plugin ) == false )
		{
			MENGE_LOG_ERROR( "Can not load %S plugin [invalid create]"
				, _pluginName.c_str() 
				);

			return false;
		}

		plugin->initialize( m_serviceProvider, _params );
		
		m_plugins.push_back( plugin );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::unloadPlugins_()
    {
		//delete plugins  in the order opposite to the order of xml declaring
		for( TVectorPlugins::reverse_iterator 
			it = m_plugins.rbegin(),
			it_end = m_plugins.rend();
		it != it_end;
		++it )
		{
			(*it)->finalize();
		}
		/*
		for( TVectorPlugins::iterator 
			it = m_plugins.begin(),
			it_end = m_plugins.end();
		it != it_end;
		++it )
		{
			(*it)->finalize();
		}
		*/
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

			m_platform->unloadDynamicLibrary( it->second );			
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
	void Application::setMaxClientAreaSize( size_t _maxWidth, size_t _maxHeight )
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
			m_platform->notifyVsyncChanged( m_vsync );
			m_invalidateVsync = false;
		}

		if( m_invalidateCursorMode == true )
		{
			m_platform->notifyCursorModeChanged( m_cursorMode );
			m_invalidateCursorMode = false;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setCursorMode( bool _mode )
	{
		m_cursorMode = _mode;

		if( m_game != NULL )
		{
			m_game->setCursorMode( m_cursorMode );
		}

		if( m_cursorMode == true && m_cursorFileName.empty() == false )
		{
			m_platform->notifyCursorIconSetup( m_cursorFileName );
		}

		m_invalidateCursorMode = true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::getCursorMode() const
	{
		return m_cursorMode;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setCursorIcon(const WString& _fileName)
	{
		m_cursorFileName = _fileName;

		if( m_cursorMode == false )
		{
			return;
		}

		m_platform->notifyCursorIconSetup(_fileName);
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
		m_platform->setAsScreensaver( _set );
	}	
	//////////////////////////////////////////////////////////////////////////
	void Application::showKeyboard()
	{
		m_platform->showKeyboard();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::hideKeyboard()
	{
		m_platform->hideKeyboard();
	}
	//////////////////////////////////////////////////////////////////////////
	const WString & Application::getScreensaverName() const
	{
		return m_game->getScreensaverName();
	}
}

