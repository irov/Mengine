#	include "Application.h"

#	include "Game.h"

#	include "Logger/Logger.h"

#	include "Watchdog.h"


#	include "Player.h"
#	include "Kernel/Scene.h"

#	include "Factory/FactoryDefault.h"
#	include "Factory/FactoryPool.h"

// All Node type
#	include "Kernel/Entity.h"

#	include "Animation.h"
#	include "Arrow.h"
#	include "ParticleEmitter.h"
#	include "ParticleEmitter2.h"
#	include "HotSpot.h"
//#	include "Light2D.h"
#	include "ShadowCaster2D.h"
#	include "Gyroscope.h"
#	include "TilePolygon.h"
#	include "Point.h"
#	include "SoundEmitter.h"
#	include "Sprite.h"
#	include "Isometric.h"
#	include "Mesh2D.h"
#	include "TextField.h"
#	include "TileMap.h"
#	include "Track.h"
#	include "Movie.h"
#	include "MovieSlot.h"
#	include "MovieSceneEffect.h"
#	include "MovieInternalObject.h"
#   include "MovieEvent.h"
#	include "Model3D.h"
#	include "Video.h"
#	include "Layer2D.h"
#	include "Layer2DParallax.h"
#	include "Layer2DIsometric.h"
#	include "Layer2DLoop.h"
#	include "RenderViewport.h"
#	include "Camera2D.h"
#	include "CameraTarget2D.h"
#	include "Camera3D.h"
#	include "Layer2DAccumulator.h"
#	include "Layer3D.h"
#	include "Window.h"
#	include "HotSpotImage.h"
#	include "HotSpotShape.h"
#	include "Layer2DTexture.h"
#	include "PhysicalBody2D.h"
#	include "Landscape2D.h"
// All Resource type
//#	include "ResourceSequence.h"
#	include "ResourceAnimation.h"
#	include "ResourceEmitterContainer.h"
#	include "ResourceEmitter.h"

#	include "Kernel/ResourceImageData.h"

#	include "ResourceImageSolid.h"
#	include "ResourceImageDefault.h"
#	include "ResourceImageSubstract.h"
#	include "ResourceImageMultiplyRGBAndAlpha.h"

//#	include "ResourceBinary.h"
#	include "ResourceFile.h"
#	include "ResourceMovie.h"
#	include "ResourceVideo.h"
#	include "ResourceMesh.h"
#	include "ResourceSkeleton.h"
#	include "ResourceSound.h"
#	include "ResourceTileMap.h"
#	include "ResourceTileSet.h"
#	include "ResourceModel3D.h"
//#	include "ResourceMeshMS3D.h"
//#	include "ResourceMeshNoise.h"
#	include "ResourceMaterial.h"
#	include "ResourceWindow.h"
#   include "ResourceHIT.h"
#   include "ResourceShape.h"
#	include "ResourceCursorICO.h"
#	include "ResourceCursorSystem.h"
#	include "ResourceInternalObject.h"

#	include "Interface/UnicodeInterface.h"

#	include "ScriptWrapper.h"

#	include "Kernel/NodePrototypeGenerator.h"
#	include "Kernel/ResourcePrototypeGenerator.h"

#	include "Codec/ImageDecoderMemory.h"
#	include "Codec/ImageDecoderArchive.h"

#	include "Codec/DecoderFactory.h"

#	include "Config/Config.h"

#   include "Core/String.h"
#	include "Core/IniUtil.h"

#   include "stdex/allocator.h"
#   include "stdex/stl_list.h"

