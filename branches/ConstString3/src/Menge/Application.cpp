#	include "Application.h"

#	include "InputEngine.h"
#	include "InputHandler.h"

#	include "ServiceProvider.h"

#	include "FileEngine.h"
#	include "RenderEngine.h"

#	include "Interface/SoundSystemInterface.h"

#	include "ParticleEngine.h"
#	include "ScriptEngine.h"
#	include "PhysicEngine.h"
#	include "PhysicEngine2D.h"

#	include "StringizeService.h"
#	include "NotificationService.h"

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

#	include "ResourceImageSolid.h"
#	include "ResourceImageDefault.h"
#	include "ResourceImageCombineRGBAndAlpha.h"
#	include "ResourceImageMultiplyRGBAndAlpha.h"

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
#	include "ResourceCursorSystem.h"
#	include "ResourceInternalObject.h"
#   include "ResourceExternal.h"

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

#	include "ConfigLoader.h"

extern bool initPluginMengeImageCodec( Menge::PluginInterface ** _plugin );
extern bool initPluginMengeVideoCodec( Menge::PluginInterface ** _plugin );

extern bool initializeSoundService( Menge::SoundServiceInterface ** _service );
extern void finalizeSoundService( Menge::SoundServiceInterface * _service );

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
		, m_soundService(0)
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
		, m_allowFullscreenSwitchShortcut(true)
		, m_resourceManager(NULL)
		, m_alphaChannelManager(NULL)
		, m_codecEngine(NULL)
		, m_textManager(NULL)
		, m_unicodeService(NULL)
        , m_notificationService(NULL)
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
		, m_cursorResource(NULL)
		, m_fixedContentResolution(false)
		, m_vsync(false)
		, m_fullScreen(true)
		, m_bits(0)
		, m_FSAAType(0)
		, m_FSAAQuality(0)
		, m_textureFiltering(true)
        , m_windowModeCheck(false)
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
	bool Application::initialize( PlatformInterface* _platform, const String& _args, const ApplicationSettings & _setting )
	{
		m_platform = _platform;
		
		m_platformName = _setting.platformName;
		m_projectCodename = _setting.projectCodename;

		m_contentResolution = _setting.contentResolution;
		m_lowContentViewport = _setting.lowContentViewport;
		m_fixedContentResolution = _setting.fixedContentResolution;
		
		m_windowResolution = _setting.windowResolution;
		m_bits = _setting.bits;
		m_fullscreen = _setting.fullscreen;
		m_vsync = _setting.vsync;

        m_windowModeCheck = _setting.windowModeCheck;
		
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
		
		exinit.add( &Application::initializeNotificationService_ );
		exinit.add( &Application::initializeLogEngine_ );
		exinit.add( &Application::initializeUnicodeEngine_ );
		exinit.add( &Application::initializeFileEngine_ );
		exinit.add( &Application::initializeThreadEngine_ );
		exinit.add( &Application::initializeParticleEngine_ );
		exinit.add( &Application::initializePhysicEngine2D_ );
		exinit.add( &Application::initializeRenderEngine_ );
		exinit.add( &Application::initializeSoundEngine_ );
		exinit.add( &Application::initializeTaskManager_ );
		exinit.add( &Application::initializeNodeManager_ );
		exinit.add( &Application::initializeLoaderEngine_ );
		exinit.add( &Application::initializeScriptEngine_ );
		exinit.add( &Application::initializeCodecEngine_ );
		exinit.add( &Application::initializeConverterEngine_ );
		exinit.add( &Application::initializeMovieKeyFrameManager_ );
		exinit.add( &Application::initializeResourceManager_ );
		exinit.add( &Application::initializeArrowManager_ );
		exinit.add( &Application::initializeSceneManager_ );
		exinit.add( &Application::initializeEntityManager_ );		
		exinit.add( &Application::initializeAlphaChannelManager_ );
		exinit.add( &Application::initializeTextManager_ );
		exinit.add( &Application::initializeInputEngine_ );

		if( exinit.run() == false )
		{
			return false;
		}

		this->setBaseDir( _setting.baseDir );
		
		//extern initPlugin initPluginMengeImageCodec;
		{
			MENGE_LOG_INFO( "load Image Codec..." );

			PluginInterface * plugin;
			initPluginMengeImageCodec( &plugin );

			plugin->initialize( m_serviceProvider );

			m_plugins.push_back( plugin );
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::loadPlugins( const TVectorWString & _plugins )
	{
		for( TVectorWString::const_iterator
			it = _plugins.begin(),
			it_end = _plugins.end();
		it != it_end;
		++it )
		{
			const WString & pluginName = *it;

			if( this->loadPlugin( pluginName ) == false )
			{
				return false;
			}
		}

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
	bool Application::initializeNotificationService_()
	{
		m_notificationService = new NotificationService;

		NotificationService::keep(m_notificationService);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeThreadEngine_()
	{
		MENGE_LOG_INFO( "Initializing Thread System..." );
		m_threadEngine = new ThreadEngine;

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
		m_fileEngine = new FileEngine;
		
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
		m_logEngine = new LogEngine;

		LogEngine::keep( m_logEngine );

		m_serviceProvider->registryService( "LogService", m_logEngine );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeUnicodeEngine_()
	{
		initInterfaceSystem( &m_unicodeService );

		m_serviceProvider->registryService("UnicodeService", m_unicodeService);

		m_unicodeService->initialize( m_logEngine );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeParticleEngine_()
	{
#	if	MENGE_PARTICLES	== (1)
		MENGE_LOG_INFO( "Initializing Particle System..." );
		m_particleEngine = new ParticleEngine;

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
		m_physicEngine2D = new PhysicEngine2D;

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
		m_renderEngine = new RenderEngine;

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

		if( initializeSoundService( &m_soundService ) == false )
		{
			MENGE_LOG_ERROR("Application::initializeSoundEngine_ Failed to create SoundEngine");

			return false;
		}

		Holder<SoundServiceInterface>::keep( m_soundService );

		if( m_soundService->initialize() == false )
		{
			MENGE_LOG_ERROR("Error: (Application::initialize) Failed to initialize SoundEngine");
			m_sound = false;
		}

		if( m_sound == false )
		{
			m_soundService->mute( true );
		}


		m_serviceProvider->registryService( "SoundService", m_soundService );

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
			
		m_taskManager = new ThreadTaskManager(threadSystem, m_countThreads);
		m_taskManager->initialize();
		
		ThreadTaskManager::keep(m_taskManager);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeNodeManager_()
	{		
		m_nodeManager = new NodeManager;

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
	bool Application::initializeInputEngine_()
	{
		MENGE_LOG_INFO( "Initializing Input Engine..." );
		m_inputEngine = new InputEngine();

		InputEngine::keep( m_inputEngine );

		bool result = m_inputEngine->initialize( m_serviceProvider );
		if( result == false )
		{
			MENGE_LOG_ERROR( "Input Engine initialization failed!" );
			return false;
		}
		
		m_serviceProvider->registryService("InputService", m_inputEngine );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeSceneManager_()
	{
		MENGE_LOG_INFO( "initialize Scene Manager..." );

		m_sceneManager = new SceneManager();

        if( m_sceneManager->initialize() == false )
        {
            delete m_sceneManager;
            m_sceneManager = NULL;

            return false;
        }

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
		
		RESOURCE_FACTORY( ResourceImageDefault );
		RESOURCE_FACTORY( ResourceImageCombineRGBAndAlpha );
        RESOURCE_FACTORY( ResourceImageMultiplyRGBAndAlpha );
		RESOURCE_FACTORY( ResourceImageSolid );
		RESOURCE_FACTORY( ResourceMovie );
		RESOURCE_FACTORY( ResourceVideo );
		RESOURCE_FACTORY( ResourcePlaylist );
		RESOURCE_FACTORY( ResourceSound );
		RESOURCE_FACTORY( ResourceGlyph );

		RESOURCE_FACTORY( ResourceWindow );
		RESOURCE_FACTORY( ResourceHotspotImage );
		RESOURCE_FACTORY( ResourceCursorICO );
		RESOURCE_FACTORY( ResourceCursorSystem );

		RESOURCE_FACTORY( ResourceInternalObject );
        RESOURCE_FACTORY( ResourceExternal );

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

		//TVectorWString paths;
		//paths.push_back( m_baseDir );

		//m_scriptEngine->addModulePath( paths );
	}
	//////////////////////////////////////////////////////////////////////////
	const WString& Application::getBaseDir() const
	{
		return m_baseDir;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::createGame( const WString & _module, const ConstString & _language, const TVectorResourcePackDesc & _resourcePacks, const TVectorResourcePackDesc & _languagePacks )
	{
		m_game = new Game(m_baseDir, m_developmentMode, m_platformName);

		Game::keep( m_game );

		MENGE_LOG_INFO( "Application:createGame load game resource"
			);

		m_game->setLanguagePack( _language );

		if( m_game->loadConfigPaks( _resourcePacks, _languagePacks ) == false )
		{
			return false;
		}

		//m_game->registerResources( m_baseDir );

		if( m_game->applyConfigPaks() == false )
        {
            return false;
        }

		m_game->setCursorMode( m_cursorMode );

        String personalityModule;
        if( Helper::UnicodeToUtf8( m_unicodeService, _module, personalityModule ) == false )
        {
            return false;
        }
        
        ConstString c_personalityModule(personalityModule);

		if( m_game->loadPersonality( c_personalityModule ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::createRenderWindow( WindowHandle _renderWindowHandle, WindowHandle _inputWindowHandle )
	{
        if( m_windowModeCheck == true )
        {
            if( m_lowContentViewport.empty() == false )
            {
                if( m_maxClientResolution.getWidth() < m_lowContentViewport.getWidth() ||
                    m_maxClientResolution.getHeight() < m_lowContentViewport.getHeight() )
                {
                    m_fullscreen = true;
                }
            }
            else
            {
                if( m_maxClientResolution.getWidth() < m_contentResolution.getWidth() ||
                    m_maxClientResolution.getHeight() < m_contentResolution.getHeight() )
                {
                    m_fullscreen = true;
                }
            }
        }

		if( m_fullscreen == true )
		{
			m_currentResolution = this->getDesktopResolution();
		}
		else
		{
			this->calcWindowResolution( m_currentResolution );
		}

		m_renderEngine->setVSync( m_vsync );

		MENGE_LOG_WARNING( "Application::createRenderWindow Current Resolution %d %d %s"			
			, m_currentResolution.getWidth()
			, m_currentResolution.getHeight()
            , m_fullscreen ? "Fullscreen" : "Window"
			);

		this->calcRenderViewport_( m_currentResolution, m_renderViewport );

		MENGE_LOG_INFO( "Application::createRenderWindow Render Viewport %f %f - %f %f"
			, m_renderViewport.begin.x
			, m_renderViewport.begin.y
			, m_renderViewport.getWidth()
			, m_renderViewport.getHeight()
			);

		m_createRenderWindow = m_renderEngine->createRenderWindow( m_currentResolution, m_contentResolution, m_lowContentViewport, m_renderViewport, m_bits, m_fullscreen,
														_renderWindowHandle, m_FSAAType, m_FSAAQuality );

		if( m_createRenderWindow == false )
		{
            MENGE_LOG_ERROR( "Failed to create render window" 
                            );
            
			//showMessageBox( "Failed to create render window", "Critical Error", 0 );
			return false;
		}
		
		if( m_fullscreen == true )
		{
			m_platform->notifyCursorClipping( m_renderViewport );
		}
		else
		{
			m_platform->notifyCursorUnClipping();
		}

		//m_renderEngine->setRenderViewport( renderViewport );

		m_renderEngine->enableTextureFiltering( m_textureFiltering );

        m_game->initializeRenderResources();

		NotificationService::get()
			->notify( "CHANGE_WINDOW_RESOLUTION", m_fullscreen, m_currentResolution );
			
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeGame( const String & _scriptInitParams, const TMapWString & _params )
	{
		if( m_game->initialize( _scriptInitParams, _params ) == false )
		{
			MENGE_LOG_ERROR("Application::initGame invalid initialize %s"
				, _scriptInitParams.c_str()
				);

			return false;
		}
        
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::parseArguments_( const String& _arguments )
	{
		String::size_type idx_mute = _arguments.find( "-mute" );
		if( idx_mute != String::npos )
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
	bool Application::onMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, int _button, bool _isDown )
	{
		if( m_inputMouseButtonEventBlock == true )
		{
			return false;
		}

		m_game->handleMouseButtonEventBegin( _touchId, _point, _button, _isDown );
		bool result = m_game->handleMouseButtonEvent( _touchId, _point, _button, _isDown );
		m_game->handleMouseButtonEventEnd( _touchId, _point, _button, _isDown );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::onMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _dx, float _dy, int _whell )
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

		return m_game->handleMouseMove( _touchId, _point, _dx, _dy, _whell );
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

		if( m_soundService != 0 )
		{
			m_soundService->update( timing );
		}


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
			if( m_soundService )
			{
				m_soundService->onTurnStream( _turn );
			}

			if( m_game )
			{
				m_game->onTurnSound( _turn );
			}

			if( m_soundService )
			{
				m_soundService->onTurnSound( _turn );
			}
		}
		else
		{
			if( m_soundService )
			{
				m_soundService->onTurnSound( _turn );
			}

			if( m_game )
			{
				m_game->onTurnSound( _turn );
			}

			if( m_soundService )
			{
				m_soundService->onTurnStream( _turn );
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
        
		if( m_soundService )
		{
			m_soundService->finalize();
		}
        
		if( m_taskManager )
		{
			m_taskManager->finalize();
		}
        
		if( m_scriptEngine )
		{
			m_scriptEngine->finalize();
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
		
        if( m_inputEngine != NULL )
        {
            m_inputEngine->finalize();
		    delete m_inputEngine;
        }
		
		finalizeSoundService( m_soundService );
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

        delete m_notificationService;

        releaseInterfaceSystem( m_unicodeService );
        m_unicodeService = NULL;

		m_serviceProvider->unregistryService( "StringizeService" );
		delete m_stringizeService;

		delete m_watchdog;
        
		delete m_serviceProvider;
		delete m_consts;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::calcWindowResolution( Resolution & _windowResolution ) const
	{
		const Resolution & dres = this->getMaxClientAreaSize();

		float aspect = m_windowResolution.getAspectRatio();

		size_t resHeight = m_windowResolution.getHeight();
        size_t resWidth = m_windowResolution.getWidth();

		size_t dresHeight = dres.getHeight();
        size_t dresWidth = dres.getWidth();

		if( resHeight > dresHeight )
		{
			size_t new_witdh = static_cast<size_t>( float(resHeight) * aspect + 0.5f );

			_windowResolution.setWidth( new_witdh );			
			_windowResolution.setHeight( dresHeight );
		}
		else if( resWidth > dresWidth )
        {
            size_t new_height = static_cast<size_t>( float(resWidth) / aspect + 0.5f );

            _windowResolution.setWidth( dresWidth );
            _windowResolution.setHeight( new_height );
        }
        else
		{
			_windowResolution = m_windowResolution;
		}
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
	void Application::calcRenderViewport_( const Resolution & _resolution, Viewport & _viewport )
	{
        float rw = float(_resolution.getWidth());
		float rh = float(_resolution.getHeight());

        float r_aspect = _resolution.getAspectRatio();
        
		if( m_fixedContentResolution == true )
		{
            size_t contentResolutionWidth = m_contentResolution.getWidth();
            size_t contentResolutionHeight = m_contentResolution.getHeight();
            
            float c_aspect = m_contentResolution.getAspectRatio();

            Resolution contentResolution;

            if( c_aspect > r_aspect && m_lowContentViewport.empty() == false )
            {
                float width = m_lowContentViewport.getWidth();
                float height = m_lowContentViewport.getHeight();

                contentResolution.setWidth( width );
                contentResolution.setHeight( height );
            }
            else
            {
                contentResolution = m_contentResolution;
            }

            float one_div_width = 1.f / rw;
            float one_div_height = 1.f / rh;

            float contentAspect = contentResolution.getAspectRatio();

            float aspect = rw * one_div_height;

            float dw = 1.f;
            float dh = rw / contentAspect * one_div_height;

            if( dh > 1.f )
            {
                dh = 1.f;
                dw = rh * contentAspect * one_div_width;
            }

            float areaWidth = dw * rw;
            float areaHeight = dh * rh;

            _viewport.begin.x = ( rw - areaWidth ) * 0.5f;
            _viewport.begin.y = ( rh - areaHeight ) * 0.5f;
            _viewport.end.x = _viewport.begin.x + areaWidth;
            _viewport.end.y = _viewport.begin.y + areaHeight;
		}
		else
		{
			_viewport.begin.x = 0.f;
			_viewport.begin.y = 0.f;

			_viewport.end.x = rw;
			_viewport.end.y = rh;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setFullscreenMode( bool _fullscreen )
	{
        if( m_windowModeCheck == true )
        {
            if( m_lowContentViewport.empty() == false )
            {
                if( m_maxClientResolution.getWidth() < m_lowContentViewport.getWidth() ||
                    m_maxClientResolution.getHeight() < m_lowContentViewport.getHeight() )
                {
                    _fullscreen = true;
                }
            }
            else
            {
                if( m_maxClientResolution.getWidth() < m_contentResolution.getWidth() ||
                    m_maxClientResolution.getHeight() < m_contentResolution.getHeight() )
                {
                    _fullscreen = true;
                }
            }
        }

		if( m_fullscreen == _fullscreen )
		{
			return;
		}

		m_fullscreen = _fullscreen;

		if( m_fullscreen == true )
		{
			m_currentResolution = this->getDesktopResolution();
		}
		else
		{
			this->calcWindowResolution( m_currentResolution );
		}

        MENGE_LOG_WARNING( "Application::setFullscreenMode %d Current Resolution %d %d"
            , _fullscreen
            , m_currentResolution.getWidth()
            , m_currentResolution.getHeight()
            );
        
		m_renderEngine->setVSync( m_vsync );

		m_platform->notifyWindowModeChanged( m_currentResolution, m_fullscreen );

		this->calcRenderViewport_( m_currentResolution, m_renderViewport );
		//m_renderEngine->applyRenderViewport( renderViewport );

		MENGE_LOG_INFO( "Application::setFullscreenMode Render Viewport %f %f - %f %f"
			, m_renderViewport.begin.x
			, m_renderViewport.begin.y
			, m_renderViewport.getWidth()
			, m_renderViewport.getHeight()
			);

		if( m_fullscreen == true )
		{
			m_platform->notifyCursorClipping( m_renderViewport );
		}
		else
		{
			//m_interface->notifyCursorUnClipping();
			this->setMouseBounded( m_mouseBounded );
		}
	
		m_renderEngine->changeWindowMode( m_currentResolution, m_contentResolution, m_lowContentViewport, m_renderViewport, m_fullscreen );
		
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

		NotificationService::get()
			->notify( "CHANGE_WINDOW_RESOLUTION", _fullscreen, m_currentResolution );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::screenshot( RenderTextureInterface* _renderTargetImage, const mt::vec4f & _rect )
	{
		mt::vec4f res; 

		res = _rect;

		res.x *= static_cast<float>( m_currentResolution.getWidth() ) / m_contentResolution.getWidth();
		res.y *= static_cast<float>( m_currentResolution.getHeight() ) / m_contentResolution.getHeight();
		res.z *= static_cast<float>( m_currentResolution.getWidth() ) / m_contentResolution.getWidth();
		res.w *= static_cast<float>( m_currentResolution.getHeight() ) / m_contentResolution.getHeight();
		
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
	const Viewport & Application::getRenderViewport() const
	{
		return m_renderViewport;
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
		if( m_textManager == NULL )
		{
			MENGE_LOG_ERROR("Application::getProjectTitle not initialize textManager"
				);

			return Utils::emptyWString();
		}

		ConstString key("APPLICATION_TITLE");
		if( m_textManager->existText( key ) == false )
		{
			return Utils::emptyWString();
		}
		
		const TextEntry & entry = m_textManager->getTextEntry( key );

		return entry.text;
	}
	//////////////////////////////////////////////////////////////////////////
	const String & Application::getProjectCodename() const
	{
		return m_projectCodename;
	}
	//////////////////////////////////////////////////////////////////////////
	const Resolution & Application::getContentResolution() const
	{
		return m_contentResolution;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::isContentResolutionFixed() const
	{
		return m_fixedContentResolution;
	}
	//////////////////////////////////////////////////////////////////////////
	const Viewport & Application::getLowContentViewport() const
	{
		return m_lowContentViewport;
	}
	//////////////////////////////////////////////////////////////////////////
	const Resolution & Application::getWindowResolution() const
	{
		return m_windowResolution;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setDesktopResolution( const Resolution& _resolution )
	{
		m_desktopResolution = _resolution;
	}
	//////////////////////////////////////////////////////////////////////////
	const char* Application::getVersionInfo()
	{
		return s_versionInfo;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::isDebugCRT() const
	{
		return m_debugCRT;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::loadPlugin( const WString& _pluginName )
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
				MENGE_LOG_ERROR( "Application::loadPlugin can't load %S plugin [invalid load]"
					, _pluginName.c_str() 
					);

				return false;
			}

			it_found = m_dynamicLibraries.insert( std::make_pair(_pluginName, lib) ).first;
		}

        DynamicLibraryInterface * lib = it_found->second;

		TDynamicLibraryFunction function_dllCreatePlugin =
			lib->getSymbol("dllCreatePlugin");

		if( function_dllCreatePlugin == NULL )
		{
			MENGE_LOG_ERROR( "Application::loadPlugin can't load %S plugin [dllCreatePlugin]"
				, _pluginName.c_str() 
				);

			return false;
		}

		TPluginCreate dllCreatePlugin = (TPluginCreate)function_dllCreatePlugin;

		PluginInterface * plugin;
		if( dllCreatePlugin( &plugin ) == false )
		{
			MENGE_LOG_ERROR( "Application::loadPlugin can't load %S plugin [invalid create]"
				, _pluginName.c_str() 
				);

			return false;
		}

		if( plugin->initialize( m_serviceProvider ) == false )
        {
            return false;
        }
		
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
			TDynamicLibraryFunction function_dllFinalizePlugin =
				it->second->getSymbol("dllFinalizePlugin");
			
			if( function_dllFinalizePlugin != NULL )
			{
				function_dllFinalizePlugin( this );
			}					

			m_platform->unloadDynamicLibrary( it->second );			
		}

        m_dynamicLibraries.clear();
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
	void Application::setMaxClientResolution( const Resolution & _resolution )
	{
		m_maxClientResolution = _resolution;
	}
	//////////////////////////////////////////////////////////////////////////
	const Resolution& Application::getMaxClientAreaSize() const
	{
		return m_maxClientResolution;
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

		if( m_cursorMode == true && m_cursorResource != NULL )
		{
			const WString & cursorName = m_cursorResource->getPath();

			size_t cursorBufferSize;
			void * cursorBufferPtr = m_cursorResource->getBuffer( cursorBufferSize );
			m_platform->notifyCursorIconSetup( cursorName, cursorBufferPtr, cursorBufferSize );
		}

		m_invalidateCursorMode = true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::getCursorMode() const
	{
		return m_cursorMode;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setCursorIcon( const ConstString & _resourceName )
	{
		if( m_cursorResource != NULL )
		{
			m_cursorResource->release();
		}

		m_cursorResource = ResourceManager::get()
			->getResourceT<ResourceCursor>(_resourceName);

		if( m_cursorResource == NULL )
		{
			return;
		}

		if( m_cursorMode == false )
		{
			return;
		}

		const WString & cursorName = m_cursorResource->getPath();

		size_t cursorBufferSize;
		void * cursorBufferPtr = m_cursorResource->getBuffer( cursorBufferSize );

		m_platform->notifyCursorIconSetup( cursorName, cursorBufferPtr, cursorBufferSize );
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

