#	include "Application.h"

#   include "Interface/OptionsInterface.h"
#   include "Interface/PrototypeManagerInterface.h"
#	include "Interface/TextInterface.h"
#   include "Interface/InputSystemInterface.h"
#   include "Interface/RenderSystemInterface.h"
#	include "Interface/SoundSystemInterface.h"
#   include "Interface/ParticleSystemInterface.h"
#   include "Interface/ScriptSystemInterface.h"
#   include "Interface/PhysicSystemInterface.h"
#   include "Interface/ConfigInterface.h"
#   include "Interface/PlayerInterface.h"
#	include "Interface/PrefetcherInterface.h"
#   include "Interface/StringizeInterface.h"
#	include "Interface/NotificationServiceInterface.h"
#   include "Interface/MousePickerSystemInterface.h"
#	include "Interface/LoaderInterface.h"
#	include "Interface/NodeInterface.h"
#	include "Interface/ThreadSystemInterface.h"
#	include "Interface/CodecInterface.h"
#   include "Interface/ConverterInterface.h"
#   include "Interface/ResourceInterface.h"
#   include "Interface/AccountInterface.h"
#   include "Interface/ProfilerInterface.h"
#   include "Interface/GameInterface.h"
#   include "Interface/WatchdogInterface.h"
#   include "Interface/GraveyardInterface.h"
#   include "Interface/PackageInterface.h"
#	include "Interface/UserdataInterface.h"
#	include "Interface/TimelineInterface.h"

#	include "Logger/Logger.h"

#	include "Watchdog.h"


#	include "Kernel/Scene.h"

#	include "Factory/FactoryDefault.h"
#	include "Factory/FactoryPool.h"


#	include "Kernel/Entity.h"
#	include "Kernel/Arrow.h"
//#	include "ScriptHolder.h"

// All Node type
#	include "ParticleEmitter2.h"
#	include "HotSpot.h"
#	include "HotSpotPolygon.h"
#	include "HotSpotCircle.h"
#	include "HotSpotBubbles.h"
#	include "HotSpotImage.h"
#	include "HotSpotShape.h"
//#	include "Light2D.h"
#	include "ShadowCaster2D.h"
#	include "Gyroscope.h"
#	include "TilePolygon.h"
#	include "Point.h"
#	include "Line.h"
#	include "SoundEmitter.h"
#	include "Mesh2D.h"
#	include "Grid2D.h"
#	include "TextField.h"
#	include "TileMap.h"
#	include "Track.h"
#	include "Meshget.h"
#	include "Movie.h"
#	include "MovieSlot.h"
#	include "MovieSceneEffect.h"
#	include "MovieInternalObject.h"
#   include "MovieEvent.h"
#	include "Model3D.h"
#	include "Layer2D.h"
//#	include "Layer2DParallax.h"
//#	include "Layer2DIsometric.h"
//#	include "Layer2DLoop.h"
#	include "Kernel/Isometric.h"
#	include "Kernel/Isometric.h"
#	include "Kernel/Parallax.h"
#	include "Kernel/RenderViewport.h"
#	include "Kernel/RenderClipplane.h"
#	include "Kernel/RenderCameraOrthogonal.h"
#	include "Kernel/RenderCameraProjection.h"
#	include "Kernel/RenderCameraOrthogonalTarget.h"

#	include "Layer2DAccumulator.h"
#	include "Layer3D.h"
#	include "Window.h"
#	include "Layer2DTexture.h"
#	include "Landscape2D.h"

#   include "ShapeQuadFixed.h"
#   include "ShapeQuadFlex.h"


#	include "SurfaceVideo.h"
#	include "SurfaceSound.h"
#	include "SurfaceImage.h"
#	include "SurfaceImageSequence.h"
#	include "SurfaceTrackMatte.h"
#   include "SurfaceSolidColor.h"


// All Resource type
//#	include "ResourceSequence.h"
#	include "ResourceAnimation.h"

#	include "Kernel/ResourceImageData.h"
#	include "Kernel/ResourceMusic.h"

#	include "ResourceImageSolid.h"
#	include "ResourceImageDefault.h"
#	include "ResourceImageSubstract.h"
#	include "ResourceImageSubstractRGBAndAlpha.h"

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

//#	include "ScriptWrapper.h"

#	include "Kernel/NodePrototypeGenerator.h"
#	include "Kernel/ResourcePrototypeGenerator.h"
#	include "Kernel/SurfacePrototypeGenerator.h"

#	include "Codec/ImageDecoderMemory.h"
#	include "Codec/ImageDecoderArchive.h"

#	include "Codec/DecoderFactory.h"

#	include "Config/Config.h"
#	include "Config/Stringstream.h"

#   include "Core/String.h"
#	include "Core/IniUtil.h"

#   include "../dependencies/SDL2/include/SDL_video.h"

#   include "stdex/allocator.h"
#   include "stdex/stl_list.h"

#	include <ctime>
#	include <sstream>

