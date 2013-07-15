#	include "Application.h"

#   include "Interface/InputSystemInterface.h"
#   include "Interface/RenderSystemInterface.h"
#	include "Interface/SoundSystemInterface.h"

#   include "Interface/ParticleSystemInterface.h"
#   include "Interface/ScriptSystemInterface.h"
#   include "Interface/EventInterface.h"
#   include "Interface/PhysicSystem2DInterface.h"

#   include "Interface/StringizeInterface.h"

#	include "Interface/NotificationServiceInterace.h"
#	include "Interface/NotificatorInterface.h"

#   include "Interface/MousePickerSystemInterface.h"

//#	include "LightSystem.h"
#   include "Consts.h"

#	include "Game.h"

#	include "Logger/Logger.h"

#	include "Interface/LoaderInterface.h"
#	include "Interface/NodeInterface.h"
#	include "Interface/TextInterface.h"
#	include "Interface/ThreadSystemInterface.h"
#	include "Interface/CodecInterface.h"

#   include "Interface/ConverterInterface.h"

#   include "Interface/ResourceInterface.h"
#   include "Interface/AlphaChannelInterface.h"
#   include "Interface/AccountInterface.h"
#   include "Interface/ProfilerInterface.h"

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
#	include "HotSpot.h"
//#	include "Light2D.h"
#	include "ShadowCaster2D.h"
#	include "TilePolygon.h"
#	include "Point.h"
#	include "SoundEmitter.h"
#	include "Sprite.h"
#	include "TextField.h"
#	include "TileMap.h"
#	include "Track.h"
#	include "Movie.h"
#	include "MovieSlot.h"
#	include "MovieSceneEffect.h"
#	include "MovieInternalObject.h"
#   include "MovieEvent.h"
#	include "Model.h"
#	include "Video.h"
#	include "Layer2D.h"
#	include "Layer2DLoop.h"
#	include "Camera2D.h"
#	include "Camera3D.h"
#	include "Layer2DAccumulator.h"
#	include "Layer3D.h"
#	include "Window.h"
#	include "HotSpotImage.h"
#	include "HotSpotShape.h"
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
#	include "ResourceImageSubstract.h"
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
#   include "ResourceHIT.h"
#   include "ResourceShape.h"
#	include "ResourceCursorICO.h"
#	include "ResourceCursorSystem.h"
#	include "ResourceInternalObject.h"

#	include "Interface/UnicodeInterface.h"

#	include "ScriptWrapper.h"

#	include <ctime>
#	include <sstream>

#	include "VersionInfo.h"

#	include "Config/Config.h"