#	include <ctime>
#	include <sstream>

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN(Consts, Menge::Consts);
SERVICE_EXTERN(TextService, Menge::TextServiceInterface);
SERVICE_EXTERN(NodeService, Menge::NodeServiceInterface);
SERVICE_EXTERN(LoaderService, Menge::LoaderServiceInterface);
SERVICE_EXTERN(ResourceService, Menge::ResourceServiceInterface);
SERVICE_EXTERN(Watchdog, Menge::WatchdogInterface);
SERVICE_EXTERN(GameService, Menge::GameServiceInterface);
SERVICE_EXTERN(PrototypeService, Menge::PrototypeServiceInterface);
SERVICE_EXTERN(Graveyard, Menge::GraveyardInterface);
//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( Application, Menge::ApplicationInterface, Menge::Application );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Application::Application()
		: m_particles(true)
		, m_debugMask(0)
		, m_phycisTiming(0.f)
		, m_resetTiming(false)
		, m_maxTiming(100.f)
        , m_mouseBounded(false)
        , m_allowFullscreenSwitchShortcut(false)
        , m_game(nullptr)
        , m_focus(true)
        , m_update(true)				
        , m_console(nullptr)
		, m_loaderService(nullptr)
		, m_resourceService(nullptr)
		, m_textService(nullptr)
        , m_nodeService(nullptr)
		, m_prototypeService(nullptr)		
		, m_createRenderWindow(false)
		, m_cursorMode(false)
		, m_invalidateVsync(false)
		, m_invalidateCursorMode(false)
		, m_fullscreen(false)		
		, m_inputMouseButtonEventBlock(false)
		, m_countThreads(2)
		, m_mouseEnter(false)
		, m_developmentMode(false)
        , m_nofullscreenMode(false)
        , m_resourceCheck(true)
		, m_resourceCheckCritical(true)
		, m_cursorResource(nullptr)
		, m_fixedContentResolution(false)
		, m_fixedDisplayResolution(false)
		, m_vsync(false)
		, m_bits(0)
		, m_FSAAType(0)
		, m_FSAAQuality(0)
		, m_textureFiltering(true)
        , m_windowModeCheck(false)
        , m_watchdog(nullptr)
        , m_profiler(nullptr)
		, m_graveyard(nullptr)
		, m_projectVersion(0)
		, m_debugPause(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Application::~Application()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void Application::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * Application::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Application::getPlatformName() const
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
			typedef stdex::list<TInitializeMethod> TListInitializeMethods;

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
	bool Application::initialize( const String & _args )
	{   
        SERVICE_CREATE( Consts, &m_consts );
        SERVICE_REGISTRY( m_serviceProvider, m_consts );

        m_consts->initialize();

        ExecuteInitialize exinit( this );
				
        exinit.add( &Application::initializePrototypeManager_ );
        exinit.add( &Application::initializeNodeManager_ );        
        exinit.add( &Application::initializeLoaderEngine_ );
        exinit.add( &Application::initializeResourceManager_ );
        exinit.add( &Application::initializeSceneManager_ );
        exinit.add( &Application::initializeTextManager_ );        
        exinit.add( &Application::initializeWatchdog_ );
        exinit.add( &Application::initializeProfiler_ );
		exinit.add( &Application::initializeGraveyard_ );

        if( exinit.run() == false )
        {
            return false;
        }

        ScriptWrapper::constsWrap( m_serviceProvider );
        ScriptWrapper::mathWrap( m_serviceProvider );
        ScriptWrapper::nodeWrap( m_serviceProvider );
        ScriptWrapper::helperWrap( m_serviceProvider );
        ScriptWrapper::soundWrap( m_serviceProvider );
        ScriptWrapper::entityWrap( m_serviceProvider );


		DecoderFactoryInterfacePtr imageDecoderMemory = new DecoderFactory<ImageDecoderMemory>(m_serviceProvider, CONST_STRING(m_serviceProvider, memoryImage) );
		DecoderFactoryInterfacePtr imageDecoderArchive = new DecoderFactory<ImageDecoderArchive>(m_serviceProvider, CONST_STRING(m_serviceProvider, archiveImage) );
		
		CODEC_SERVICE(m_serviceProvider)
			->registerDecoder( CONST_STRING(m_serviceProvider, memoryImage), imageDecoderMemory );

		CODEC_SERVICE(m_serviceProvider)
			->registerDecoder( CONST_STRING(m_serviceProvider, archiveImage), imageDecoderArchive );

		
		this->parseArguments_( _args );
	
		m_companyName = CONFIG_VALUE(m_serviceProvider, "Project", "Company", L"NONAME");
		m_projectName = CONFIG_VALUE(m_serviceProvider, "Project", "Name", L"UNKNOWN");

		m_platformName = CONFIG_VALUE(m_serviceProvider, "Project", "Platform", CONST_STRING_LOCAL(m_serviceProvider, "WIN"));
		m_projectCodename = CONFIG_VALUE(m_serviceProvider, "Project", "Codename", ConstString::none());
		m_projectVersion = CONFIG_VALUE(m_serviceProvider, "Project", "Version", 0U);

        m_contentResolution = CONFIG_VALUE(m_serviceProvider, "Game", "ContentResolution", Resolution(1024, 768));
        m_fixedContentResolution = CONFIG_VALUE(m_serviceProvider, "Game", "FixedContentResolution", true);
		m_fixedDisplayResolution = CONFIG_VALUE(m_serviceProvider, "Game", "FixedDisplayResolution", true);
		m_windowModeCheck = CONFIG_VALUE(m_serviceProvider, "Game", "WindowModeCheck", false);
		

		TVectorAspectRatioViewports aspectRatioViewports;
		CONFIG_VALUES(m_serviceProvider, "Game", "AspectRatioViewport", aspectRatioViewports);

        for( TVectorAspectRatioViewports::const_iterator
            it = aspectRatioViewports.begin(),
            it_end = aspectRatioViewports.end();
        it != it_end;
        ++it )
        {
           float aspect = it->width / it->height;

           m_aspectRatioViewports[aspect] = it->viewport;
        }

        return true;
    }
	/////////////////////////////////////////////////////
	bool Application::loadResourcePacks_( const ConstString & _fileGroup, const FilePath & _resourceIni )
	{
		InputStreamInterfacePtr resourceInputStream = 
			FILE_SERVICE(m_serviceProvider)->openInputFile( _fileGroup, _resourceIni, false );

		if( resourceInputStream == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Application::loadResourcePacks_ Invalid open resourcesPack setting '%s'"
				, _resourceIni.c_str()
				);  

			return false;
		}

		IniUtil::IniStore ini;
		if( IniUtil::loadIni( ini, resourceInputStream, m_serviceProvider ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Application::loadResourcePacks_ Invalid load resource settings '%s'"
				, _resourceIni.c_str()
				);

			return false;
		}

		resourceInputStream = nullptr;

		TVectorString resourcePacksSettings;
		IniUtil::getIniValue( ini, "GAME_RESOURCES", "ResourcePack", resourcePacksSettings, m_serviceProvider );

		ConstString c_dir = CONST_STRING_LOCAL(m_serviceProvider, "dir");

		for( TVectorString::iterator
			it = resourcePacksSettings.begin(),
			it_end = resourcePacksSettings.end();
		it != it_end;
		++it )
		{
			const String & resourcePack = *it;

			ResourcePackDesc pack;

			pack.dev = false;
			pack.preload = true;
			pack.type = c_dir;

			if( ini.hasSection( resourcePack.c_str() ) == false )
			{
				LOGGER_CRITICAL(m_serviceProvider)("Application::loadResourcePacks_ %s invalid load resource pack no found section for '%s'"
					, _resourceIni.c_str()
					, resourcePack.c_str()
					);

				return false;
			}

			IniUtil::getIniValue( ini, resourcePack.c_str(), "Name", pack.name, m_serviceProvider );
			IniUtil::getIniValue( ini, resourcePack.c_str(), "Type", pack.type, m_serviceProvider );            
			IniUtil::getIniValue( ini, resourcePack.c_str(), "Path", pack.path, m_serviceProvider );
			IniUtil::getIniValue( ini, resourcePack.c_str(), "Locale", pack.locale, m_serviceProvider );
			IniUtil::getIniValue( ini, resourcePack.c_str(), "Platform", pack.platform, m_serviceProvider );
			IniUtil::getIniValue( ini, resourcePack.c_str(), "Description", pack.descriptionPath, m_serviceProvider );
			IniUtil::getIniValue( ini, resourcePack.c_str(), "Dev", pack.dev, m_serviceProvider );
			IniUtil::getIniValue( ini, resourcePack.c_str(), "PreLoad", pack.preload, m_serviceProvider );			

			m_game->addResourcePak( pack );
		}

		TVectorString languagePackSettings;
		IniUtil::getIniValue( ini, "GAME_RESOURCES", "LanguagePack", languagePackSettings, m_serviceProvider );

		for( TVectorString::iterator
			it = languagePackSettings.begin(),
			it_end = languagePackSettings.end();
		it != it_end;
		++it )
		{
			const String & languagePack = *it;

			ResourcePackDesc pack;

			pack.dev = false;
			pack.preload = true;
			pack.type = c_dir;

			if( ini.hasSection( languagePack.c_str() ) == false )
			{
				LOGGER_CRITICAL(m_serviceProvider)("Application::loadResourcePacks_ %s invalid load language pack no found section for '%s'"
					, _resourceIni.c_str()
					, languagePack.c_str()
					);

				return false;
			}

			IniUtil::getIniValue( ini, languagePack.c_str(), "Name", pack.name, m_serviceProvider );
			IniUtil::getIniValue( ini, languagePack.c_str(), "Type", pack.type, m_serviceProvider );
			IniUtil::getIniValue( ini, languagePack.c_str(), "Path", pack.path, m_serviceProvider );
			IniUtil::getIniValue( ini, languagePack.c_str(), "Locale", pack.locale, m_serviceProvider );
			IniUtil::getIniValue( ini, languagePack.c_str(), "Platform", pack.platform, m_serviceProvider );
			IniUtil::getIniValue( ini, languagePack.c_str(), "Description", pack.descriptionPath, m_serviceProvider );
			IniUtil::getIniValue( ini, languagePack.c_str(), "Dev", pack.dev, m_serviceProvider );
			IniUtil::getIniValue( ini, languagePack.c_str(), "PreLoad", pack.preload, m_serviceProvider );	

			m_game->addLanguagePak( pack );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeNodeManager_()
	{	
		NodeServiceInterface * nodeService;
        if( SERVICE_CREATE( NodeService, &nodeService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, nodeService ) == false )
        {
            return false;
        }

        m_nodeService = nodeService;

        m_nodeService->initialize();

/*#	define NODE_FACTORY( serviceProvider, Type )\
		nodeService->registerFactory( Helper::stringizeString( serviceProvider, #Type), new FactoryPool<Type, 128> )
*/

#	define NODE_FACTORY( serviceProvider, Type )\
        PROTOTYPE_SERVICE(serviceProvider)\
            ->addPrototype( CONST_STRING(serviceProvider, Node), CONST_STRING_LOCAL( serviceProvider, #Type), new NodePrototypeGenerator<Type, 128>(serviceProvider) );

		LOGGER_WARNING(m_serviceProvider)("Creating Object Factory..." );

		NODE_FACTORY( m_serviceProvider, Node );

		NODE_FACTORY( m_serviceProvider, Entity );
		NODE_FACTORY( m_serviceProvider, Arrow );
		NODE_FACTORY( m_serviceProvider, Scene );

		NODE_FACTORY( m_serviceProvider, Animation );
		NODE_FACTORY( m_serviceProvider, Gyroscope );
		NODE_FACTORY( m_serviceProvider, Isometric );
		
		NODE_FACTORY( m_serviceProvider, ParticleEmitter );
		NODE_FACTORY( m_serviceProvider, ParticleEmitter2 );

		NODE_FACTORY( m_serviceProvider, HotSpot );
        NODE_FACTORY( m_serviceProvider, HotSpotImage );
        NODE_FACTORY( m_serviceProvider, HotSpotShape );
		//NODE_FACTORY( Light2D );
		//NODE_FACTORY( ShadowCaster2D );
		//NODE_FACTORY( TilePolygon );
		NODE_FACTORY( m_serviceProvider, Point );
		//NODE_FACTORY( RigidBody2D );
		//NODE_FACTORY( PhysicalBody2D );
		NODE_FACTORY( m_serviceProvider, SoundEmitter );
		NODE_FACTORY( m_serviceProvider, Sprite );
		NODE_FACTORY( m_serviceProvider, Mesh2D );
		NODE_FACTORY( m_serviceProvider, TextField );
		//NODE_FACTORY( TileMap );
		//NODE_FACTORY( Track );
		NODE_FACTORY( m_serviceProvider, Movie );
        NODE_FACTORY( m_serviceProvider, MovieSlot );
        NODE_FACTORY( m_serviceProvider, MovieSceneEffect );
		NODE_FACTORY( m_serviceProvider, MovieInternalObject );
        NODE_FACTORY( m_serviceProvider, MovieEvent );

		NODE_FACTORY( m_serviceProvider, Model3D );
		//NODE_FACTORY( Model );
		NODE_FACTORY( m_serviceProvider, Video );
		NODE_FACTORY( m_serviceProvider, Layer2D );
		NODE_FACTORY( m_serviceProvider, Landscape2D );
		NODE_FACTORY( m_serviceProvider, Layer2DParallax );
		NODE_FACTORY( m_serviceProvider, Layer2DIsometric );
		//NODE_FACTORY( m_serviceProvider, Layer2DPhysic );
		//NODE_FACTORY( Layer2DLoop );
		//NODE_FACTORY( Layer2DAccumulator );
		//NODE_FACTORY( Layer2DTexture );
		//NODE_FACTORY( LayerScene );
		//NODE_FACTORY( RenderMesh );
		NODE_FACTORY( m_serviceProvider, RenderViewport );
		NODE_FACTORY( m_serviceProvider, Camera2D );
		NODE_FACTORY( m_serviceProvider, CameraTarget2D );
		NODE_FACTORY( m_serviceProvider, Camera3D );
		//NODE_FACTORY( SceneNode3D );
		NODE_FACTORY( m_serviceProvider, Window );
		
#	undef NODE_FACTORY

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeLoaderEngine_()
	{
		LOGGER_WARNING(m_serviceProvider)("Initializing Loader Engine..." );

		LoaderServiceInterface * loaderService;
        if( SERVICE_CREATE( LoaderService, &loaderService ) == false )
        {
            return false;
        }

        SERVICE_REGISTRY( m_serviceProvider, loaderService );

		if( loaderService->initialize( CONST_STRING_LOCAL(m_serviceProvider, "protocol.xml") ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Application::initializeLoaderEngine_ invalid initialize Loader Engine"
				);

			return false;
		}

        m_loaderService = loaderService;

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    bool Application::initializePrototypeManager_()
    {
        LOGGER_WARNING(m_serviceProvider)("Inititalizing PrototypeManager..." );

        PrototypeServiceInterface * prototypeService;

        if( SERVICE_CREATE( PrototypeService, &prototypeService ) == false )
        {
            return false;
        }

        SERVICE_REGISTRY( m_serviceProvider, prototypeService );

        m_prototypeService = prototypeService;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::initializeWatchdog_()
    {
        LOGGER_WARNING(m_serviceProvider)("Inititalizing Watchdog..." );

        WatchdogInterface * watchdog;
        if( SERVICE_CREATE( Watchdog, &watchdog ) == false )
        {
            return false;
        }

        SERVICE_REGISTRY( m_serviceProvider, watchdog );

        m_watchdog = watchdog;

        return true;    
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::initializeProfiler_()
    {
        //LOGGER_ERROR(m_serviceProvider)("Inititalizing ProfilerService..." );

        //ProfilerServiceInterface * profiler;
        //if( SERVICE_CREATE( ProfilerService, &profiler ) == false )
        //{
        //    return false;
        //}

        //SERVICE_REGISTRY( m_serviceProvider, profiler );

        //profiler->initialize();

        //m_profiler = profiler;

        return true;    
    }
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeGraveyard_()
	{
		LOGGER_WARNING(m_serviceProvider)("Inititalizing Graveyard..." );

		GraveyardInterface * graveyard;
		if( SERVICE_CREATE( Graveyard, &graveyard ) == false )
		{
			return false;
		}

		SERVICE_REGISTRY( m_serviceProvider, graveyard );

		if( graveyard->initialize() == false )
		{
			return false;
		}

		m_graveyard = graveyard;

		return true;    
	}
    //////////////////////////////////////////////////////////////////////////
    namespace
    {
        class SceneCategoryGenerator
            : public PrototypeGeneratorInterface
        {
        public:
            SceneCategoryGenerator( ServiceProviderInterface * _serviceProvider )
                : m_serviceProvider(_serviceProvider)
            {
            }

        protected:
            Factorable * generate( const ConstString & _category, const ConstString & _prototype ) override
            {
                Scene * scene = NODE_SERVICE(m_serviceProvider)
                    ->createNodeT<Scene>( CONST_STRING(m_serviceProvider, Scene) );

                if( scene == nullptr )
                {
                    LOGGER_ERROR(m_serviceProvider)("SceneCategoryGenerator can't create %s %s"
                        , _category.c_str()
                        , _prototype.c_str()
                        );

                    return nullptr;
                }

                scene->setName( _prototype );

                return scene;
            }

            uint32_t count() const override
            {
                return 0;
            }

            void destroy() override
            {
                delete this;
            }

        protected:
            ServiceProviderInterface * m_serviceProvider;
        };
    }
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeSceneManager_()
	{
		LOGGER_INFO(m_serviceProvider)( "initialize Scene Manager..." );

        //SceneServiceInterface * sceneService;
        //if( createSceneService( &sceneService ) == false )
        //{
        //    return false;
        //}

        //SERVICE_REGISTRY( m_serviceProvider, sceneService );

        //m_sceneService = sceneService;

        PrototypeGeneratorInterface * generator = new SceneCategoryGenerator(m_serviceProvider);

        PROTOTYPE_SERVICE(m_serviceProvider)
            ->addPrototype( CONST_STRING(m_serviceProvider, Scene), ConstString::none(), generator );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeResourceManager_()
	{
        LOGGER_INFO(m_serviceProvider)( "Initializing Resource Manager..." );

        ResourceServiceInterface * resourceService;
        if( SERVICE_CREATE( ResourceService, &resourceService ) == false )
        {
            return false;
        }

        SERVICE_REGISTRY( m_serviceProvider, resourceService );
        
        m_resourceService = resourceService;


#	define RESOURCE_FACTORY( serviceProvider, Type ) \
    PROTOTYPE_SERVICE(serviceProvider)->addPrototype( CONST_STRING(serviceProvider, Resource), CONST_STRING_LOCAL(serviceProvider, #Type), new ResourcePrototypeGenerator<Type, 128>(m_serviceProvider) )

		RESOURCE_FACTORY( m_serviceProvider, ResourceAnimation );

		RESOURCE_FACTORY( m_serviceProvider, ResourceEmitterContainer );
		RESOURCE_FACTORY( m_serviceProvider, ResourceEmitter );

		RESOURCE_FACTORY( m_serviceProvider, ResourceParticle );

		//RESOURCE_FACTORY( m_serviceProvider, ResourceFont );
		//RESOURCE_FACTORY( m_serviceProvider, ResourceGlyph );
		
		RESOURCE_FACTORY( m_serviceProvider, ResourceImageData );
		RESOURCE_FACTORY( m_serviceProvider, ResourceImageDefault );
        RESOURCE_FACTORY( m_serviceProvider, ResourceImageSubstract );
        RESOURCE_FACTORY( m_serviceProvider, ResourceImageMultiplyRGBAndAlpha );
		RESOURCE_FACTORY( m_serviceProvider, ResourceImageSolid );
		RESOURCE_FACTORY( m_serviceProvider, ResourceMovie );
		RESOURCE_FACTORY( m_serviceProvider, ResourceModel3D );
		RESOURCE_FACTORY( m_serviceProvider, ResourceVideo );
		RESOURCE_FACTORY( m_serviceProvider, ResourceSound );		
		RESOURCE_FACTORY( m_serviceProvider, ResourceFile );

		RESOURCE_FACTORY( m_serviceProvider, ResourceWindow );
        RESOURCE_FACTORY( m_serviceProvider, ResourceHIT );
        RESOURCE_FACTORY( m_serviceProvider, ResourceShape );
		RESOURCE_FACTORY( m_serviceProvider, ResourceCursorICO );
		RESOURCE_FACTORY( m_serviceProvider, ResourceCursorSystem );

		RESOURCE_FACTORY( m_serviceProvider, ResourceInternalObject );

#	undef RESOURCE_FACTORY

		m_resourceService->initialize();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeTextManager_()
	{
		LOGGER_INFO(m_serviceProvider)( "initialize Text Manager..." );

        TextServiceInterface * textService;
        if( SERVICE_CREATE( TextService, &textService ) == false )
        {
            return false;
        }

        m_textService = textService;

        if( SERVICE_REGISTRY( m_serviceProvider, m_textService ) == false )
        {
            return false;
        }

		m_textService->initialize();
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setDefaultWindowDescription( const Resolution & _resolution, uint32_t _bits, bool _fullscreen, bool _vsync )
	{
		m_windowResolution = _resolution;
		m_bits = _bits;
		m_fullscreen = _fullscreen;
		m_vsync = _vsync;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::createGame( const ConstString & _personalityModule, const ConstString & _language, const ConstString & _resourcesIniGroup, const FilePath & _resourcesIniPath )
	{
        SERVICE_CREATE(GameService, &m_game);

        if( SERVICE_REGISTRY( m_serviceProvider, m_game ) == false )
        {
            LOGGER_INFO(m_serviceProvider)( "Application:createGame invalid register game service"
                );

            return false;
        }

        m_game->setDevelopmentMode( m_developmentMode );
        m_game->setPlatformName( m_platformName );
        
		LOGGER_INFO(m_serviceProvider)( "Application:createGame load game resource"
			);

		m_game->setLanguagePack( _language );

		if( this->loadResourcePacks_( _resourcesIniGroup, _resourcesIniPath ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Application::setup Invalid load resourcesPack setting %s:%s"
				, _resourcesIniGroup.c_str()
				, _resourcesIniPath.c_str()
				);

			return false;
		}
					
		if( m_game->applyConfigPaks() == false )
        {
            return false;
        }

		if( m_developmentMode == true )
		{
			if( TEXT_SERVICE(m_serviceProvider)
				->validate() == false )
			{
				LOGGER_ERROR(m_serviceProvider)("TextService invalid validate!"
					);
			}
		}

		const ConstString & defaultFontName = TEXT_SERVICE(m_serviceProvider)
			->getDefaultFontName();

		if( defaultFontName.empty() == true )
		{
			LOGGER_ERROR(m_serviceProvider)("TextService not setup default font name!"
				);
		}

        if( m_developmentMode == true && m_resourceCheck == true )
        {
            if( RESOURCE_SERVICE(m_serviceProvider)
                ->validationResources() == false )
			{
				LOGGER_ERROR(m_serviceProvider)("Resources validation is invalid!!!!!!!!!!!!!"
					);

				if( m_resourceCheckCritical == true )
				{
					LOGGER_CRITICAL(m_serviceProvider)("Fix Resources"
						);

					return false;
				}
			}
        }
        
		if( m_game->loadPersonality( _personalityModule ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::createRenderWindow( WindowHandle _renderWindowHandle )
	{
        //if( this->isValidWindowMode() == false )
        //{
        //    m_fullscreen = true;
        //}

        bool fullscreen = this->getFullscreenMode();

		if( fullscreen == true )
		{
            Resolution desktopResolution;
            PLATFORM_SERVICE(m_serviceProvider)
				->getDesktopResolution( desktopResolution );

            m_currentResolution = desktopResolution;
		}
		else
		{
			this->calcWindowResolution( m_currentResolution );
		}

		RENDER_SERVICE(m_serviceProvider)->setVSync( m_vsync );

		LOGGER_WARNING(m_serviceProvider)( "Application::createRenderWindow current resolution %d %d %s"			
			, m_currentResolution.getWidth()
			, m_currentResolution.getHeight()
            , fullscreen ? "Fullscreen" : "Window"
			);

		this->calcRenderViewport_( m_currentResolution, m_renderViewport );

		LOGGER_INFO(m_serviceProvider)( "Application::createRenderWindow render viewport %f %f - %f %f"
			, m_renderViewport.begin.x
			, m_renderViewport.begin.y
			, m_renderViewport.getWidth()
			, m_renderViewport.getHeight()
			);

		m_createRenderWindow = RENDER_SERVICE(m_serviceProvider)->createRenderWindow( m_currentResolution
			, m_contentResolution
			, m_renderViewport
			, m_bits
			, fullscreen
			, _renderWindowHandle
			, m_FSAAType
			, m_FSAAQuality );

		if( m_createRenderWindow == false )
		{
            LOGGER_ERROR(m_serviceProvider)("Application::createRenderWindow failed to create render window" 
                            );            

			return false;
		}
		
        if( m_mouseBounded == true )
        {
            if( fullscreen == true )
            {
                PLATFORM_SERVICE(m_serviceProvider)
					->notifyCursorClipping( m_renderViewport );
            }
            else
            {
                PLATFORM_SERVICE(m_serviceProvider)
					->notifyCursorUnClipping();
            }
        }

        m_game->initializeRenderResources();

		NOTIFICATION_SERVICE(m_serviceProvider)
            ->notify( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION, fullscreen, m_currentResolution );
		
		m_game->setRenderViewport( m_renderViewport, m_contentResolution );

		float gameViewportAspect;
		Viewport gameViewport;

		this->getGameViewport( gameViewportAspect, gameViewport );

		m_game->setGameViewport( gameViewport, gameViewportAspect );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeGame( const FilePath & _accountPath, const String & _scriptInitParams )
	{
		TMapParams params;		
		CONFIG_SECTION(m_serviceProvider, "Params", params);

		if( m_game->initialize( _accountPath, m_projectVersion, params, _scriptInitParams ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Application::initGame invalid initialize"
				);

			return false;
		}

        m_game->setCursorMode( m_cursorMode );

		m_game->run();
		        
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::finalizeGame()
	{
		if( m_game != nullptr )
		{
			m_game->finalize();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::parseArguments_( const String& _arguments )
	{
		String::size_type idx_particles = _arguments.find( " -particles " );
		if( idx_particles != String::npos )
		{
			m_particles = false;
		}

        String::size_type idx_dev = _arguments.find( " -dev " );
        if( idx_dev != String::npos )
        {
            m_developmentMode = true;
        }

        String::size_type idx_noresourcecheck = _arguments.find( " -noresourcecheck " );
        if( idx_noresourcecheck != String::npos )
        {
            m_resourceCheck = false;
        }

		String::size_type idx_noresourcecheckcritical = _arguments.find( " -noresourcecheckcritical " );
		if( idx_noresourcecheckcritical != String::npos )
		{
			m_resourceCheckCritical = false;
		}

        String::size_type idx_nofullscreen = _arguments.find( " -nofullscreen " );
        if( idx_nofullscreen != String::npos )
        {
            m_nofullscreenMode = true;
        }
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::keyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown, bool _repeating )
	{
		if( m_console != nullptr )
		{
			m_console->proccessInput( _key, _char, _isDown );
		}

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

			if( _key == KC_F5 && _isDown == true )
			{
				m_resourceService->dumpResources("Application");


			}

			if( _key == KC_OEM_MINUS && _isDown == true )
			{
				if( RENDER_SERVICE(m_serviceProvider)
					->decrefLimitRenderObjects() == false )
				{
					m_debugPause = false;

					RENDER_SERVICE(m_serviceProvider)
						->enableDebugMode( false );
				}				
			}

			if( _key == KC_OEM_PLUS && _isDown == true )
			{
				RENDER_SERVICE(m_serviceProvider)
					->increfLimitRenderObjects();

				if( m_debugPause == false )
				{
					RENDER_SERVICE(m_serviceProvider)
						->enableDebugMode( true );
				}

				m_debugPause = true;
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

			if( _key == KC_F11 && _isDown == true )
			{
                PLAYER_SERVICE(m_serviceProvider)
					->toggleDebugText();
			}

			if( _key == KC_P && _isDown == true && INPUT_SERVICE(m_serviceProvider)->isKeyDown( KC_CONTROL ) == true )
			{
				static bool s_particle_enable = true;

				s_particle_enable = !s_particle_enable;

				this->setParticlesEnabled( s_particle_enable );
			}

			if( _key == KC_0 && _isDown == true )
			{
				static uint32_t batchMode = RENDER_SERVICE(m_serviceProvider)
					->getBatchMode();

				++batchMode;

				ERenderBatchMode mode = ERBM_NONE;
				switch(batchMode %3)
				{
				case 0:
					mode = ERBM_NONE;
					break;
				case 1:
					mode = ERBM_NORMAL;
					break;
				case 2:
					mode = ERBM_SMART;
					break;
				}

				RENDER_SERVICE(m_serviceProvider)
					->setBatchMode( mode );
			}

            if( _key == KC_F3 && _isDown == true )
            {
				stdex_memory_info_t mi[25];
                uint32_t count = stdex_allocator_memoryinfo( mi, 25 );

				size_t pool_now = 0;
				size_t pool_max = 0;

				for( uint32_t i = 0; i != count; ++i )
				{
					const stdex_memory_info_t & m = mi[i];

					pool_now += m.block_size * m.block_count;
					pool_max += m.block_size * m.block_total * m.chunk_count;

					printf("block %lu:%lu %lu alloc %lu:%lu over %.2f\n"
						, m.block_size
						, m.chunk_count
						, m.block_count
						, m.block_size * m.block_count
						, m.block_size * m.block_total * m.chunk_count
						, ( m.block_count > 0 ) ? float(m.block_size * m.block_total * m.chunk_count) / float(m.block_size * m.block_count) : 0.f
						);
				}

				float pool_now_mb = float(pool_now / (1024.f * 1024.f));
				float pool_max_mb = float(pool_max / (1024.f * 1024.f));
				printf("-------------------------------------\n");
				printf("pool %.3f:%.3f\n", pool_now_mb, pool_max_mb);

				size_t global = stdex_allocator_globalmemoryuse();
				float global_mb = float(global / (1024.f * 1024.f));
				printf("-------------------------------------\n");
				printf("global %.3f\n", global_mb);

				float total_now_mb = pool_now_mb + global_mb;;
				float total_max_mb = pool_max_mb + global_mb;
				printf("-------------------------------------\n");
				printf("total %.3f:%.3f\n", total_now_mb, total_max_mb);
            }
		}

		bool handle = m_game->handleKeyEvent( _point, _key, _char, _isDown, _repeating );

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::mouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, int _button, bool _isDown )
	{
		if( m_inputMouseButtonEventBlock == true )
		{
			return false;
		}

		m_game->handleMouseButtonEventBegin( _touchId, _point, _button, _isDown );
		bool handle = m_game->handleMouseButtonEvent( _touchId, _point, _button, _isDown );
		m_game->handleMouseButtonEventEnd( _touchId, _point, _button, _isDown );

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::mouseMove( unsigned int _touchId, const mt::vec2f & _point, float _dx, float _dy )
	{
		if( INPUT_SERVICE(m_serviceProvider)
			->validCursorPosition( _point ) == false )
		{
			m_mouseEnter = false;

			return false;
		}

		if( m_mouseEnter == false )
		{
			this->mouseEnter( _point );
		}

		bool handle = m_game->handleMouseMove( _touchId, _point, _dx, _dy );

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::mouseWheel( unsigned int _touchId, const mt::vec2f & _point, int _wheel )
	{
		bool handle = m_game->handleMouseWheel( _touchId, _point, _wheel );

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::mousePosition( unsigned int _touchId, const mt::vec2f & _point )
	{
		(void)_touchId;

		if( INPUT_SERVICE(m_serviceProvider)
			->validCursorPosition( _point ) == false )
		{
			m_mouseEnter = false;

			return;
		}

		if( m_mouseEnter == false )
		{
			this->mouseEnter( _point );
		}

		m_game->mouseEnter( _point );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::mouseLeave()
	{
		m_mouseEnter = false;

		m_game->mouseLeave();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::mouseEnter( const mt::vec2f & _point )
	{
		if( INPUT_SERVICE(m_serviceProvider)
			->validCursorPosition( _point ) == false )
		{
			return;
		}

		m_mouseEnter = true;

		m_game->mouseEnter( _point );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::quit()	
	{
		PLATFORM_SERVICE(m_serviceProvider)
			->stop();
		
		RENDER_SERVICE(m_serviceProvider)
			->onWindowClose();
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
	void Application::setFocus( bool _focus, const mt::vec2f & _point )
	{
		//return;
		if( m_focus == _focus ) 
		{
			return;
		}
		
		m_focus = _focus;

		LOGGER_INFO(m_serviceProvider)("Application::onFocus from %d to %d"
			, m_focus
			, _focus
			);

		if( m_game != nullptr )
		{
			m_game->setFocus( m_focus );

			if( m_focus == false )
			{
				this->mouseLeave();
			}
			else
			{
				this->mouseEnter( _point );
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
		PLATFORM_SERVICE(m_serviceProvider)
			->minimizeWindow();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::beginUpdate()
	{	
		if( SERVICE_EXIST(m_serviceProvider, Menge::ThreadServiceInterface) == true )
		{
			THREAD_SERVICE(m_serviceProvider)->update();
		}

		if( m_update == false && m_focus == false )
		{						
			return false;
		}

		INPUT_SERVICE(m_serviceProvider)
			->update();

		if( SERVICE_EXIST(m_serviceProvider, Menge::ParticleServiceInterface) == true )
		{
			PARTICLE_SERVICE(m_serviceProvider)
				->update();
		}

		if( SERVICE_EXIST(m_serviceProvider, Menge::ParticleServiceInterface2) == true )
		{
			PARTICLE_SERVICE2(m_serviceProvider)
				->update();
		}

		if( m_game->beginUpdate() == false )
		{
			this->quit();

			return false;
		}

        //EVENT_SERVICE(m_serviceProvider)->update();

		if( m_focus == false && m_update == true )
		{
			m_update = false;
		}
		else if( m_focus == true && m_update == false )
		{
			m_update = true;
		}

		this->updateNotification();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::tick( float _timing )
	{
		float timing = _timing;

		if( m_debugPause == true )
		{
			timing = 0.f;
		}

		if( _timing > m_maxTiming )
		{
			timing = m_maxTiming;
		}

		m_game->tick( timing );

		if( SERVICE_EXIST(m_serviceProvider, Menge::SoundServiceInterface) == true )
		{
			SOUND_SERVICE(m_serviceProvider)
				->update( timing );
		}

		if( SERVICE_EXIST(m_serviceProvider, Menge::GraveyardInterface) == true )
		{
			GRAVEYARD_SERVICE(m_serviceProvider)
				->update( timing );
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
	void Application::endUpdate()
	{
		m_game->endUpdate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::render()
	{
		if( RENDER_SERVICE(m_serviceProvider)->beginScene() == false )
		{
			return false;
		}

		m_game->render();

		if( m_console != nullptr )
		{
			m_console->render();
		}		

		RENDER_SERVICE(m_serviceProvider)->endScene();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::flush()
	{
		RENDER_SERVICE(m_serviceProvider)
            ->swapBuffers();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::close()
	{
		bool needQuit = true;

		if( m_game != nullptr )
		{
			needQuit = m_game->close();
		}

		if( needQuit == true )
		{
			quit();
		}
	}
    //////////////////////////////////////////////////////////////////////////
    void Application::turnSound( bool _turn )
    {
		if( _turn == false )
		{
			if( SERVICE_EXIST(m_serviceProvider, SoundServiceInterface) == true )
			{
				SOUND_SERVICE(m_serviceProvider)->onTurnStream( false );
			}

			if( m_game != nullptr )
			{
				m_game->turnSound( false );
			}

			if( SERVICE_EXIST(m_serviceProvider, SoundServiceInterface) == true )
			{
				SOUND_SERVICE(m_serviceProvider)->onTurnSound( false );
			}
		}
		else
		{
			if( SERVICE_EXIST(m_serviceProvider, SoundServiceInterface) == true )
			{
				SOUND_SERVICE(m_serviceProvider)->onTurnSound( true );
			}

			if( m_game != nullptr )
			{
				m_game->turnSound( true );
			}

			if( SERVICE_EXIST(m_serviceProvider, SoundServiceInterface) == true )
			{
				SOUND_SERVICE(m_serviceProvider)->onTurnStream( true );
			}
		}

		if( SERVICE_EXIST(m_serviceProvider, SoundServiceInterface) == true )
		{
			SOUND_SERVICE(m_serviceProvider)->updateVolume();
		}
    }
	//////////////////////////////////////////////////////////////////////////
	void Application::finalize()
	{		
		if( m_game != nullptr )
		{			
			m_game->finalizeRenderResources();
		}

		CODEC_SERVICE(m_serviceProvider)
			->unregisterDecoder( CONST_STRING(m_serviceProvider, memoryImage) );

		CODEC_SERVICE(m_serviceProvider)
			->unregisterDecoder( CONST_STRING(m_serviceProvider, archiveImage) );

        if( m_nodeService != nullptr )
        {
            m_nodeService->finalize();
        }
        
		if( SCRIPT_SERVICE(m_serviceProvider) )
		{
            ScriptWrapper::constsUnwrap( m_serviceProvider );            			
		}

        SERVICE_DESTROY( GameService, m_game );

		if( m_resourceService != nullptr )
		{
			m_resourceService->finalize();

			SERVICE_DESTROY( ResourceService, m_resourceService );
		}

        SERVICE_DESTROY( Watchdog, m_watchdog );
        SERVICE_DESTROY( LoaderService, m_loaderService );
        
		if( m_textService != nullptr )
		{
			m_textService->finalize();

	        SERVICE_DESTROY( TextService, m_textService );
		}

        SERVICE_DESTROY( NodeService, m_nodeService );
        
		SERVICE_DESTROY( PrototypeService, m_prototypeService );
        SERVICE_DESTROY( Consts, m_consts );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::calcWindowResolution( Resolution & _windowResolution ) const
	{
		Resolution dres;
        PLATFORM_SERVICE(m_serviceProvider)
			->getMaxClientResolution( dres );

		float aspect = m_windowResolution.getAspectRatio();

		uint32_t resHeight = m_windowResolution.getHeight();
        uint32_t resWidth = m_windowResolution.getWidth();

		uint32_t dresHeight = dres.getHeight();
        uint32_t dresWidth = dres.getWidth();

		if( resHeight > dresHeight )
		{
			uint32_t new_witdh = static_cast<uint32_t>( float(resHeight) * aspect + 0.5f );
            uint32_t new_height = dresHeight;

            if( new_witdh > dresWidth )
            {
                new_witdh = dresWidth;
                new_height = static_cast<uint32_t>( float(dresWidth) / aspect + 0.5f );
            }

			_windowResolution.setWidth( new_witdh );			
			_windowResolution.setHeight( new_height );
		}
		else if( resWidth > dresWidth )
        {
            uint32_t new_witdh = dresWidth;
            uint32_t new_height = static_cast<uint32_t>( float(resWidth) / aspect + 0.5f );

            if( new_height > dresHeight )
            {
                new_witdh = static_cast<uint32_t>( float(dresHeight) * aspect + 0.5f );
                new_height = dresHeight;
            }

            _windowResolution.setWidth( new_witdh );
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

        bool fullscreen = this->getFullscreenMode();

		if( fullscreen == false )	// don't override fullscreen behavior
		{
			if( m_mouseBounded == true )
			{
				PLATFORM_SERVICE(m_serviceProvider)
					->notifyCursorClipping( m_renderViewport );
			}
			else
			{
				PLATFORM_SERVICE(m_serviceProvider)
					->notifyCursorUnClipping();
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::getMouseBounded() const
	{
		return m_mouseBounded;
	}
    //////////////////////////////////////////////////////////////////////////
    bool Application::findBestAspectViewport_( float _aspect, float & _bestAspect, Viewport & _viewport ) const
    {
        LOGGER_INFO(m_serviceProvider)("Application::findBestAspectViewport_ for aspect %f"
            , _aspect
            );

        if( m_aspectRatioViewports.empty() == true )
        {
            LOGGER_INFO(m_serviceProvider)("Application::findBestAspectViewport_ empty"
                );

            return false;
        }

        bool found_aspect = false;

        for( TMapAspectRatioViewports::const_iterator
            it = m_aspectRatioViewports.begin(),
            it_found = m_aspectRatioViewports.end();
        it != it_found;
        ++it )
        {
            float aspect = it->first;

            if( aspect < _aspect || fabsf( aspect - _aspect ) < 0.00001f )
            {
                _bestAspect = it->first;
                _viewport = it->second;

                found_aspect = true;
            }
        }

        if( found_aspect == false )
        {
            TMapAspectRatioViewports::const_iterator
                it_first = m_aspectRatioViewports.begin();

            _bestAspect = it_first->first;
            _viewport = it_first->second;
        }

        LOGGER_INFO(m_serviceProvider)("Application::findBestAspectViewport_ best aspect %f viewport [%f, %f, %f, %f]"
            , _bestAspect
            , _viewport.begin.x
            , _viewport.begin.y
            , _viewport.end.x
            , _viewport.end.y
            );

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void Application::calcRenderViewport_( const Resolution & _resolution, Viewport & _viewport )
	{
        LOGGER_INFO(m_serviceProvider)("Application::calcRenderViewport resolution [%d %d]"
            , _resolution.getWidth()
            , _resolution.getHeight()
            );

        float rw = (float)_resolution.getWidth();
		float rh = (float)_resolution.getHeight();

        float r_aspect = _resolution.getAspectRatio();
        
		if( m_fixedDisplayResolution == true )
		{           
            float c_aspect = m_contentResolution.getAspectRatio();

            Viewport dummy_aspectRatioViewport;
            float contentAspect;

            if( this->findBestAspectViewport_( r_aspect, contentAspect, dummy_aspectRatioViewport ) == false )
            {
                contentAspect = c_aspect;
            }

            float one_div_width = 1.f / rw;
            float one_div_height = 1.f / rh;

            float dw = 1.f;
            float dh = rw / contentAspect * one_div_height;

            if( dh > 1.f )
            {
                dh = 1.f;
                dw = rh * contentAspect * one_div_width;
            }

            float areaWidth = ceilf(dw * rw);
            float areaHeight = ceilf(dh * rh);

            LOGGER_INFO(m_serviceProvider)("area [%d %d]"
                , areaWidth
                , areaHeight
                );

            _viewport.begin.x = ceilf(( rw - areaWidth ) * 0.5f);
            _viewport.begin.y = ceilf(( rh - areaHeight ) * 0.5f);
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
    bool Application::isValidWindowMode() const
    {
        if( m_windowModeCheck == false )
        {
            return true;
        }

        Resolution windowResolution;
        this->calcWindowResolution( windowResolution );

        LOGGER_INFO(m_serviceProvider)("Application::isValidWindowMode resolution %d:%d\n"
            , windowResolution.getWidth()
            , windowResolution.getHeight()
            );

        float aspect = windowResolution.getAspectRatio();

        Viewport aspectRatioViewport;
        float bestAspect;

        if( this->findBestAspectViewport_( aspect, bestAspect, aspectRatioViewport ) == true )
        {
            LOGGER_INFO(m_serviceProvider)("Application::isValidWindowMode viewport (1) %f:%f\n"
                , aspectRatioViewport.getWidth()
                , aspectRatioViewport.getHeight()
                );

            if( windowResolution.getWidth() < aspectRatioViewport.getWidth() ||
                windowResolution.getHeight() < aspectRatioViewport.getHeight() )
            {
                return false;
            }
        }
        else
        {
            LOGGER_INFO(m_serviceProvider)("Application::isValidWindowMode viewport (2) %d:%d\n"
                , m_contentResolution.getWidth()
                , m_contentResolution.getHeight()
                );

            if( windowResolution.getWidth() < m_contentResolution.getWidth() ||
                windowResolution.getHeight() < m_contentResolution.getHeight() )
            {
                return false;
            }
        }

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void Application::changeWindowResolution( const Resolution & _resolution )
	{
		m_windowResolution = _resolution;

		this->invalidateWindow_();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setFullscreenMode( bool _fullscreen )
	{
		if( m_fullscreen == _fullscreen )
		{
			return;
		}

		m_fullscreen = _fullscreen;

        this->invalidateWindow_();

        bool fullscreen = this->getFullscreenMode();        
        m_game->setFullscreen( m_currentResolution, fullscreen );
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::invalidateWindow_()
    {
        bool fullscreen = this->getFullscreenMode();

		if( fullscreen == true )
		{
            Resolution desktopResolution;
            PLATFORM_SERVICE(m_serviceProvider)
				->getDesktopResolution( desktopResolution );

            m_currentResolution = desktopResolution;
		}
		else
		{
			this->calcWindowResolution( m_currentResolution );
		}

        LOGGER_WARNING(m_serviceProvider)( "Application::invalidateWindow_ %d Current Resolution %d %d"
            , fullscreen
            , m_currentResolution.getWidth()
            , m_currentResolution.getHeight()
            );
        
		RENDER_SERVICE(m_serviceProvider)
			->setVSync( m_vsync );

		PLATFORM_SERVICE(m_serviceProvider)
			->notifyWindowModeChanged( m_currentResolution, fullscreen );

		this->calcRenderViewport_( m_currentResolution, m_renderViewport );
		//m_renderEngine->applyRenderViewport( renderViewport );

		LOGGER_WARNING(m_serviceProvider)("Application::invalidateWindow_ Render Viewport %f %f - %f %f"
			, m_renderViewport.begin.x
			, m_renderViewport.begin.y
			, m_renderViewport.getWidth()
			, m_renderViewport.getHeight()
			);

		if( fullscreen == true )
		{
			PLATFORM_SERVICE(m_serviceProvider)
				->notifyCursorClipping( m_renderViewport );
		}
		else
		{
			//m_interface->notifyCursorUnClipping();
			this->setMouseBounded( m_mouseBounded );
		}
	
		RENDER_SERVICE(m_serviceProvider)
            ->changeWindowMode( m_currentResolution, m_contentResolution, m_renderViewport, fullscreen );
		
		if( !m_mouseBounded && RENDER_SERVICE(m_serviceProvider)->isWindowCreated() )
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

		NOTIFICATION_SERVICE(m_serviceProvider)
			->notify( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION, fullscreen, m_currentResolution );

        if( m_game != nullptr )
        {
            m_game->setRenderViewport( m_renderViewport, m_contentResolution );

			float gameViewportAspect;
			Viewport gameViewport;

			APPLICATION_SERVICE(m_serviceProvider)
				->getGameViewport( gameViewportAspect, gameViewport );

			m_game->setGameViewport( gameViewport, gameViewportAspect );

			LOGGER_WARNING(m_serviceProvider)("Application::invalidateWindow_ Game Viewport %f %f - %f %f Aspect %f"
				, gameViewport.begin.x
				, gameViewport.begin.y
				, gameViewport.end.x
				, gameViewport.end.y
				, gameViewportAspect
				);
        }
	}
	////////////////////////////////////////////////////////////////////////////
	//void Application::screenshot( const RenderTextureInterfacePtr & _renderTargetImage, const mt::vec4f & _rect )
	//{
	//	mt::vec4f res; 

	//	res = _rect;

	//	res.x *= static_cast<float>( m_currentResolution.getWidth() ) / m_contentResolution.getWidth();
	//	res.y *= static_cast<float>( m_currentResolution.getHeight() ) / m_contentResolution.getHeight();
	//	res.z *= static_cast<float>( m_currentResolution.getWidth() ) / m_contentResolution.getWidth();
	//	res.w *= static_cast<float>( m_currentResolution.getHeight() ) / m_contentResolution.getHeight();
	//	
	//	RENDER_SERVICE(m_serviceProvider)
 //           ->screenshot( _renderTargetImage, _rect );
	//}
	//////////////////////////////////////////////////////////////////////////
	bool Application::getFullscreenMode() const
	{
        if( m_nofullscreenMode == true )
        {
            return false;
        }

		return m_fullscreen;
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
	void Application::paint()
	{
		if( m_focus == false && RENDER_SERVICE(m_serviceProvider) && m_game && m_createRenderWindow == true )
		{
			if( RENDER_SERVICE(m_serviceProvider)->beginScene() == true )
			{
				m_game->render();

				RENDER_SERVICE(m_serviceProvider)->endScene();
				RENDER_SERVICE(m_serviceProvider)->swapBuffers();
			}
		}
	}
    //////////////////////////////////////////////////////////////////////////
    bool Application::userEvent( const ConstString & _event, const TMapParams & _params )
    {
        if( m_game == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("Application::onUserEvent %s game not create"
                , _event.c_str()
                );

            return false;
        }

        m_game->userEvent( _event, _params );

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	const WString & Application::getCompanyName() const
	{
		return m_companyName;
	}
	//////////////////////////////////////////////////////////////////////////
	const WString & Application::getProjectName() const
	{
		return m_projectName;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Application::getProjectTitle() const
	{
		if( TEXT_SERVICE(m_serviceProvider) == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Application::getProjectTitle not initialize textManager"
				);

			return Utils::emptyConstString();
		}

        const TextEntryInterface * entry;
		if( TEXT_SERVICE(m_serviceProvider)->existText( CONST_STRING_LOCAL(m_serviceProvider, "APPLICATION_TITLE"), &entry ) == false )
		{
            return Utils::emptyConstString();
		}

		const ConstString & text = entry->getText();

		return text;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Application::getProjectCodename() const
	{
		return m_projectCodename;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t Application::getProjectVersion() const
	{
		return m_projectVersion;
	}
	//////////////////////////////////////////////////////////////////////////
	const Resolution & Application::getContentResolution() const
	{
		return m_contentResolution;
	}
    //////////////////////////////////////////////////////////////////////////
    void Application::setFixedContentResolution( bool _fixedContetResolution )
    {
        if( m_fixedContentResolution == _fixedContetResolution )
        {
            return;
        }

        m_fixedContentResolution = _fixedContetResolution;

        this->invalidateWindow_();

        m_game->setFixedContentResolution( m_currentResolution, m_fixedContentResolution );
    }
	//////////////////////////////////////////////////////////////////////////
	bool Application::getFixedContentResolution() const
	{
		return m_fixedContentResolution;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::getGameViewport( float & _aspect, Viewport & _viewport ) const
	{
        float aspect = m_currentResolution.getAspectRatio();

        if( m_fixedContentResolution == false )
        {
            _aspect = aspect;
            
            _viewport.begin.x = 0.f;
            _viewport.begin.y = 0.f;
            _viewport.end.x = (float)m_contentResolution.getWidth();
            _viewport.end.y = (float)m_contentResolution.getHeight();

            return;
        }
        
        Viewport aspectRatioViewport;
        float bestAspect;

        if( this->findBestAspectViewport_( aspect, bestAspect, aspectRatioViewport ) == true )
        {
            _aspect = bestAspect;
            _viewport = aspectRatioViewport;
        }
        else
        {
            _aspect = aspect;

            _viewport.begin.x = 0.f;
            _viewport.begin.y = 0.f;
            _viewport.end.x = (float)m_contentResolution.getWidth();
            _viewport.end.y = (float)m_contentResolution.getHeight();
        }
	}
	//////////////////////////////////////////////////////////////////////////
	const Resolution & Application::getWindowResolution() const
	{
		return m_windowResolution;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::getVSync() const
	{
		if( RENDER_SERVICE(m_serviceProvider) )
		{
			return RENDER_SERVICE(m_serviceProvider)->getVSync();
		}

		return false;
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
			if( RENDER_SERVICE(m_serviceProvider) != nullptr )
			{
				RENDER_SERVICE(m_serviceProvider)->setVSync( m_vsync );
			}

			PLATFORM_SERVICE(m_serviceProvider)
				->notifyVsyncChanged( m_vsync );

			m_invalidateVsync = false;
		}

		if( m_invalidateCursorMode == true )
		{
			PLATFORM_SERVICE(m_serviceProvider)
				->notifyCursorModeChanged( m_cursorMode );

			m_invalidateCursorMode = false;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setCursorMode( bool _mode )
	{
		m_cursorMode = _mode;

		if( m_game != nullptr )
		{
			m_game->setCursorMode( m_cursorMode );
		}

		if( m_cursorMode == true && m_cursorResource != nullptr )
		{
			const ConstString & cursorName = m_cursorResource->getName();

			size_t cursorBufferSize;
			void * cursorBufferPtr = m_cursorResource->getBuffer( cursorBufferSize );

			PLATFORM_SERVICE(m_serviceProvider)
				->notifyCursorIconSetup( cursorName, cursorBufferPtr, cursorBufferSize );
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
		if( m_cursorResource != nullptr )
		{
			m_cursorResource->decrementReference();
		}

		m_cursorResource = RESOURCE_SERVICE(m_serviceProvider)
			->getResourceT<ResourceCursor>(_resourceName);

		if( m_cursorResource == nullptr )
		{
            LOGGER_ERROR(m_serviceProvider)("Application::setCursorIcon: can't find resource cursor %s"
                , _resourceName.c_str()
                );

			return;
		}

		if( m_cursorMode == false )
		{
			return;
		}

		const ConstString & cursorName = m_cursorResource->getName();

		size_t cursorBufferSize;
		void * cursorBufferPtr = m_cursorResource->getBuffer( cursorBufferSize );

		PLATFORM_SERVICE(m_serviceProvider)
			->notifyCursorIconSetup( cursorName, cursorBufferPtr, cursorBufferSize );
	}
	////////////////////////////////////////////////////////////////////////////
	//void Application::setAsScreensaver( bool _set )
	//{
	//	//m_platform->setAsScreensaver( _set );
	//}	
	//////////////////////////////////////////////////////////////////////////
	void Application::showKeyboard()
	{
		PLATFORM_SERVICE(m_serviceProvider)
			->showKeyboard();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::hideKeyboard()
	{
		PLATFORM_SERVICE(m_serviceProvider)
			->hideKeyboard();
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::debugPause( bool _pause )
	{
		m_debugPause = _pause;
	}
}