#	include <math.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( Consts );
SERVICE_EXTERN( PrototypeService );
SERVICE_EXTERN( NodeService );
SERVICE_EXTERN( LoaderService );
SERVICE_EXTERN( RenderService );
SERVICE_EXTERN( RenderMaterialService );
SERVICE_EXTERN( RenderTextureService );
SERVICE_EXTERN( ResourceService );
SERVICE_EXTERN( TextService );
SERVICE_EXTERN( Watchdog );
SERVICE_EXTERN( ProfilerService );
SERVICE_EXTERN( Graveyard );
SERVICE_EXTERN( PackageService );
SERVICE_EXTERN( UserdataService );
SERVICE_EXTERN( PlayerService );
SERVICE_EXTERN( GameService );
SERVICE_EXTERN( TimelineService );
//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( Application, Menge::Application );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Application::Application()
		: m_particleEnable(true)
		, m_textEnable(true)
		, m_debugMask(0)
		, m_phycisTiming(0.f)
		, m_resetTiming(false)
		, m_maxTiming(100.f)
        , m_focus(true)
		, m_freeze(false)
        , m_update(true)				
		, m_nopause(false)
		, m_createRenderWindow(false)
		, m_cursorMode(false)
		, m_invalidateVsync( true )
		, m_invalidateCursorMode(true)
		, m_fullscreen(false)		
		, m_nofullscreen(false)
		, m_inputMouseButtonEventBlock(false)
		, m_mouseEnter(false)
        , m_resourceCheck(true)
		, m_cursorResource(nullptr)
		, m_fixedContentResolution(false)
		, m_fixedDisplayResolution(false)
		, m_vsync(false)
		, m_bits(0)
		, m_FSAAType(0)
		, m_FSAAQuality(0)
		, m_textureFiltering(true)
		, m_projectVersion(0)
		, m_debugPause(false)
		, m_debugFileOpen(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Application::~Application()
	{
	}
	//////////////////////////////////////////////////////////////////////////	
	bool Application::_initialize()
	{   
		SERVICE_CREATE( m_serviceProvider, Consts );
		SERVICE_CREATE( m_serviceProvider, PrototypeService );
		SERVICE_CREATE( m_serviceProvider, NodeService );
		SERVICE_CREATE( m_serviceProvider, LoaderService );

		SERVICE_CREATE( m_serviceProvider, RenderService );
		SERVICE_CREATE( m_serviceProvider, RenderMaterialService );
		SERVICE_CREATE( m_serviceProvider, RenderTextureService );

		SERVICE_CREATE( m_serviceProvider, ResourceService );
		SERVICE_CREATE( m_serviceProvider, TextService );
		SERVICE_CREATE( m_serviceProvider, Watchdog );
		SERVICE_CREATE( m_serviceProvider, ProfilerService );
		SERVICE_CREATE( m_serviceProvider, Graveyard );
		SERVICE_CREATE( m_serviceProvider, PackageService );
		SERVICE_CREATE( m_serviceProvider, UserdataService );
		SERVICE_CREATE( m_serviceProvider, PlayerService );
		SERVICE_CREATE( m_serviceProvider, GameService );
		SERVICE_CREATE( m_serviceProvider, TimelineService );

		if( this->registerBaseNodeTypes_() == false )
		{
			return false;
		}

		if( this->registerBaseResourceTypes_() == false )
		{
			return false;
		}

		if( this->registerSceneGenerator_() == false )
		{
			return false;
		}

        Helper::registerDecoder<ImageDecoderMemory>( m_serviceProvider, "memoryImage" );
        Helper::registerDecoder<ImageDecoderArchive>( m_serviceProvider, "archiveImage" );
	
		m_companyName = CONFIG_VALUE(m_serviceProvider, "Project", "Company", L"NONAME");
		m_projectName = CONFIG_VALUE(m_serviceProvider, "Project", "Name", L"UNKNOWN");

		m_projectCodename = CONFIG_VALUE(m_serviceProvider, "Project", "Codename", ConstString::none());
		m_projectVersion = CONFIG_VALUE(m_serviceProvider, "Project", "Version", 0U);

        m_contentResolution = CONFIG_VALUE(m_serviceProvider, "Game", "ContentResolution", Resolution(1024, 768));
        m_fixedContentResolution = CONFIG_VALUE(m_serviceProvider, "Game", "FixedContentResolution", true);
		m_fixedDisplayResolution = CONFIG_VALUE(m_serviceProvider, "Game", "FixedDisplayResolution", true);
		
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

		m_locale = CONFIG_VALUE( m_serviceProvider, "Locale", "Default", STRINGIZE_FILEPATH_LOCAL( m_serviceProvider, "en" ) );

		LOGGER_WARNING( m_serviceProvider )("Application::_initialize locale %s"
			, m_locale.c_str()
			);

		m_windowResolution = CONFIG_VALUE( m_serviceProvider, "Window", "Size", Resolution( 1024, 768 ) );
		m_bits = CONFIG_VALUE( m_serviceProvider, "Window", "Bits", 32U );
        m_fullscreen = CONFIG_VALUE( m_serviceProvider, "Window", "Fullscreen", true );
		m_nofullscreen = CONFIG_VALUE( m_serviceProvider, "Window", "NoFullscreen", false );
		m_vsync = CONFIG_VALUE( m_serviceProvider, "Window", "VSync", true );

		if( HAS_OPTION( m_serviceProvider, "nofullscreen" ) == true )
		{
			m_nofullscreen = true;
		}

		if( HAS_OPTION( m_serviceProvider, "author" ) == true || HAS_OPTION( m_serviceProvider, "support" ) == true )
		{
			LOGGER_CRITICAL( m_serviceProvider )("Author: IROV\n Email for support/feedbacks/improvement request and suggestions: irov13@mail.ru");
		}

		bool fullscreen = this->getFullscreenMode();

		if( fullscreen == true )
		{
			Resolution desktopResolution;
			PLATFORM_SERVICE( m_serviceProvider )
				->getDesktopResolution( desktopResolution );

			m_currentResolution = desktopResolution;
		}
		else
		{
			this->calcWindowResolution( m_currentResolution );
		}

        const TextEntryInterface * entry;
        if( TEXT_SERVICE( m_serviceProvider )
            ->existText( STRINGIZE_STRING_LOCAL( m_serviceProvider, "APPLICATION_TITLE" ), &entry ) == false )
        {
            LOGGER_WARNING( m_serviceProvider )("Application not setup title 'APPLICATION_TITLE'"
                );
        }
        else
        {
            m_projectTitle = entry->getValue();
        }

		if( CONFIG_VALUE( m_serviceProvider, "Debug", "ShowHotspots", false ) == true )
		{
			m_debugMask |= MENGE_DEBUG_HOTSPOTS;
		}

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool Application::registerBaseNodeTypes_()
	{	
#	define NODE_FACTORY( serviceProvider, Type )\
        if( PROTOTYPE_SERVICE(serviceProvider)\
            ->addPrototype( STRINGIZE_STRING_LOCAL(serviceProvider, "Node"), STRINGIZE_STRING_LOCAL( serviceProvider, #Type), new NodePrototypeGenerator<Type, 128> ) == false )\
		{\
			return false;\
		}

		LOGGER_WARNING(m_serviceProvider)("Creating Object Factory..." );

		NODE_FACTORY( m_serviceProvider, Node );

		NODE_FACTORY( m_serviceProvider, Entity );
		//NODE_FACTORY( m_serviceProvider, ScriptHolder );
		NODE_FACTORY( m_serviceProvider, Arrow );
		NODE_FACTORY( m_serviceProvider, Scene );

		NODE_FACTORY( m_serviceProvider, Gyroscope );
		NODE_FACTORY( m_serviceProvider, Isometric );
		NODE_FACTORY( m_serviceProvider, Parallax );
		
		//NODE_FACTORY( m_serviceProvider, ParticleEmitter );
		NODE_FACTORY( m_serviceProvider, ParticleEmitter2 );

		NODE_FACTORY( m_serviceProvider, HotSpotPolygon );
		NODE_FACTORY( m_serviceProvider, HotSpotCircle );
		NODE_FACTORY( m_serviceProvider, HotSpotBubbles );
        NODE_FACTORY( m_serviceProvider, HotSpotImage );
        NODE_FACTORY( m_serviceProvider, HotSpotShape );
		//NODE_FACTORY( Light2D );
		//NODE_FACTORY( ShadowCaster2D );
		//NODE_FACTORY( TilePolygon );
		NODE_FACTORY( m_serviceProvider, Point );
		NODE_FACTORY( m_serviceProvider, Line );
		//NODE_FACTORY( RigidBody2D );
		//NODE_FACTORY( PhysicalBody2D );
		NODE_FACTORY( m_serviceProvider, SoundEmitter );
		NODE_FACTORY( m_serviceProvider, Mesh2D );
		NODE_FACTORY( m_serviceProvider, Grid2D );
		NODE_FACTORY( m_serviceProvider, TextField );
		//NODE_FACTORY( TileMap );
		//NODE_FACTORY( Track );
		NODE_FACTORY( m_serviceProvider, Movie );
        NODE_FACTORY( m_serviceProvider, MovieSlot );
        NODE_FACTORY( m_serviceProvider, MovieSceneEffect );
		NODE_FACTORY( m_serviceProvider, MovieInternalObject );
        NODE_FACTORY( m_serviceProvider, MovieEvent );

		NODE_FACTORY( m_serviceProvider, Meshget );

		NODE_FACTORY( m_serviceProvider, Model3D );
		//NODE_FACTORY( Model );
		NODE_FACTORY( m_serviceProvider, Layer2D );
		NODE_FACTORY( m_serviceProvider, Landscape2D );
		//NODE_FACTORY( m_serviceProvider, Layer2DParallax );
		//NODE_FACTORY( m_serviceProvider, Layer2DIsometric );
		//NODE_FACTORY( m_serviceProvider, Layer2DPhysic );
		//NODE_FACTORY( Layer2DLoop );
		//NODE_FACTORY( Layer2DAccumulator );
		//NODE_FACTORY( Layer2DTexture );
		//NODE_FACTORY( LayerScene );
		//NODE_FACTORY( RenderMesh );
		NODE_FACTORY( m_serviceProvider, RenderViewport );
		NODE_FACTORY( m_serviceProvider, RenderClipplane );
		NODE_FACTORY( m_serviceProvider, RenderCameraOrthogonal );
		NODE_FACTORY( m_serviceProvider, RenderCameraProjection );
		NODE_FACTORY( m_serviceProvider, RenderCameraOrthogonalTarget );
		//NODE_FACTORY( SceneNode3D );
		NODE_FACTORY( m_serviceProvider, Window );

        NODE_FACTORY( m_serviceProvider, ShapeQuadFixed );
        NODE_FACTORY( m_serviceProvider, ShapeQuadFlex );
		
#	undef NODE_FACTORY

#	define SURFACE_FACTORY( serviceProvider, Type )\
        if( PROTOTYPE_SERVICE(serviceProvider)\
            ->addPrototype( STRINGIZE_STRING_LOCAL(serviceProvider, "Surface"), STRINGIZE_STRING_LOCAL( serviceProvider, #Type), new SurfacePrototypeGenerator<Type, 128> ) == false )\
						{\
			return false;\
						}
		
		SURFACE_FACTORY( m_serviceProvider, SurfaceVideo );
		SURFACE_FACTORY( m_serviceProvider, SurfaceSound );
        SURFACE_FACTORY( m_serviceProvider, SurfaceImage );
		SURFACE_FACTORY( m_serviceProvider, SurfaceImageSequence );
		SURFACE_FACTORY( m_serviceProvider, SurfaceTrackMatte );
        SURFACE_FACTORY( m_serviceProvider, SurfaceSolidColor );

#	undef SURFACE_FACTORY

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    namespace
    {
        class SceneCategoryGenerator
            : public ServantBase<PrototypeGeneratorInterface>
        {
        public:
            SceneCategoryGenerator()
            {
            }

		protected:
			bool initialize( const ConstString & _category, const ConstString & _prototype ) override
			{
				m_category = _category;
				m_prototype = _prototype;

				return true;
			}

        protected:
			PointerFactorable generate() override
            {
                Scene * scene = NODE_SERVICE(m_serviceProvider)
                    ->createNodeT<Scene *>( CONST_STRING(m_serviceProvider, Scene) );

                if( scene == nullptr )
                {
                    LOGGER_ERROR(m_serviceProvider)("SceneCategoryGenerator can't create %s %s"
                        , m_category.c_str()
                        , m_prototype.c_str()
                        );

                    return nullptr;
                }

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
			ConstString m_category;
			ConstString m_prototype;
        };
    }
	//////////////////////////////////////////////////////////////////////////
	bool Application::registerSceneGenerator_()
	{
		LOGGER_INFO(m_serviceProvider)( "initialize Scene Manager..." );

        PrototypeGeneratorInterface * generator = new SceneCategoryGenerator;

		if( PROTOTYPE_SERVICE( m_serviceProvider )
			->addPrototype( CONST_STRING( m_serviceProvider, Scene ), ConstString::none(), generator ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::registerBaseResourceTypes_()
	{
        LOGGER_INFO(m_serviceProvider)( "Initializing Resource Type..." );

#	define RESOURCE_FACTORY( serviceProvider, Type ) \
		if( PROTOTYPE_SERVICE(serviceProvider)\
			->addPrototype( STRINGIZE_STRING_LOCAL(serviceProvider, "Resource"), STRINGIZE_STRING_LOCAL(serviceProvider, #Type), new ResourcePrototypeGenerator<Type, 128> ) == false )\
		{\
			return false;\
		}

		RESOURCE_FACTORY( m_serviceProvider, ResourceMusic );
		
		RESOURCE_FACTORY( m_serviceProvider, ResourceAnimation );

		RESOURCE_FACTORY( m_serviceProvider, ResourceParticle );

		//RESOURCE_FACTORY( m_serviceProvider, ResourceFont );
		//RESOURCE_FACTORY( m_serviceProvider, ResourceGlyph );
		
		RESOURCE_FACTORY( m_serviceProvider, ResourceImageData );
		RESOURCE_FACTORY( m_serviceProvider, ResourceImageDefault );
        RESOURCE_FACTORY( m_serviceProvider, ResourceImageSubstract );
        RESOURCE_FACTORY( m_serviceProvider, ResourceImageSubstractRGBAndAlpha );
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
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::createRenderWindow()
	{
        //if( this->isValidWindowMode() == false )
        //{
        //    m_fullscreen = true;
        //}

        bool fullscreen = this->getFullscreenMode();

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

        m_createRenderWindow = RENDER_SERVICE( m_serviceProvider )
            ->createRenderWindow( m_currentResolution
                , m_contentResolution
                , m_renderViewport
                , m_bits
                , fullscreen
                , m_FSAAType
                , m_FSAAQuality
            );

		if( m_createRenderWindow == false )
		{
            LOGGER_ERROR(m_serviceProvider)("Application::createRenderWindow failed to create render window" 
                            );            

			return false;
		}

        GAME_SERVICE(m_serviceProvider)
			->initializeRenderResources();

        PLAYER_SERVICE( m_serviceProvider )
            ->initializeRenderResources();

		NOTIFICATION_SERVICE(m_serviceProvider)
			->notify( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION, fullscreen, m_currentResolution );
		
		GAME_SERVICE( m_serviceProvider )
			->setRenderViewport( m_renderViewport, m_contentResolution );

		float gameViewportAspect;
		Viewport gameViewport;

		this->getGameViewport( gameViewportAspect, gameViewport );

		GAME_SERVICE( m_serviceProvider )
			->setGameViewport( gameViewport, gameViewportAspect );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::initializeGame( const ConstString & _category, const FilePath & _resourceIniPath )
	{
		if( SERVICE_EXIST(m_serviceProvider, Menge::GameServiceInterface) == false )
		{
			return false;
		}

		LOGGER_INFO( m_serviceProvider )("Application load resource packs... %s:%s"
			, _category.c_str()
			, _resourceIniPath.c_str()
			);

		if( PACKAGE_SERVICE( m_serviceProvider )
			->loadPackages( _category, _resourceIniPath ) == false )
		{
			LOGGER_CRITICAL( m_serviceProvider )("Application invalid load resource packs"
				);

			return false;
		}

		if( HAS_OPTION( m_serviceProvider, "locale" ) == true )
		{
			const char * option_locale = GET_OPTION_VALUE( m_serviceProvider, "locale" );

			m_locale = Helper::stringizeString( m_serviceProvider, option_locale );

			LOGGER_WARNING( m_serviceProvider )("Application:initializeGame setup locale '%s'"
				, m_locale.c_str()
				);
		}

		LOGGER_INFO(m_serviceProvider)( "Application:initializeGame load game resource"
			);

		const Tags & platformTags = PLATFORM_SERVICE( m_serviceProvider )
			->getPlatformTags();

		if( PACKAGE_SERVICE( m_serviceProvider )
			->enablePackages( m_locale, platformTags ) == false )
		{
            LOGGER_ERROR( m_serviceProvider )("PackageService invalid enable for locale '%s' platform '%s'!"
                , m_locale.c_str()
				, platformTags.to_str().c_str()
                );

			return false;
		}

		bool developmentMode = HAS_OPTION( m_serviceProvider, "dev" );
		bool noresourceCheck = HAS_OPTION( m_serviceProvider, "noresourcecheck" );

		if( developmentMode == true && noresourceCheck == false )
		{
			if( TEXT_SERVICE(m_serviceProvider)
				->validate() == false )
			{
				LOGGER_ERROR(m_serviceProvider)("TextService invalid validate!"
					);
			}

			if( PACKAGE_SERVICE( m_serviceProvider )
				->validatePackages() == false )
			{
				LOGGER_ERROR(m_serviceProvider)("Resources validation is invalid!!!!!!!!!!!!!"
					);

				bool resourceCheckCritical = HAS_OPTION( m_serviceProvider, "noresourcecheckcritical" );

				if( resourceCheckCritical == false )
				{
					LOGGER_CRITICAL(m_serviceProvider)("Fix Resources"
						);

					return false;
				}
			}
		}

		if( GAME_SERVICE( m_serviceProvider )
			->loadPersonality() == false )
		{
			return false;
		}

		GAME_SERVICE( m_serviceProvider )
			->setCursorMode( m_cursorMode );
		        
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_printChildren2( ServiceProviderInterface * _serviceProvider, Node * _node, uint32_t _tab )
	{
		TListNodeChild & children = _node->getChildren();

		for( TSlugChild it(children); it.eof() == false; )
		{
			Node * child = *it;

			it.next_shuffle();

			if( child->isEnable() == false )
			{
				continue;
			}

			LOGGER_ERROR( _serviceProvider )("%.*s-%s [%s] (%.2f, %.2f) %d"
				, _tab
				, "                                         "
				, child->getName().c_str() 
				, child->getType().c_str()
				, child->getWorldPosition().x
				, child->getWorldPosition().y
				, child->isRenderable()
				);

			s_printChildren2( _serviceProvider, child, _tab + 1 );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_printChildren( ServiceProviderInterface * _serviceProvider, Node * _node )
	{
		if( _node == nullptr )
		{
			return;
		}

		s_printChildren2( _serviceProvider, _node, 0 );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::keyEvent( const InputKeyEvent & _event )
	{
		bool developmentMode = HAS_OPTION( m_serviceProvider, "dev" );

		if( developmentMode == true )
		{
			if( _event.code == KC_F6 && _event.isDown )
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

			if( _event.code == KC_F11 && _event.isDown )
			{
				static bool wireframeMode = false;

				wireframeMode = !wireframeMode;

				if( wireframeMode )
				{
					RENDER_SYSTEM( m_serviceProvider )
						->setFillMode( FM_WIREFRAME );
				}
				else
				{
					RENDER_SYSTEM( m_serviceProvider )
						->setFillMode( FM_SOLID );
				}
			}

			if( _event.code == KC_F10 && _event.isDown )
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

			if( _event.code == KC_F9 && _event.isDown )
			{
				PLAYER_SERVICE( m_serviceProvider )
					->toggleDebugText();
			}

			if( _event.code == KC_F8 && _event.isDown )
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

			if( _event.code == KC_F5 && _event.isDown == true )
			{
				//RESOURCE_SERVICE(m_serviceProvider)
					//->dumpResources("Application");
                class VisitorPlayerFactoryManager
                    : public VisitorPrototypeGenerator
                {
                public:
                    VisitorPlayerFactoryManager( ServiceProviderInterface * _serviceProvider, const ConstString & _category, Stringstream & _ss )
                        : m_serviceProvider( _serviceProvider )
                        , m_category( _category )
                        , m_ss( _ss )
                    {
                    }

                private:
                    VisitorPlayerFactoryManager & operator = ( const VisitorPlayerFactoryManager & _vpfm )
                    {
                        (void)_vpfm;

                        return *this;
                    }

                protected:
                    void visit( const ConstString & _category, const ConstString & _type, const PrototypeGeneratorInterfacePtr & _generator ) override
                    {
                        if( m_category != _category )
                        {
                            return;
                        }

                        uint32_t count = _generator->count();

                        if( count == 0 )
                        {
                            return;
                        }

                        m_ss << "" << _type.c_str() << ": " << count << "\n";
                    }

                protected:
                    ServiceProviderInterface * m_serviceProvider;
                    ConstString m_category;
                    Stringstream & m_ss;
                };

                Stringstream ss;
                VisitorPlayerFactoryManager pfmv( m_serviceProvider, CONST_STRING( m_serviceProvider, Node ), ss );

                PROTOTYPE_SERVICE( m_serviceProvider )
                    ->visitGenerators( &pfmv );

                const String & str = ss.str();

                LOGGER_ERROR( m_serviceProvider )("%s", str.c_str() );
			}

			if( _event.code == KC_OEM_MINUS && _event.isDown == true )
			{
				if( RENDER_SERVICE(m_serviceProvider)
					->decrefLimitRenderObjects() == false )
				{
					m_debugPause = false;

					RENDER_SERVICE(m_serviceProvider)
						->enableDebugStepRenderMode( false );
				}				
			}

			if( _event.code == KC_OEM_PLUS && _event.isDown == true )
			{
				RENDER_SERVICE(m_serviceProvider)
					->increfLimitRenderObjects();

				if( m_debugPause == false )
				{
					RENDER_SERVICE(m_serviceProvider)
						->enableDebugStepRenderMode( true );
				}

				m_debugPause = true;
			}

			if( _event.code == KC_F12 && _event.isDown == true )
			{
				bool enable = RENDER_SERVICE( m_serviceProvider )
					->isRedAlertMode();

				RENDER_SERVICE( m_serviceProvider )
					->enableRedAlertMode( !enable );
			}

			if( _event.code == KC_F4 && _event.isDown == true )
			{
				m_debugFileOpen = !m_debugFileOpen;

				if( m_debugFileOpen == true )
				{
					m_notifyDebugOpenFile = NOTIFICATION_SERVICE( m_serviceProvider )
						->addObserverMethod( NOTIFICATOR_DEBUG_OPEN_FILE, this, &Application::notifyDebugOpenFile_ );
				}
				else
				{
					m_notifyDebugOpenFile = nullptr;
				}
			}

			if( _event.code == KC_P && _event.isDown == true && INPUT_SERVICE( m_serviceProvider )->isCtrlDown() == true )
			{
				static bool s_particle_enable = true;

				s_particle_enable = !s_particle_enable;

				APPLICATION_SERVICE(m_serviceProvider)
					->setParticleEnable( s_particle_enable );
			}

			if( _event.code == KC_T && _event.isDown == true && INPUT_SERVICE( m_serviceProvider )->isCtrlDown() == true )
			{
				static bool s_text_enable = true;

				s_text_enable = !s_text_enable;

				APPLICATION_SERVICE( m_serviceProvider )
					->setTextEnable( s_text_enable );
			}

			if( _event.code == KC_R && _event.isDown == true && INPUT_SERVICE( m_serviceProvider )->isCtrlDown() == true )
			{
				static bool s_text_debug = true;

				s_text_debug = !s_text_debug;

				NOTIFICATION_SERVICE( m_serviceProvider )
					->notify( NOTIFICATOR_DEBUG_TEXT_MODE, s_text_debug );
			}

			if( _event.code == KC_E && _event.isDown == true && INPUT_SERVICE( m_serviceProvider )->isCtrlDown() == true )
			{
				NOTIFICATION_SERVICE( m_serviceProvider )
					->notify( NOTIFICATOR_RELOAD_LOCALE_PREPARE );

				NOTIFICATION_SERVICE( m_serviceProvider )
					->notify( NOTIFICATOR_RELOAD_LOCALE );

				NOTIFICATION_SERVICE( m_serviceProvider )
					->notify( NOTIFICATOR_RELOAD_LOCALE_POST );
				
				const ConstString & locale = APPLICATION_SERVICE( m_serviceProvider )
					->getLocale();

				APPLICATION_SERVICE( m_serviceProvider )
					->setLocale( locale );
			}

			if( _event.code == KC_0 && _event.isDown == true )
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

			if( _event.code == KC_F3 && _event.isDown == true )
            {
				stdex_memory_info_t mi[25];
                size_t count = stdex_allocator_memoryinfo( mi, 25 );

				size_t pool_now = 0;
				size_t pool_max = 0;

				for( size_t i = 0; i != count; ++i )
				{
					const stdex_memory_info_t & m = mi[i];

					pool_now += m.block_size * m.block_count;
					pool_max += m.block_size * m.block_total * m.chunk_count;

					printf("block %u:%u %u alloc %u:%u over %.2f\n"
						, (uint32_t)m.block_size
						, (uint32_t)m.chunk_count
						, (uint32_t)m.block_count
						, (uint32_t)(m.block_size * m.block_count)
						, (uint32_t)(m.block_size * m.block_total * m.chunk_count)
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

			if( _event.code == KC_F2 && _event.isDown == true )
			{
				Scene * scene = PLAYER_SERVICE(m_serviceProvider)
					->getCurrentScene();

				s_printChildren( m_serviceProvider, scene );
			}
		}

		bool handle = GAME_SERVICE( m_serviceProvider )
			->handleKeyEvent( _event );

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::textEvent( const InputTextEvent & _event )
	{
		bool handle = GAME_SERVICE( m_serviceProvider )
			->handleTextEvent( _event );

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::mouseButtonEvent( const InputMouseButtonEvent& _event )
	{
		if( m_inputMouseButtonEventBlock == true )
		{
			return false;
		}

		GAME_SERVICE( m_serviceProvider )
			->handleMouseButtonEventBegin( _event );

		bool handle = GAME_SERVICE( m_serviceProvider )
			->handleMouseButtonEvent( _event );

		GAME_SERVICE( m_serviceProvider )
			->handleMouseButtonEventEnd( _event );

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::mouseMove( const InputMouseMoveEvent& _event )
	{
		if( INPUT_SERVICE(m_serviceProvider)
			->validCursorPosition( _event.x, _event.y ) == false )
		{
			m_mouseEnter = false;

			return false;
		}

		if( m_mouseEnter == false )
		{
			InputMousePositionEvent ne;
			ne.type = IET_MOUSE_ENTER;
			ne.touchId = _event.touchId;
			ne.x = _event.x;
			ne.y = _event.y;
			ne.pressure = _event.pressure;

			this->mouseEnter( ne );
		}

		bool handle = GAME_SERVICE( m_serviceProvider )
			->handleMouseMove( _event );

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::notifyDebugOpenFile_( const char * _folder, const char * _fileName )
	{ 
		LOGGER_WARNING( m_serviceProvider )("open %s:%s"
			, _folder
			, _fileName
			);
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::mouseWheel( const InputMouseWheelEvent & _event )
	{
		bool handle = GAME_SERVICE( m_serviceProvider )
			->handleMouseWheel( _event );

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::mousePosition( const InputMousePositionEvent & _event )
	{
		if( INPUT_SERVICE(m_serviceProvider)
			->validCursorPosition( _event.x, _event.y ) == false )
		{
			m_mouseEnter = false;

			return;
		}

		if( m_mouseEnter == false )
		{
			InputMousePositionEvent ne;
			ne.type = IET_MOUSE_ENTER;
			ne.touchId = _event.touchId;
			ne.x = _event.x;
			ne.y = _event.y;
			ne.pressure = _event.pressure;

			this->mouseEnter( ne );
		}

		GAME_SERVICE( m_serviceProvider )
			->mousePosition( _event );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::mouseEnter( const InputMousePositionEvent & _event )
	{
		if( INPUT_SERVICE( m_serviceProvider )
			->validCursorPosition( _event.x, _event.y ) == false )
		{
			return;
		}

		m_mouseEnter = true;

		GAME_SERVICE( m_serviceProvider )
			->mouseEnter( _event );
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::mouseLeave( const InputMousePositionEvent & _event )
	{
		m_mouseEnter = false;

		GAME_SERVICE( m_serviceProvider )
			->mouseLeave( _event );
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
	void Application::setParticleEnable( bool _enable )
	{
		m_particleEnable = _enable;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::getParticleEnable() const
	{
		return m_particleEnable;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setTextEnable( bool _enable )
	{
		m_textEnable = _enable;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::getTextEnable() const
	{
		return m_textEnable;
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
	void Application::setFocus( bool _focus )
	{
		//return;
		if( m_focus == _focus ) 
		{
			return;
		}
		
		m_focus = _focus;

		LOGGER_INFO(m_serviceProvider)("Application::setFocus %d (freeze %d)"
			, m_focus
			, m_freeze
			);

		if( m_freeze == true )
		{
			return;
		}

		if( SERVICE_EXIST(m_serviceProvider, Menge::GameServiceInterface) == true )
		{
			GAME_SERVICE( m_serviceProvider )
				->setFocus( m_focus );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setFreeze( bool _freeze )
	{
		if( m_freeze == _freeze )
		{
			return;
		}

		m_freeze = _freeze;

		LOGGER_INFO( m_serviceProvider )("Application::setFreeze %d (focus %d)"
			, m_freeze
			, m_focus
			);

		if( m_focus == false )
		{
			return;
		}

		if( SERVICE_EXIST( m_serviceProvider, Menge::GameServiceInterface ) == true )
		{
			GAME_SERVICE( m_serviceProvider )
				->setFocus( !m_freeze );
		}	
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setNopause( bool _nopause )
	{
		m_nopause = _nopause;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::getNopause() const
	{
		return m_nopause;
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
			THREAD_SERVICE(m_serviceProvider)
				->update();
		}

		if( SERVICE_EXIST( m_serviceProvider, Menge::PrefetcherServiceInterface ) == true )
		{
			PREFETCHER_SERVICE( m_serviceProvider )
				->update();
		}

		if( SERVICE_EXIST( m_serviceProvider, Menge::ModuleServiceInterface ) == true )
		{
			MODULE_SERVICE( m_serviceProvider )
				->update( m_focus );
		}

		if( m_update == false && m_focus == false && m_nopause == false )
		{						
			return false;
		}

		GAME_SERVICE( m_serviceProvider )
			->update();

		INPUT_SERVICE(m_serviceProvider)
			->update();

		if( PLAYER_SERVICE( m_serviceProvider )->update() == false )
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

		float time = TIMELINE_SERVICE( m_serviceProvider )
			->getTime();

		GAME_SERVICE( m_serviceProvider )
			->tick( time, timing );

		MODULE_SERVICE( m_serviceProvider )
			->tick( time, timing );

		if( SERVICE_EXIST(m_serviceProvider, Menge::SoundServiceInterface) == true )
		{
			SOUND_SERVICE(m_serviceProvider)
				->tick( time, timing );
		}

		if( SERVICE_EXIST(m_serviceProvider, Menge::GraveyardInterface) == true )
		{
			GRAVEYARD_SERVICE(m_serviceProvider)
				->tick( time, timing );
		}

		TIMELINE_SERVICE( m_serviceProvider )
			->tick( timing );
		
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
		//PLAYER_SERVICE( m_serviceProvider )
		//	->updateChangeScene();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::render()
	{
		if( RENDER_SERVICE(m_serviceProvider)->beginScene() == false )
		{
			return false;
		}

		GAME_SERVICE( m_serviceProvider )
			->render();

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

		if( SERVICE_EXIST(m_serviceProvider, Menge::GameServiceInterface) == true )
		{
			needQuit = GAME_SERVICE( m_serviceProvider )
				->close();
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

			if( SERVICE_EXIST( m_serviceProvider, Menge::GameServiceInterface ) == true )
			{
				GAME_SERVICE( m_serviceProvider )
					->turnSound( false );
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

			if( SERVICE_EXIST( m_serviceProvider, Menge::GameServiceInterface ) == true )
			{
				GAME_SERVICE( m_serviceProvider )
					->turnSound( true );
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
	void Application::_finalize()
	{		
		m_notifyDebugOpenFile = nullptr;

		if( SERVICE_EXIST( m_serviceProvider, Menge::GameServiceInterface ) == true )
		{
			GAME_SERVICE( m_serviceProvider )
				->finalizeRenderResources();
		}

		if( SERVICE_EXIST( m_serviceProvider, Menge::PlayerServiceInterface ) == true )
		{
			PLAYER_SERVICE( m_serviceProvider )
				->finalizeRenderResources();
		}

		PLAYER_SERVICE( m_serviceProvider )
			->destroyCurrentScene();

		m_cursorResource = nullptr;

		SERVICE_FINALIZE( m_serviceProvider, GameServiceInterface );
		SERVICE_FINALIZE( m_serviceProvider, PlayerServiceInterface );
		SERVICE_FINALIZE( m_serviceProvider, PackageServiceInterface );
		SERVICE_FINALIZE( m_serviceProvider, UserdataServiceInterface );

		CODEC_SERVICE(m_serviceProvider)
			->unregisterDecoder( STRINGIZE_STRING_LOCAL(m_serviceProvider, "memoryImage") );

		CODEC_SERVICE(m_serviceProvider)
			->unregisterDecoder( STRINGIZE_STRING_LOCAL( m_serviceProvider, "archiveImage" ) );

		SERVICE_FINALIZE( m_serviceProvider, GraveyardInterface );
		SERVICE_FINALIZE( m_serviceProvider, NodeServiceInterface );
		SERVICE_FINALIZE( m_serviceProvider, ResourceServiceInterface );
		SERVICE_FINALIZE( m_serviceProvider, RenderServiceInterface );
		SERVICE_FINALIZE( m_serviceProvider, RenderMaterialServiceInterface );
		SERVICE_FINALIZE( m_serviceProvider, RenderTextureServiceInterface );
		SERVICE_FINALIZE( m_serviceProvider, TextServiceInterface );
		SERVICE_FINALIZE( m_serviceProvider, PrototypeServiceInterface );

		m_locale.clear();
		m_projectCodename.clear();
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

		float minimal_aspect = 100.f;

        for( TMapAspectRatioViewports::const_iterator
            it = m_aspectRatioViewports.begin(),
            it_found = m_aspectRatioViewports.end();
        it != it_found;
        ++it )
        {
            float aspect = it->first;

			float deltha_aspect = fabsf( _aspect - aspect );

			if( deltha_aspect < minimal_aspect )
            {
				minimal_aspect = deltha_aspect;

                _bestAspect = it->first;
                _viewport = it->second;
            }
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
		bool windowModeCheck = CONFIG_VALUE( m_serviceProvider, "Game", "WindowModeCheck", false );

		if( windowModeCheck == false )
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
		if( m_nofullscreen == true )
		{
			_fullscreen = false;
		}

		if( m_fullscreen == _fullscreen )
		{
			return;
		}

		m_fullscreen = _fullscreen;

        this->invalidateWindow_();

        bool fullscreen = this->getFullscreenMode();
		
		GAME_SERVICE(m_serviceProvider)
			->setFullscreen( m_currentResolution, fullscreen );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Application::getWindowBits() const
    {
        return m_bits;
    }
    //////////////////////////////////////////////////////////////////////////
    int Application::getWindowFSAAType() const
    {
        return m_FSAAType;
    }
    //////////////////////////////////////////////////////////////////////////
    int Application::getWindowFSAAQuality() const
    {
        return m_FSAAQuality;
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

		RENDER_SERVICE(m_serviceProvider)
            ->changeWindowMode( m_currentResolution, m_contentResolution, m_renderViewport, fullscreen );
		
		NOTIFICATION_SERVICE(m_serviceProvider)
			->notify( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION, fullscreen, m_currentResolution );

        if( SERVICE_EXIST(m_serviceProvider, Menge::GameServiceInterface) == true )
        {
			GAME_SERVICE( m_serviceProvider )
				->setRenderViewport( m_renderViewport, m_contentResolution );

			float gameViewportAspect;
			Viewport gameViewport;

			APPLICATION_SERVICE(m_serviceProvider)
				->getGameViewport( gameViewportAspect, gameViewport );

			GAME_SERVICE( m_serviceProvider )
				->setGameViewport( gameViewport, gameViewportAspect );

			LOGGER_WARNING(m_serviceProvider)("Application::invalidateWindow_ Game Viewport %f %f - %f %f Aspect %f"
				, gameViewport.begin.x
				, gameViewport.begin.y
				, gameViewport.end.x
				, gameViewport.end.y
				, gameViewportAspect
				);
        }
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::getFullscreenMode() const
	{
		if( PLATFORM_SERVICE( m_serviceProvider )
			->getNoFullscreen() == true )
		{
			return false;
		}

		if( PLATFORM_SERVICE( m_serviceProvider )
			->getAlwaysFullscreen() == true )
		{
			return true;
		}

		if( m_nofullscreen == true )
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
    uint32_t Application::getDebugMask() const
	{
		return m_debugMask;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::paint()
	{
		if( m_createRenderWindow == false )
		{
			return;
		}

		if( m_focus == true )
		{
			return;
		}

		if( SERVICE_EXIST( m_serviceProvider, Menge::RenderServiceInterface ) == false )
		{
			return;
		}

		if( SERVICE_EXIST( m_serviceProvider, Menge::GameServiceInterface ) == false )
		{
			return;
		}

		if( RENDER_SERVICE( m_serviceProvider )->beginScene() == true )
		{
			GAME_SERVICE( m_serviceProvider )
				->render();

			RENDER_SERVICE( m_serviceProvider )
				->endScene();

			RENDER_SERVICE( m_serviceProvider )
				->swapBuffers();
		}
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
    const String & Application::getProjectTitle() const
	{
		return m_projectTitle;
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
	void Application::setLocale( const ConstString & _locale )
	{
		if( m_locale == _locale )
		{
			LOGGER_WARNING( m_serviceProvider )("Application::setLocale alredy set locale '%s'"
				, _locale.c_str()
				);

			return;
		}

		ConstString prevLocale = m_locale;

		m_locale = _locale;

		LOGGER_WARNING( m_serviceProvider )("Application::setLocale new locale '%s' old '%s'"
			, m_locale.c_str()
			, prevLocale.c_str()
			);

		NOTIFICATION_SERVICE( m_serviceProvider )
			->notify( NOTIFICATOR_CHANGE_LOCALE_PREPARE, prevLocale, m_locale );

		NOTIFICATION_SERVICE( m_serviceProvider )
			->notify( NOTIFICATOR_CHANGE_LOCALE, prevLocale, m_locale );

		NOTIFICATION_SERVICE( m_serviceProvider )
			->notify( NOTIFICATOR_CHANGE_LOCALE_POST, prevLocale, m_locale );
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Application::getLocale() const
	{
		return m_locale;
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

		GAME_SERVICE( m_serviceProvider )
			->setFixedContentResolution( m_currentResolution, m_fixedContentResolution );
    }
	//////////////////////////////////////////////////////////////////////////
	bool Application::getFixedContentResolution() const
	{
		return m_fixedContentResolution;
	}
	//////////////////////////////////////////////////////////////////////////
	void Application::setFixedDisplayResolution( bool _fixedDisplayResolution )
	{
		if( m_fixedDisplayResolution == _fixedDisplayResolution )
		{
			return;
		}

		m_fixedDisplayResolution = _fixedDisplayResolution;

		this->invalidateWindow_();

		GAME_SERVICE( m_serviceProvider )
			->setFixedDisplayResolution( m_currentResolution, m_fixedDisplayResolution );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Application::getFixedDisplayResolution()
	{
		return m_fixedDisplayResolution;
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
	bool Application::getAllowFullscreenSwitchShortcut() const
	{
		return false;
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
	bool Application::getVSync() const
	{
		return m_vsync;
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

		if( SERVICE_EXIST(m_serviceProvider, Menge::GameServiceInterface) == true )
		{
			GAME_SERVICE( m_serviceProvider )
				->setCursorMode( m_cursorMode );
		}

		if( m_cursorMode == true && m_cursorResource != nullptr )
		{
			const ConstString & name = m_cursorResource->getName();

			const FilePath & path = m_cursorResource->getPath();

			const MemoryInterfacePtr & buffer = m_cursorResource->getBuffer();

			PLATFORM_SERVICE(m_serviceProvider)
				->notifyCursorIconSetup( name, path, buffer );
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
			->getResourceT<ResourceCursorPtr>( _resourceName );

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

		const ConstString & name = m_cursorResource->getName();
		const FilePath & path = m_cursorResource->getPath();

		const MemoryInterfacePtr & buffer = m_cursorResource->getBuffer();

		PLATFORM_SERVICE(m_serviceProvider)
			->notifyCursorIconSetup( name, path, buffer );
	}
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