#   include "Core/String.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN(Consts, Menge::Consts);
SERVICE_EXTERN(NodeService, Menge::NodeServiceInterface);
SERVICE_EXTERN(LoaderService, Menge::LoaderServiceInterface);
SERVICE_EXTERN(MovieKeyFrameService, Menge::MovieKeyFrameServiceInterface);
SERVICE_EXTERN(ResourceService, Menge::ResourceServiceInterface);
SERVICE_EXTERN(AlphaChannelService, Menge::AlphaChannelServiceInterface);
SERVICE_EXTERN(TextService, Menge::TextServiceInterface);
SERVICE_EXTERN(Watchdog, Menge::WatchdogInterface);
SERVICE_EXTERN(GameService, Menge::GameServiceInterface);
SERVICE_EXTERN(PrototypeService, Menge::PrototypeServiceInterface);
SERVICE_EXTERN(AmplifierService, Menge::AmplifierServiceInterface);
//SERVICE_EXTERN(ProfilerService, Menge::ProfilerServiceInterface);
//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( Application, Menge::ApplicationInterface, Menge::Application );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Application::Application()
		: m_platform(nullptr)
		, m_particles(true)
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
        , m_amplifierService(nullptr)
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
		, m_cursorResource(nullptr)
		, m_fixedContentResolution(false)
		, m_vsync(false)
		, m_fullScreen(true)
		, m_bits(0)
		, m_FSAAType(0)
		, m_FSAAQuality(0)
		, m_textureFiltering(true)
        , m_windowModeCheck(false)
        , m_watchdog(nullptr)
        , m_profiler(nullptr)
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

        m_platform = PLATFORM_SERVICE(m_serviceProvider);
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
	bool Application::initialize()
	{   
        SERVICE_CREATE( Consts, &m_consts );
        SERVICE_REGISTRY( m_serviceProvider, m_consts );

        m_consts->initialize();

        ExecuteInitialize exinit( this );

        exinit.add( &Application::initializePrototypeManager_ );
        exinit.add( &Application::initializeNodeManager_ );        
        exinit.add( &Application::initializeAmplifierService_ );
        exinit.add( &Application::initializeLoaderEngine_ );
        exinit.add( &Application::initializeMovieKeyFrameManager_ );
        exinit.add( &Application::initializeResourceManager_ );
        exinit.add( &Application::initializeSceneManager_ );
        exinit.add( &Application::initializeTextManager_ );        
        exinit.add( &Application::initializeWatchdog_ );
        exinit.add( &Application::initializeProfiler_ );

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
		
		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    bool Application::setup( const String& _args, const ApplicationSettings & _setting )
    {
        m_platformName = _setting.platformName;
        m_projectCodename = _setting.projectCodename;

        m_contentResolution = _setting.contentResolution;
        m_fixedContentResolution = _setting.fixedContentResolution;

        for( TVectorAspectRatioViewports::const_iterator
            it = _setting.aspectRatioViewports.begin(),
            it_end = _setting.aspectRatioViewports.end();
        it != it_end;
        ++it )
        {
           float aspect = it->width / it->height;

           m_aspectRatioViewports[aspect] = it->viewport;
        }

        m_windowResolution = _setting.windowResolution;

        m_bits = _setting.bits;
        m_fullscreen = _setting.fullscreen;
        m_vsync = _setting.vsync;

        m_windowModeCheck = _setting.windowModeCheck;

        this->parseArguments_( _args );
        this->setBaseDir( _setting.baseDir );

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	PlatformInterface * Application::getPlatform() const
	{
		return m_platform;
	}
    //////////////////////////////////////////////////////////////////////////
    namespace
    {
        template<class Type, size_t Count>
        class NodePrototypeGenerator
            : public PrototypeGeneratorInterface
        {
        public:
            NodePrototypeGenerator( ServiceProviderInterface * _serviceProvider )
                : m_serviceProvider(_serviceProvider)
            {
            }

        protected:
            Factorable * generate( const ConstString & _category, const ConstString & _prototype ) override
            {
                (void)_category;
                (void)_prototype;

                Node * node = m_factory.createObjectT();

                if( node == nullptr )
                {
                    LOGGER_ERROR(m_serviceProvider)( "NodePrototypeGenerator can't generate %s %s"
                        , _category.c_str()
                        , _prototype.c_str()
                        );

                    return nullptr;
                }

                node->setType( _prototype );

                node->setServiceProvider( m_serviceProvider );

                return node;
            }

            size_t count() const override
            {
                size_t count = m_factory.countObject();

                return count;
            }

            void destroy() override
            {
                delete this;
            }

        protected:
            ServiceProviderInterface * m_serviceProvider;

            typedef FactoryPool<Type, Count> TNodeFactory;
            TNodeFactory m_factory;
        };
    }
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeNodeManager_()
	{	
		NodeServiceInterface * nodeService;
        if( createNodeService( &nodeService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, nodeService ) == false )
        {
            return false;
        }

        m_nodeService = nodeService;

        m_nodeService->initialize();

//#	define NODE_FACTORY( serviceProvider, Type )\
//		nodeService->registerFactory( Helper::stringizeString( serviceProvider, #Type), new FactoryPool<Type, 128> )

#	define NODE_FACTORY( serviceProvider, Type )\
        PROTOTYPE_SERVICE(serviceProvider)\
            ->addPrototype( CONST_STRING(serviceProvider, Node), Helper::stringizeString( serviceProvider, #Type), new NodePrototypeGenerator<Type, 128>(serviceProvider) );

		LOGGER_INFO(m_serviceProvider)( "Creating Object Factory..." );

		NODE_FACTORY( m_serviceProvider, Node );

		NODE_FACTORY( m_serviceProvider, Entity );
		NODE_FACTORY( m_serviceProvider, Arrow );
		NODE_FACTORY( m_serviceProvider, Scene );

		NODE_FACTORY( m_serviceProvider, Animation );
		
#   ifdef MENGE_PARTICLES
		NODE_FACTORY( m_serviceProvider, ParticleEmitter );
#   endif

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
		NODE_FACTORY( m_serviceProvider, TextField );
		//NODE_FACTORY( TileMap );
		//NODE_FACTORY( Track );
		NODE_FACTORY( m_serviceProvider, Movie );
        NODE_FACTORY( m_serviceProvider, MovieSlot );
        NODE_FACTORY( m_serviceProvider, MovieSceneEffect );
		NODE_FACTORY( m_serviceProvider, MovieInternalObject );
        NODE_FACTORY( m_serviceProvider, MovieEvent );
		//NODE_FACTORY( Model );
		NODE_FACTORY( m_serviceProvider, Video );
		NODE_FACTORY( m_serviceProvider, Layer2D );
		//NODE_FACTORY( m_serviceProvider, Layer2DPhysic );
		//NODE_FACTORY( Layer2DLoop );
		//NODE_FACTORY( Layer2DAccumulator );
		//NODE_FACTORY( Layer2DTexture );
		//NODE_FACTORY( LayerScene );
		//NODE_FACTORY( RenderMesh );
		NODE_FACTORY( m_serviceProvider, Camera2D );
		NODE_FACTORY( m_serviceProvider, Camera3D );
		//NODE_FACTORY( SceneNode3D );
		NODE_FACTORY( m_serviceProvider, Window );
		
#	undef NODE_FACTORY

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeLoaderEngine_()
	{
		LOGGER_INFO(m_serviceProvider)( "Initializing Loader Engine..." );

		LoaderServiceInterface * loaderService;
        if( createLoaderService( &loaderService ) == false )
        {
            return false;
        }

        SERVICE_REGISTRY( m_serviceProvider, loaderService );

        m_loaderService = loaderService;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeMovieKeyFrameManager_()
	{
		LOGGER_INFO(m_serviceProvider)( "Inititalizing MovieKeyFrameManager..." );
		
		MovieKeyFrameServiceInterface * movieKeyFrameService;

        if( SERVICE_CREATE( MovieKeyFrameService, &movieKeyFrameService ) == false )
        {
            return false;
        }
		
        SERVICE_REGISTRY( m_serviceProvider, movieKeyFrameService );

        m_movieKeyFrameService = movieKeyFrameService;

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    bool Application::initializePrototypeManager_()
    {
        LOGGER_INFO(m_serviceProvider)( "Inititalizing PrototypeManager..." );

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
        LOGGER_INFO(m_serviceProvider)( "Inititalizing Watchdog..." );

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
        //LOGGER_INFO(m_serviceProvider)( "Inititalizing ProfilerService..." );

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
                    LOGGER_ERROR(m_serviceProvider)( "SceneCategoryGenerator can't create %s %s"
                        , _category.c_str()
                        , _prototype.c_str()
                        );

                    return nullptr;
                }

                scene->setName( _prototype );

                return scene;
            }

            size_t count() const override
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
    bool Application::initializeAmplifierService_()
    {
        LOGGER_INFO(m_serviceProvider)( "Initializing Amplifier Service..." );

        AmplifierServiceInterface * amplifierService;
        if( createAmplifierService( &amplifierService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, amplifierService ) == false )
        {
            return false;
        }

        m_amplifierService = amplifierService;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    namespace
    {
        template<class Type, size_t Count>
        class ResourcePrototypeGenerator
            : public PrototypeGeneratorInterface
        {
        public:
            ResourcePrototypeGenerator( ServiceProviderInterface * _serviceProvider )
                : m_serviceProvider(_serviceProvider)
            {
            }

        protected:
            Factorable * generate( const ConstString & _category, const ConstString & _prototype ) override
            {
                (void)_category;
                (void)_prototype;

                ResourceReference * resource = m_factory.createObjectT();

                if( resource == nullptr )
                {
                    LOGGER_ERROR(m_serviceProvider)( "ResourcePrototypeGenerator can't generate %s %s"
                        , _category.c_str()
                        , _prototype.c_str()
                        );

                    return nullptr;
                }

                resource->setType( _prototype );

                resource->setServiceProvider( m_serviceProvider );

                return resource;
            }

            size_t count() const override
            {
                size_t count = m_factory.countObject();

                return count;
            }

            void destroy() override
            {
                delete this;
            }

        protected:
            ServiceProviderInterface * m_serviceProvider;

            typedef FactoryPool<Type, Count> TNodeFactory;
            TNodeFactory m_factory;
        };
    }    
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeResourceManager_()
	{
        LOGGER_INFO(m_serviceProvider)( "Initializing Resource Manager..." );

        ResourceServiceInterface * resourceService;
        if( createResourceService( &resourceService ) == false )
        {
            return false;
        }

        SERVICE_REGISTRY( m_serviceProvider, resourceService );
        
        m_resourceService = resourceService;


#	define RESOURCE_FACTORY( serviceProvider, Type ) \
    PROTOTYPE_SERVICE(serviceProvider)->addPrototype( CONST_STRING(serviceProvider, Resource), Helper::stringizeString(serviceProvider, #Type), new ResourcePrototypeGenerator<Type, 128>(m_serviceProvider) )

		RESOURCE_FACTORY( m_serviceProvider, ResourceAnimation );

#   ifdef MENGE_PARTICLES
		RESOURCE_FACTORY( m_serviceProvider, ResourceEmitterContainer );
#   endif

		RESOURCE_FACTORY( m_serviceProvider, ResourceFont );
		
		RESOURCE_FACTORY( m_serviceProvider, ResourceImageDefault );
        RESOURCE_FACTORY( m_serviceProvider, ResourceImageSubstract );
		RESOURCE_FACTORY( m_serviceProvider, ResourceImageCombineRGBAndAlpha );
        RESOURCE_FACTORY( m_serviceProvider, ResourceImageMultiplyRGBAndAlpha );
		RESOURCE_FACTORY( m_serviceProvider, ResourceImageSolid );
		RESOURCE_FACTORY( m_serviceProvider, ResourceMovie );
		RESOURCE_FACTORY( m_serviceProvider, ResourceVideo );
		RESOURCE_FACTORY( m_serviceProvider, ResourcePlaylist );
		RESOURCE_FACTORY( m_serviceProvider, ResourceSound );
		RESOURCE_FACTORY( m_serviceProvider, ResourceGlyph );

		RESOURCE_FACTORY( m_serviceProvider, ResourceWindow );
        RESOURCE_FACTORY( m_serviceProvider, ResourceHIT );
        RESOURCE_FACTORY( m_serviceProvider, ResourceShape );
		RESOURCE_FACTORY( m_serviceProvider, ResourceCursorICO );
		RESOURCE_FACTORY( m_serviceProvider, ResourceCursorSystem );

		RESOURCE_FACTORY( m_serviceProvider, ResourceInternalObject );

#	undef RESOURCE_FACTORY

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeTextManager_()
	{
		LOGGER_INFO(m_serviceProvider)( "initialize Text Manager..." );

        TextServiceInterface * textService;
        if( createTextService( &textService ) == false )
        {
            return false;
        }

        m_textService = textService;

        if( SERVICE_REGISTRY( m_serviceProvider, m_textService ) == false )
        {
            return false;
        }
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setBaseDir( const FilePath & _dir )
	{
		m_baseDir = _dir;

		LOGGER_WARNING(m_serviceProvider)( "setBaseDir '%s'"
			, m_baseDir.c_str()
			);
	}
	//////////////////////////////////////////////////////////////////////////
	const FilePath & Application::getBaseDir() const
	{
		return m_baseDir;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::createGame( const WString & _module, const ConstString & _language, const TVectorResourcePackDesc & _resourcePacks, const TVectorResourcePackDesc & _languagePacks )
	{
        SERVICE_CREATE(GameService, &m_game);

        if( SERVICE_REGISTRY( m_serviceProvider, m_game ) == false )
        {
            LOGGER_INFO(m_serviceProvider)( "Application:createGame invalid register game service"
                );

            return false;
        }

		m_game->setBaseDir( m_baseDir );
        m_game->setDevelopmentMode( m_developmentMode );
        m_game->setPlatformName( m_platformName );
        
		LOGGER_INFO(m_serviceProvider)( "Application:createGame load game resource"
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

        if( m_developmentMode == true && m_resourceCheck == true )
        {
            RESOURCE_SERVICE(m_serviceProvider)
                ->validationResources();
        }

        String personalityModule;                
        if( Helper::unicodeToUtf8( m_serviceProvider, _module, personalityModule ) == false )
        {
            return false;
        }
        
		if( m_game->loadPersonality( Helper::stringizeString(m_serviceProvider, personalityModule) ) == false )
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
            m_platform->getDesktopResolution( desktopResolution );

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

		m_createRenderWindow = RENDER_SERVICE(m_serviceProvider)->createRenderWindow( m_currentResolution, m_contentResolution, m_renderViewport, m_bits, fullscreen,
														_renderWindowHandle, m_FSAAType, m_FSAAQuality );

		if( m_createRenderWindow == false )
		{
            LOGGER_ERROR(m_serviceProvider)( "Application::createRenderWindow failed to create render window" 
                            );            

			return false;
		}
		
		if( fullscreen == true )
		{
			m_platform->notifyCursorClipping( m_renderViewport );
		}
		else
		{
			m_platform->notifyCursorUnClipping();
		}

		//m_renderEngine->setRenderViewport( renderViewport );

		RENDER_SERVICE(m_serviceProvider)
            ->enableTextureFiltering( m_textureFiltering );

        m_game->initializeRenderResources();

		NOTIFICATION_SERVICE(m_serviceProvider)
            ->notify( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION, fullscreen, m_currentResolution );
			
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeGame( const TMapParams & _params, const String & _scriptInitParams )
	{
		if( m_game->initialize( _params ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Application::initGame invalid initialize"
				);

			return false;
		}

        if( m_game->run( _scriptInitParams ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Application::runGame invalid run %s"
                , _scriptInitParams.c_str()
                );

            return false;
        }

        m_game->setCursorMode( m_cursorMode );
        
		return true;
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

        String::size_type idx_nofullscreen = _arguments.find( " -nofullscreen " );
        if( idx_nofullscreen != String::npos )
        {
            m_nofullscreenMode = true;
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
				m_resourceService->dumpResources("Application");
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
                PlayerServiceInterface * player = m_game->getPlayer();
				
                player->toggleDebugText();
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
		if( INPUT_SERVICE(m_serviceProvider)->validCursorPosition( _point ) == false )
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
		if( INPUT_SERVICE(m_serviceProvider)
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
		
		RENDER_SERVICE(m_serviceProvider)->onWindowClose();
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
	void Application::onFocus( bool _focus, const mt::vec2f & _point )
	{
		//return;
        LOGGER_INFO(m_serviceProvider)("Application::onFocus from %d to %d"
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
		if( THREAD_SERVICE(m_serviceProvider) )
		{
			THREAD_SERVICE(m_serviceProvider)->update();
		}

		if( !m_update && !m_focus ) 
		{						
			return false;
		}

		INPUT_SERVICE(m_serviceProvider)->update();

		if( m_game->update() == false )
		{
			this->quit();

			return false;
		}

        //EVENT_SERVICE(m_serviceProvider)->update();

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

		if( SOUND_SERVICE(m_serviceProvider) )
		{
			SOUND_SERVICE(m_serviceProvider)->update( timing );
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
		if( RENDER_SERVICE(m_serviceProvider)->beginScene() == false )
		{
			return false;
		}

		bool immediatelyFlush = m_game->render();

		if( m_console != nullptr )
		{
			m_console->render();
		}
		//Holder<Console>::get()->render();

		RENDER_SERVICE(m_serviceProvider)->endScene();

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
		RENDER_SERVICE(m_serviceProvider)
            ->swapBuffers();
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
			if( SOUND_SERVICE(m_serviceProvider) )
			{
				SOUND_SERVICE(m_serviceProvider)->onTurnStream( _turn );
			}

			if( m_game )
			{
				m_game->onTurnSound( _turn );
			}

			if( SOUND_SERVICE(m_serviceProvider) )
			{
				SOUND_SERVICE(m_serviceProvider)->onTurnSound( _turn );
			}
		}
		else
		{
			if( SOUND_SERVICE(m_serviceProvider) )
			{
				SOUND_SERVICE(m_serviceProvider)->onTurnSound( _turn );
			}

			if( m_game )
			{
				m_game->onTurnSound( _turn );
			}

			if( SOUND_SERVICE(m_serviceProvider) )
			{
				SOUND_SERVICE(m_serviceProvider)->onTurnStream( _turn );
			}
		}
    }
	//////////////////////////////////////////////////////////////////////////
	void Application::finalize()
	{		
		if( m_game != nullptr )
		{
			m_game->finalizeRenderResources();
			m_game->finalize();
		}

        if( m_nodeService != nullptr )
        {
            m_nodeService->finalize();
        }
        
		if( SCRIPT_SERVICE(m_serviceProvider) )
		{
            ScriptWrapper::constsUnwrap( m_serviceProvider );            			
		}

        SERVICE_DESTROY( GameService, m_game );

        //delete m_paramManager;
        //destroyArrowService

        SERVICE_DESTROY( ResourceService, m_resourceService );

        SERVICE_DESTROY( Watchdog, m_watchdog );
        SERVICE_DESTROY( LoaderService, m_loaderService );
        SERVICE_DESTROY( AmplifierService, m_amplifierService );
        SERVICE_DESTROY( TextService, m_textService );
        SERVICE_DESTROY( NodeService, m_nodeService );
        //m_eventService->destroy();
        

        SERVICE_DESTROY( MovieKeyFrameService, m_movieKeyFrameService );
        SERVICE_DESTROY( PrototypeService, m_prototypeService );

        SERVICE_DESTROY( Consts, m_consts );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::calcWindowResolution( Resolution & _windowResolution ) const
	{
		Resolution dres;
        m_platform->getMaxClientResolution( dres );

		float aspect = m_windowResolution.getAspectRatio();

		size_t resHeight = m_windowResolution.getHeight();
        size_t resWidth = m_windowResolution.getWidth();

		size_t dresHeight = dres.getHeight();
        size_t dresWidth = dres.getWidth();

		if( resHeight > dresHeight )
		{
			size_t new_witdh = static_cast<size_t>( float(resHeight) * aspect + 0.5f );
            size_t new_height = dresHeight;

            if( new_witdh > dresWidth )
            {
                new_witdh = dresWidth;
                new_height = static_cast<size_t>( float(dresWidth) / aspect + 0.5f );
            }

			_windowResolution.setWidth( new_witdh );			
			_windowResolution.setHeight( new_height );
		}
		else if( resWidth > dresWidth )
        {
            size_t new_witdh = dresWidth;
            size_t new_height = static_cast<size_t>( float(resWidth) / aspect + 0.5f );

            if( new_height > dresHeight )
            {
                new_witdh = static_cast<size_t>( float(dresHeight) * aspect + 0.5f );
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
        
		if( m_fixedContentResolution == true )
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

            float areaWidth = dw * rw;
            float areaHeight = dh * rh;

            LOGGER_INFO(m_serviceProvider)("area [%d %d]"
                , areaWidth
                , areaHeight
                );

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
	void Application::setFullscreenMode( bool _fullscreen )
	{
		if( m_fullscreen == _fullscreen )
		{
			return;
		}

		m_fullscreen = _fullscreen;

        this->invalidateWindow_();

        bool fullscreen = this->getFullscreenMode();        
        m_game->onFullscreen( m_currentResolution, fullscreen );
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::invalidateWindow_()
    {
        bool fullscreen = this->getFullscreenMode();

		if( fullscreen == true )
		{
            Resolution desktopResolution;
            m_platform->getDesktopResolution( desktopResolution );

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
        
		RENDER_SERVICE(m_serviceProvider)->setVSync( m_vsync );

		m_platform->notifyWindowModeChanged( m_currentResolution, fullscreen );

		this->calcRenderViewport_( m_currentResolution, m_renderViewport );
		//m_renderEngine->applyRenderViewport( renderViewport );

		LOGGER_INFO(m_serviceProvider)( "Application::invalidateWindow_ Render Viewport %f %f - %f %f"
			, m_renderViewport.begin.x
			, m_renderViewport.begin.y
			, m_renderViewport.getWidth()
			, m_renderViewport.getHeight()
			);

		if( fullscreen == true )
		{
			m_platform->notifyCursorClipping( m_renderViewport );
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
	void Application::onPaint()
	{
		if( /*m_focus == true &&*/ RENDER_SERVICE(m_serviceProvider) && m_game && m_createRenderWindow == true )
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
	const ConstString & Application::getProjectTitle() const
	{
		if( TEXT_SERVICE(m_serviceProvider) == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Application::getProjectTitle not initialize textManager"
				);

			return ConstString::none();
		}

        ConstString key = Helper::stringizeString(m_serviceProvider, "APPLICATION_TITLE");

        const TextEntry * entry = nullptr;
		if( TEXT_SERVICE(m_serviceProvider)->existText( key, &entry ) == false )
		{
            return ConstString::none();
		}

        const String & text = entry->text;

        m_projectTitleCache = Helper::stringizeString( m_serviceProvider, text );

		return m_projectTitleCache;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Application::getProjectCodename() const
	{
		return m_projectCodename;
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

        m_game->onFixedContentResolution( m_currentResolution, m_fixedContentResolution );
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
	const char* Application::getVersionInfo()
	{
		return s_versionInfo;
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
			if( RENDER_SERVICE(m_serviceProvider) != NULL )
			{
				RENDER_SERVICE(m_serviceProvider)->setVSync( m_vsync );

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
			const FilePath & cursorName = m_cursorResource->getPath();

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

		m_cursorResource = RESOURCE_SERVICE(m_serviceProvider)
			->getResourceT<ResourceCursor>(_resourceName);

		if( m_cursorResource == NULL )
		{
            LOGGER_ERROR(m_serviceProvider)( "Application::setCursorIcon: can't find resource cursor %s"
                , _resourceName.c_str()
                );

			return;
		}

		if( m_cursorMode == false )
		{
			return;
		}

		const FilePath & cursorName = m_cursorResource->getPath();

		size_t cursorBufferSize;
		void * cursorBufferPtr = m_cursorResource->getBuffer( cursorBufferSize );

		m_platform->notifyCursorIconSetup( cursorName, cursorBufferPtr, cursorBufferSize );
	}
	////////////////////////////////////////////////////////////////////////////
	//void Application::setAsScreensaver( bool _set )
	//{
	//	//m_platform->setAsScreensaver( _set );
	//}	
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
}

