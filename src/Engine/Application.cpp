#include "Application.h"

#include "Interface/OptionsInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/RenderServiceInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/SoundSystemInterface.h"
#include "Interface/SoundServiceInterface.h"
#include "Interface/ScriptSystemInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/PlayerInterface.h"
#include "Interface/PrefetcherInterface.h"
#include "Interface/StringizeInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/PickerServiceInterface.h"
#include "Interface/LoaderServiceInterface.h"
#include "Interface/NodeInterface.h"
#include "Interface/CodecInterface.h"
#include "Interface/ConverterInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/AccountInterface.h"
#include "Interface/ProfilerInterface.h"
#include "Interface/GameServiceInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/WatchdogInterface.h"
#include "Interface/GraveyardInterface.h"
#include "Interface/PackageServiceInterface.h"
#include "Interface/TimelineServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Scene.h"

#include "Kernel/FactoryDefault.h"
#include "Kernel/FactoryPool.h"


#include "Kernel/Entity.h"
#include "Kernel/Arrow.h"

// All Node type
#include "HotSpot.h"
#include "HotSpotPolygon.h"
#include "HotSpotCircle.h"
#include "HotSpotBubbles.h"
#include "HotSpotImage.h"
#include "HotSpotShape.h"
#include "Gyroscope.h"
#include "Point.h"
#include "Line.h"
#include "SoundEmitter.h"
#include "Mesh2D.h"
#include "Grid2D.h"
#include "TextField.h"
#include "Meshget.h"
#include "Layer2D.h"

#include "Kernel/Interender.h"
#include "Kernel/Isometric.h"
#include "Kernel/Parallax.h"
#include "Kernel/MatrixProxy.h"
#include "Kernel/RenderViewport.h"
#include "Kernel/RenderScissor.h"
#include "Kernel/RenderCameraOrthogonal.h"
#include "Kernel/RenderCameraProjection.h"
#include "Kernel/RenderCameraOrthogonalTarget.h"

#include "Kernel/BasePrototypeGenerator.h"

#include "Window.h"
#include "Landscape2D.h"

#include "ShapeQuadFixed.h"
#include "ShapeQuadFlex.h"

#include "SurfaceSound.h"
#include "SurfaceImage.h"
#include "SurfaceImageSequence.h"
#include "SurfaceTrackMatte.h"
#include "SurfaceSolidColor.h"


// All Resource type
#include "ResourceImageSequence.h"

#include "Kernel/ResourceImageData.h"
#include "Kernel/ResourceMusic.h"

#include "ResourceImageSolid.h"
#include "ResourceImageDefault.h"
#include "ResourceImageSubstract.h"
#include "ResourceImageSubstractRGBAndAlpha.h"

#include "ResourceFile.h"
#include "ResourceSound.h"
#include "ResourceWindow.h"
#include "ResourceHIT.h"
#include "ResourceShape.h"
#include "ResourceCursorICO.h"
#include "ResourceCursorSystem.h"

#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/ResourcePrototypeGenerator.h"
#include "Kernel/SurfacePrototypeGenerator.h"

#include "Kernel/ImageDecoderMemory.h"
#include "Kernel/ImageDecoderArchive.h"

#include "Kernel/DecoderFactory.h"

#include "Config/Config.h"
#include "Config/Stringstream.h"
#include "Config/List.h"

#include "Kernel/String.h"
#include "Kernel/IniUtil.h"

#include "stdex/allocator.h"
#include "stdex/allocator_report.h"

#include <ctime>
#include <sstream>

#include <math.h>

/////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( Application, Mengine::Application );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Application::Application()
        : m_particleEnable( true )
        , m_textEnable( true )
        , m_videoEnable( true )
        , m_debugMask( 0 )
        , m_phycisFrameTime( 0.f )
        , m_resetFrameTime( false )
        , m_maxFrameTime( 100.f )
        , m_updateRevision( 0 )
        , m_focus( true )
        , m_freeze( false )
        , m_update( true )
        , m_nopause( false )
        , m_createRenderWindow( false )
        , m_cursorMode( false )
        , m_invalidateVsync( true )
        , m_invalidateCursorMode( true )
        , m_fullscreen( false )
        , m_nofullscreen( false )
        , m_inputMouseButtonEventBlock( false )
        , m_mouseEnter( false )
        , m_resourceCheck( true )
        , m_cursorResource( nullptr )
        , m_fixedContentResolution( false )
        , m_fixedDisplayResolution( false )
        , m_vsync( false )
        , m_bits( 0 )
        , m_FSAAType( 0 )
        , m_FSAAQuality( 0 )
        , m_textureFiltering( true )
        , m_projectVersion( 0 )
        , m_debugPause( false )
        , m_debugFileOpen( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Application::~Application()
    {
    }
    //////////////////////////////////////////////////////////////////////////	
    bool Application::_initializeService()
    {
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

        Helper::registerDecoder<ImageDecoderMemory>( "memoryImage" );
        Helper::registerDecoder<ImageDecoderArchive>( "archiveImage" );

        m_companyName = CONFIG_VALUE( "Project", "Company", L"NONAME" );
        m_projectName = CONFIG_VALUE( "Project", "Name", L"UNKNOWN" );

        m_projectCodename = CONFIG_VALUE( "Project", "Codename", ConstString::none() );
        m_projectVersion = CONFIG_VALUE( "Project", "Version", 0U );

        m_contentResolution = CONFIG_VALUE( "Game", "ContentResolution", Resolution( 1024, 768 ) );
        m_fixedContentResolution = CONFIG_VALUE( "Game", "FixedContentResolution", true );
        m_fixedDisplayResolution = CONFIG_VALUE( "Game", "FixedDisplayResolution", true );

        VectorAspectRatioViewports aspectRatioViewports;
        CONFIG_VALUES( "Game", "AspectRatioViewport", aspectRatioViewports );

        for( const AspectRatioViewports & viewports : aspectRatioViewports )
        {
            float aspect = viewports.width / viewports.height;

            m_aspectRatioViewports[aspect] = viewports.viewport;
        }

        m_locale = CONFIG_VALUE( "Locale", "Default", STRINGIZE_FILEPATH_LOCAL( "en" ) );

        LOGGER_WARNING( "Application::_initialize locale %s"
            , m_locale.c_str()
        );

#if TARGET_OS_IPHONE
#elif __ANDROID__
#else
        m_windowResolution = CONFIG_VALUE( "Window", "Size", Resolution( 1024, 768 ) );
#endif

        m_bits = CONFIG_VALUE( "Window", "Bits", 32U );
        m_fullscreen = CONFIG_VALUE( "Window", "Fullscreen", true );
        m_nofullscreen = CONFIG_VALUE( "Window", "NoFullscreen", false );
        m_vsync = CONFIG_VALUE( "Window", "VSync", true );

        if( HAS_OPTION( "nofullscreen" ) == true )
        {
            m_nofullscreen = true;
        }

        if( HAS_OPTION( "author" ) == true || HAS_OPTION( "support" ) == true )
        {
            LOGGER_CRITICAL( "Author: IROV\n Email for support/feedbacks/improvement request and suggestions: irov13@mail.ru" );
        }

        bool fullscreen = this->getFullscreenMode();

        if( fullscreen == true )
        {
            Resolution desktopResolution;
            PLATFORM_SERVICE()
                ->getDesktopResolution( desktopResolution );

            m_currentResolution = desktopResolution;
        }
        else
        {
            this->calcWindowResolution( m_currentResolution );
        }

        if( CONFIG_VALUE( "Debug", "ShowHotspots", false ) == true )
        {
            m_debugMask |= MENGINE_DEBUG_HOTSPOTS;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::registerBaseNodeTypes_()
    {
#	define NODE_FACTORY( Type )\
        if( PROTOTYPE_SERVICE()\
            ->addPrototype( STRINGIZE_STRING_LOCAL("Node"), STRINGIZE_STRING_LOCAL(#Type), new FactorableUnique<NodePrototypeGenerator<Type, 128> > ) == false )\
		{\
			return false;\
		}

        LOGGER_WARNING( "Creating Object Factory..." );

        NODE_FACTORY( Node );

        NODE_FACTORY( Entity );
        //NODE_FACTORY( ScriptHolder );
        NODE_FACTORY( Arrow );
        NODE_FACTORY( Scene );

        NODE_FACTORY( Gyroscope );
        NODE_FACTORY( Isometric );
        //NODE_FACTORY( Parallax );
        NODE_FACTORY( MatrixProxy );

        //NODE_FACTORY( ParticleEmitter );
        //NODE_FACTORY( ParticleEmitter2 );

        NODE_FACTORY( HotSpotPolygon );
        NODE_FACTORY( HotSpotCircle );
        NODE_FACTORY( HotSpotBubbles );
        NODE_FACTORY( HotSpotImage );
        NODE_FACTORY( HotSpotShape );
        //NODE_FACTORY( Light2D );
        //NODE_FACTORY( ShadowCaster2D );
        //NODE_FACTORY( TilePolygon );
        NODE_FACTORY( Point );
        NODE_FACTORY( Line );
        NODE_FACTORY( SoundEmitter );
        NODE_FACTORY( Mesh2D );
        NODE_FACTORY( Grid2D );
        NODE_FACTORY( TextField );
        //NODE_FACTORY( TileMap );
        //NODE_FACTORY( Track );

        NODE_FACTORY( Meshget );

        //NODE_FACTORY( Model );
        NODE_FACTORY( Interender );
        NODE_FACTORY( Layer2D );
        NODE_FACTORY( Landscape2D );
        //NODE_FACTORY( Layer2DParallax );
        //NODE_FACTORY( Layer2DIsometric );
        //NODE_FACTORY( Layer2DPhysic );
        //NODE_FACTORY( Layer2DLoop );
        //NODE_FACTORY( Layer2DAccumulator );
        //NODE_FACTORY( Layer2DTexture );
        //NODE_FACTORY( LayerScene );
        //NODE_FACTORY( RenderMesh );
        NODE_FACTORY( RenderViewport );
        NODE_FACTORY( RenderScissor );
        NODE_FACTORY( RenderCameraOrthogonal );
        NODE_FACTORY( RenderCameraProjection );
        NODE_FACTORY( RenderCameraOrthogonalTarget );
        //NODE_FACTORY( SceneNode3D );
        NODE_FACTORY( Window );

        NODE_FACTORY( ShapeQuadFixed );
        NODE_FACTORY( ShapeQuadFlex );

#	undef NODE_FACTORY

#	define SURFACE_FACTORY(Type)\
        if( PROTOTYPE_SERVICE()\
            ->addPrototype( STRINGIZE_STRING_LOCAL("Surface"), STRINGIZE_STRING_LOCAL(#Type), new FactorableUnique<SurfacePrototypeGenerator<Type, 128> > ) == false )\
						{\
			return false;\
						}

        SURFACE_FACTORY( SurfaceSound );
        SURFACE_FACTORY( SurfaceImage );
        SURFACE_FACTORY( SurfaceImageSequence );
        SURFACE_FACTORY( SurfaceTrackMatte );
        SURFACE_FACTORY( SurfaceSolidColor );

#	undef SURFACE_FACTORY

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    namespace
    {
        class SceneCategoryGenerator
            : public BasePrototypeGenerator
        {
        public:
            SceneCategoryGenerator()
            {
            }

            ~SceneCategoryGenerator() override
            {
            }

        protected:
            FactorablePointer generate() override
            {
                ScenePtr scene = NODE_SERVICE()
                    ->createNode( STRINGIZE_STRING_LOCAL( "Scene" ) );

                if( scene == nullptr )
                {
                    LOGGER_ERROR( "SceneCategoryGenerator can't create %s %s"
                        , m_category.c_str()
                        , m_prototype.c_str()
                    );

                    return nullptr;
                }

                return scene;
            }
        };
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::registerSceneGenerator_()
    {
        LOGGER_INFO( "initialize Scene Manager..." );

        PrototypeGeneratorInterfacePtr generator = new FactorableUnique<SceneCategoryGenerator>;

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Scene" ), ConstString::none(), generator ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::registerBaseResourceTypes_()
    {
        LOGGER_INFO( "Initializing Resource Type..." );

#	define RESOURCE_FACTORY( Type ) \
		if( PROTOTYPE_SERVICE()\
			->addPrototype( STRINGIZE_STRING_LOCAL("Resource"), STRINGIZE_STRING_LOCAL(#Type), new FactorableUnique<ResourcePrototypeGenerator<Type, 128> > ) == false )\
		{\
			return false;\
		}

        RESOURCE_FACTORY( ResourceMusic );

        RESOURCE_FACTORY( ResourceImageSequence );

        //RESOURCE_FACTORY( ResourceFont );
        //RESOURCE_FACTORY( ResourceGlyph );

        RESOURCE_FACTORY( ResourceImage );
        RESOURCE_FACTORY( ResourceImageData );
        RESOURCE_FACTORY( ResourceImageDefault );
        RESOURCE_FACTORY( ResourceImageSubstract );
        RESOURCE_FACTORY( ResourceImageSubstractRGBAndAlpha );
        RESOURCE_FACTORY( ResourceImageSolid );
        RESOURCE_FACTORY( ResourceSound );
        RESOURCE_FACTORY( ResourceFile );

        RESOURCE_FACTORY( ResourceWindow );
        RESOURCE_FACTORY( ResourceHIT );
        RESOURCE_FACTORY( ResourceShape );
        RESOURCE_FACTORY( ResourceCursorICO );
        RESOURCE_FACTORY( ResourceCursorSystem );

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

        bool vsync = this->getVSync();

        RENDER_SERVICE()
            ->setVSync( vsync );

        LOGGER_WARNING( "Application::createRenderWindow current resolution %d %d %s"
            , m_currentResolution.getWidth()
            , m_currentResolution.getHeight()
            , fullscreen ? "Fullscreen" : "Window"
        );

        this->calcRenderViewport_( m_currentResolution, m_renderViewport );

        LOGGER_INFO( "Application::createRenderWindow render viewport %f %f - %f %f"
            , m_renderViewport.begin.x
            , m_renderViewport.begin.y
            , m_renderViewport.getWidth()
            , m_renderViewport.getHeight()
        );

        m_createRenderWindow = RENDER_SERVICE()
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
            LOGGER_ERROR( "Application::createRenderWindow failed to create render window"
            );

            return false;
        }

        GAME_SERVICE()
            ->initializeRenderResources();

        PLAYER_SERVICE()
            ->initializeRenderResources();

        NOTIFICATION_SERVICE()
            ->notify( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION, fullscreen, m_currentResolution );

        GAME_SERVICE()
            ->setRenderViewport( m_renderViewport, m_contentResolution );

        float gameViewportAspect;
        Viewport gameViewport;

        this->getGameViewport( gameViewportAspect, gameViewport );

        GAME_SERVICE()
            ->setGameViewport( gameViewport, gameViewportAspect );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::initializeGame( const FileGroupInterfacePtr & _category, const FilePath & _resourceIniPath )
    {
        if( SERVICE_EXIST( Mengine::GameServiceInterface ) == false )
        {
            return false;
        }

        LOGGER_INFO( "Application load resource packs... %s:%s"
            , _category->getName().c_str()
            , _resourceIniPath.c_str()
        );

        if( PACKAGE_SERVICE()
            ->loadPackages( _category, _resourceIniPath ) == false )
        {
            LOGGER_CRITICAL( "Application invalid load resource packs"
            );

            return false;
        }

        if( HAS_OPTION( "locale" ) == true )
        {
            const char * option_locale = GET_OPTION_VALUE( "locale" );

            m_locale = Helper::stringizeString( option_locale );

            LOGGER_WARNING( "Application:initializeGame setup locale '%s'"
                , m_locale.c_str()
            );
        }

        LOGGER_INFO( "Application:initializeGame load game resource"
        );

        const Tags & platformTags = PLATFORM_SERVICE()
            ->getPlatformTags();

        if( PACKAGE_SERVICE()
            ->enablePackages( m_locale, platformTags ) == false )
        {
            LOGGER_ERROR( "PackageService invalid enable for locale '%s' platform '%s'!"
                , m_locale.c_str()
                , platformTags.to_str().c_str()
            );

            return false;
        }

#ifndef MENGINE_MASTER_RELEASE
        bool developmentMode = HAS_OPTION( "dev" );
        bool noresourceCheck = HAS_OPTION( "noresourcecheck" );

        if( developmentMode == true && noresourceCheck == false )
        {
            if( TEXT_SERVICE()
                ->validate() == false )
            {
                LOGGER_ERROR( "TextService invalid validate!"
                );
            }

            //if( PACKAGE_SERVICE()
            //    ->validatePackages() == false )
            //{
            //    LOGGER_ERROR( "Resources validation is invalid!!!!!!!!!!!!!"
            //    );

            //    bool resourceCheckCritical = HAS_OPTION( "noresourcecheckcritical" );

            //    if( resourceCheckCritical == false )
            //    {
            //        LOGGER_CRITICAL( "Fix Resources"
            //        );

            //        return false;
            //    }
            //}
        }
#endif

        m_cursorMode = CONFIG_VALUE( "Platform", "Cursor", false );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::loadPersonality()
    {
        if( GAME_SERVICE()
            ->loadPersonality() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    static void s_printChildren2( const NodePtr & _node, uint32_t _tab )
    {
        IntrusiveSlugListNodeChild & children = _node->getChildren();

        for( IntrusiveSlugChild it( children ); it.eof() == false; )
        {
            const NodePtr & child = *it;

            it.next_shuffle();

            if( child->isEnable() == false )
            {
                continue;
            }

            LOGGER_ERROR( "%.*s-%s [%s] |%p| lp (%.2f, %.2f, %.2f) lo (%.2f, %.2f, %.2f) wp (%.2f, %.2f, %.2f) %s"
                , _tab
                , "                                         "                
                , child->getName().c_str()
                , child->getType().c_str()
                , child.get()
                , child->getLocalPosition().x
                , child->getLocalPosition().y
                , child->getLocalPosition().z
                , child->getOrientation().x
                , child->getOrientation().y
                , child->getOrientation().z
                , child->getWorldPosition().x
                , child->getWorldPosition().y
                , child->getWorldPosition().z
                , child->getAnimation() == nullptr ? "" : (child->getAnimation()->isPlay() == true ? "[Play]" : "[Stop]")
            );

            s_printChildren2( child, _tab + 1 );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    static void s_printChildren( const NodePtr & _node )
    {
        if( _node == nullptr )
        {
            return;
        }

        s_printChildren2( _node, 0 );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::keyEvent( const InputKeyEvent & _event )
    {
        bool developmentMode = HAS_OPTION( "dev" );

        if( developmentMode == true )
        {
            if( _event.code == KC_F6 && _event.isDown )
            {
                if( (m_debugMask & MENGINE_DEBUG_HOTSPOTS) != 0 )
                {
                    m_debugMask ^= MENGINE_DEBUG_HOTSPOTS;
                }
                else
                {
                    m_debugMask |= MENGINE_DEBUG_HOTSPOTS;
                }
            }

            if( _event.code == KC_F11 && _event.isDown )
            {
                static bool wireframeMode = false;

                wireframeMode = !wireframeMode;

                if( wireframeMode == true )
                {
                    RENDER_SYSTEM()
                        ->setFillMode( FM_WIREFRAME );
                }
                else
                {
                    RENDER_SYSTEM()
                        ->setFillMode( FM_SOLID );
                }
            }

            if( _event.code == KC_F10 && _event.isDown )
            {
                if( (m_debugMask & MENGINE_DEBUG_NODES) != 0 )
                {
                    m_debugMask ^= MENGINE_DEBUG_NODES;
                }
                else
                {
                    m_debugMask |= MENGINE_DEBUG_NODES;
                }
            }

            if( _event.code == KC_F8 && _event.isDown )
            {
                if( (m_debugMask & MENGINE_DEBUG_TILEPOLYGON) != 0 )
                {
                    m_debugMask ^= MENGINE_DEBUG_TILEPOLYGON;
                }
                else
                {
                    m_debugMask |= MENGINE_DEBUG_TILEPOLYGON;
                }
            }

            if( _event.code == KC_F5 && _event.isDown == true )
            {
                class VisitorPlayerFactoryManager
                {
                public:
                    VisitorPlayerFactoryManager( const ConstString & _category, Stringstream & _ss )
                        : m_category( _category )
                        , m_ss( _ss )
                    {
                    }

				public:
                    void visit( const PrototypeGeneratorInterfacePtr & _generator )
                    {
                        const ConstString & category = _generator->getCategory();
                        const ConstString & prototype = _generator->getPrototype();

                        if( m_category != category )
                        {
                            return;
                        }

                        uint32_t count = _generator->count();

                        if( count == 0 )
                        {
                            return;
                        }

                        m_ss << "" << prototype.c_str() << ": " << count << "\n";
                    }

                protected:
                    ConstString m_category;
                    Stringstream & m_ss;
                };

                Stringstream ss;
                VisitorPlayerFactoryManager pfmv( STRINGIZE_STRING_LOCAL( "Node" ), ss );

                PROTOTYPE_SERVICE()
					->foreachGenerators([&pfmv](const PrototypeGeneratorInterfacePtr & _generator) {pfmv.visit(_generator); });

                const String & str = ss.str();

                LOGGER_ERROR( "%s", str.c_str() );
            }

            if( _event.code == KC_OEM_MINUS && _event.isDown == true )
            {
                if( RENDER_SERVICE()
                    ->decrefLimitRenderObjects() == false )
                {
                    m_debugPause = false;

                    RENDER_SERVICE()
                        ->enableDebugStepRenderMode( false );
                }
            }

            if( _event.code == KC_OEM_PLUS && _event.isDown == true )
            {
                RENDER_SERVICE()
                    ->increfLimitRenderObjects();

                if( m_debugPause == false )
                {
                    RENDER_SERVICE()
                        ->enableDebugStepRenderMode( true );
                }

                m_debugPause = true;
            }

            if( _event.code == KC_F12 && _event.isDown == true )
            {
                bool enable = RENDER_SERVICE()
                    ->isRedAlertMode();

                RENDER_SERVICE()
                    ->enableRedAlertMode( !enable );
            }

            if( _event.code == KC_F4 && _event.isDown == true )
            {
                m_debugFileOpen = !m_debugFileOpen;

                if( m_debugFileOpen == true )
                {
                    NOTIFICATION_SERVICE()
                        ->addObserverMethod( NOTIFICATOR_DEBUG_OPEN_FILE, this, &Application::notifyDebugOpenFile_ );
                }
                else
                {
                    NOTIFICATION_SERVICE()
                        ->removeObserver( NOTIFICATOR_DEBUG_OPEN_FILE, this );
                }
            }

            if( _event.code == KC_P && _event.isDown == true && INPUT_SERVICE()->isCtrlDown() == true )
            {
                static bool s_particle_enable = true;

                s_particle_enable = !s_particle_enable;

                APPLICATION_SERVICE()
                    ->setParticleEnable( s_particle_enable );
            }

            if( _event.code == KC_T && _event.isDown == true && INPUT_SERVICE()->isCtrlDown() == true )
            {
                static bool s_text_enable = true;

                s_text_enable = !s_text_enable;

                APPLICATION_SERVICE()
                    ->setTextEnable( s_text_enable );
            }

            if( _event.code == KC_O && _event.isDown == true && INPUT_SERVICE()->isCtrlDown() == true )
            {
                static bool s_video_enable = true;

                s_video_enable = !s_video_enable;

                APPLICATION_SERVICE()
                    ->setVideoEnable( s_video_enable );
            }

            if( _event.code == KC_R && _event.isDown == true && INPUT_SERVICE()->isCtrlDown() == true )
            {
                static bool s_text_debug = true;

                s_text_debug = !s_text_debug;

                NOTIFICATION_SERVICE()
                    ->notify( NOTIFICATOR_DEBUG_TEXT_MODE, s_text_debug );
            }

            if( _event.code == KC_E && _event.isDown == true && INPUT_SERVICE()->isCtrlDown() == true )
            {
                NOTIFICATION_SERVICE()
                    ->notify( NOTIFICATOR_RELOAD_LOCALE_PREPARE );

                NOTIFICATION_SERVICE()
                    ->notify( NOTIFICATOR_RELOAD_LOCALE );

                NOTIFICATION_SERVICE()
                    ->notify( NOTIFICATOR_RELOAD_LOCALE_POST );

                const ConstString & locale = APPLICATION_SERVICE()
                    ->getLocale();

                APPLICATION_SERVICE()
                    ->setLocale( locale );
            }

            if( _event.code == KC_0 && _event.isDown == true )
            {
                static uint32_t batchMode = RENDER_SERVICE()
                    ->getBatchMode();

                ++batchMode;

                ERenderBatchMode mode = ERBM_NONE;
                switch( batchMode % 3 )
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

                RENDER_SERVICE()
                    ->setBatchMode( mode );
            }

            if( _event.code == KC_F3 && _event.isDown == true )
            {
                uint32_t count = stdex_allocator_report_count();

                for( uint32_t i = 0; i != count; ++i )
                {
                    stdex_memory_report_t * report = stdex_allocator_report_info( i );

                    LOGGER_ERROR( "block %s size [%u]"
                        , report->name
                        , report->count
                    );
                }
            }

            if( _event.code == KC_F2 && _event.isDown == true )
            {
                const ScenePtr & scene = PLAYER_SERVICE()
                    ->getCurrentScene();

                s_printChildren( scene );
            }
        }

        bool handle = GAME_SERVICE()
            ->handleKeyEvent( _event );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::textEvent( const InputTextEvent & _event )
    {
        bool handle = GAME_SERVICE()
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

        GAME_SERVICE()
            ->handleMouseButtonEventBegin( _event );

        bool handle = GAME_SERVICE()
            ->handleMouseButtonEvent( _event );

        GAME_SERVICE()
            ->handleMouseButtonEventEnd( _event );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::mouseMove( const InputMouseMoveEvent& _event )
    {
        float vx;
        float vy;
        if( INPUT_SERVICE()
            ->validCursorPosition( _event.x, _event.y, &vx, &vy ) == false )
        {
            m_mouseEnter = false;

            return false;
        }

        if( m_mouseEnter == false )
        {
            InputMousePositionEvent ne;
            ne.type = IET_MOUSE_ENTER;
            ne.touchId = _event.touchId;
            ne.x = vx;
            ne.y = vy;
            ne.pressure = _event.pressure;

            this->mouseEnter( ne );
        }

        bool handle = GAME_SERVICE()
            ->handleMouseMove( _event );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::notifyDebugOpenFile_( const char * _folder, const char * _fileName )
    {
        LOGGER_WARNING( "open %s:%s"
            , _folder
            , _fileName
        );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::mouseWheel( const InputMouseWheelEvent & _event )
    {
        bool handle = GAME_SERVICE()
            ->handleMouseWheel( _event );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::mousePosition( const InputMousePositionEvent & _event )
    {
        float vx;
        float vy;

        if( INPUT_SERVICE()
            ->validCursorPosition( _event.x, _event.y, &vx, &vy ) == false )
        {
            m_mouseEnter = false;

            return;
        }

        if( m_mouseEnter == false )
        {
            InputMousePositionEvent ne;
            ne.type = IET_MOUSE_ENTER;
            ne.touchId = _event.touchId;
            ne.x = vx;
            ne.y = vy;
            ne.pressure = _event.pressure;

            this->mouseEnter( ne );
        }

        GAME_SERVICE()
            ->mousePosition( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::mouseEnter( const InputMousePositionEvent & _event )
    {
        float vx;
        float vy;

        if( INPUT_SERVICE()
            ->validCursorPosition( _event.x, _event.y, &vx, &vy ) == false )
        {
            return;
        }

        m_mouseEnter = true;

        InputMousePositionEvent vevent = _event;
        vevent.x = vx;
        vevent.y = vy;

        GAME_SERVICE()
            ->mouseEnter( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::mouseLeave( const InputMousePositionEvent & _event )
    {
        m_mouseEnter = false;

        GAME_SERVICE()
            ->mouseLeave( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::quit()
    {
        PLATFORM_SERVICE()
            ->stopPlatform();

        RENDER_SERVICE()
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
    void Application::setVideoEnable( bool _enabled )
    {
        m_videoEnable = _enabled;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::getVideoEnable() const
    {
        return m_videoEnable;
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

        LOGGER_INFO( "Application::setFocus %d (freeze %d)"
            , m_focus
            , m_freeze
        );

        if( m_freeze == true )
        {
            return;
        }

        if( SERVICE_EXIST( Mengine::GameServiceInterface ) == true )
        {
            GAME_SERVICE()
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

        LOGGER_INFO( "Application::setFreeze %d (focus %d)"
            , m_freeze
            , m_focus
        );

        if( m_focus == false )
        {
            return;
        }

        if( SERVICE_EXIST( Mengine::GameServiceInterface ) == true )
        {
            GAME_SERVICE()
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
        PLATFORM_SERVICE()
            ->minimizeWindow();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::beginUpdate()
    {
        if( SERVICE_EXIST( Mengine::ThreadServiceInterface ) == true )
        {
            THREAD_SERVICE()
                ->update();
        }

        if( SERVICE_EXIST( Mengine::PrefetcherServiceInterface ) == true )
        {
            PREFETCHER_SERVICE()
                ->update();
        }

        if( SERVICE_EXIST( Mengine::ModuleServiceInterface ) == true )
        {
            MODULE_SERVICE()
                ->update( m_focus );
        }

        if( m_update == false && m_focus == false && m_nopause == false )
        {
            return false;
        }

        GAME_SERVICE()
            ->update();

        INPUT_SERVICE()
            ->update();

        if( PLAYER_SERVICE()->update() == false )
        {
            this->quit();

            return false;
        }

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
    void Application::tick( float _time )
    {
        ++m_updateRevision;

        float time = _time;

        if( m_debugPause == true )
        {
            time = 0.f;
        }

        if( _time > m_maxFrameTime )
        {
            time = m_maxFrameTime;
        }

        float current = TIMELINE_SERVICE()
            ->getTime();

        UpdateContext applicationContext;
        applicationContext.revision = m_updateRevision;
        applicationContext.current = current;
        applicationContext.time = time;

        GAME_SERVICE()
            ->tick( &applicationContext );

        MODULE_SERVICE()
            ->tick( &applicationContext );

        if( SERVICE_EXIST( Mengine::SoundServiceInterface ) == true )
        {
            SOUND_SERVICE()
                ->tick( &applicationContext );
        }

        if( SERVICE_EXIST( Mengine::GraveyardInterface ) == true )
        {
            GRAVEYARD_SERVICE()
                ->tick( &applicationContext );
        }

        TIMELINE_SERVICE()
            ->tick( time );
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::endUpdate()
    {
        //PLAYER_SERVICE()
        //	->updateChangeScene();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::render()
    {
        if( RENDER_SERVICE()->beginScene() == false )
        {
            return false;
        }

        GAME_SERVICE()
            ->render();

        RENDER_SERVICE()
            ->endScene();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::flush()
    {
        RENDER_SERVICE()
            ->swapBuffers();
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::close()
    {
        bool needQuit = true;

        if( SERVICE_EXIST( Mengine::GameServiceInterface ) == true )
        {
            needQuit = GAME_SERVICE()
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
            if( SERVICE_EXIST( SoundServiceInterface ) == true )
            {
                SOUND_SERVICE()
                    ->onTurnStream( false );
            }

            if( SERVICE_EXIST( Mengine::GameServiceInterface ) == true )
            {
                GAME_SERVICE()
                    ->turnSound( false );
            }

            if( SERVICE_EXIST( SoundServiceInterface ) == true )
            {
                SOUND_SERVICE()
                    ->onTurnSound( false );
            }
        }
        else
        {
            if( SERVICE_EXIST( SoundServiceInterface ) == true )
            {
                SOUND_SERVICE()
                    ->onTurnSound( true );
            }

            if( SERVICE_EXIST( GameServiceInterface ) == true )
            {
                GAME_SERVICE()
                    ->turnSound( true );
            }

            if( SERVICE_EXIST( SoundServiceInterface ) == true )
            {
                SOUND_SERVICE()
                    ->onTurnStream( true );
            }
        }

        if( SERVICE_EXIST( SoundServiceInterface ) == true )
        {
            SOUND_SERVICE()
                ->updateVolume();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::_finalizeService()
    {
        NOTIFICATION_SERVICE()
            ->removeObserver( NOTIFICATOR_DEBUG_OPEN_FILE, this );

        if( SERVICE_EXIST( GameServiceInterface ) == true )
        {
            GAME_SERVICE()
                ->finalizeRenderResources();
        }

        if( SERVICE_EXIST( PlayerServiceInterface ) == true )
        {
            PLAYER_SERVICE()
                ->finalizeRenderResources();
        }

        PLAYER_SERVICE()
            ->destroyCurrentScene();

        m_cursorResource = nullptr;

        CODEC_SERVICE()
            ->unregisterDecoder( STRINGIZE_STRING_LOCAL( "memoryImage" ) );

        CODEC_SERVICE()
            ->unregisterDecoder( STRINGIZE_STRING_LOCAL( "archiveImage" ) );

        m_locale.clear();
        m_projectCodename.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::calcWindowResolution( Resolution & _windowResolution ) const
    {
        Resolution dres;
        PLATFORM_SERVICE()
            ->getMaxClientResolution( dres );

        LOGGER_WARNING( "Application::calcWindowResolution Max Client Resolution Resolution %u %u"
            , dres.getWidth()
            , dres.getHeight()
        );

        LOGGER_WARNING( "Application::calcWindowResolution Window Resolution Resolution %u %u"
            , m_windowResolution.getWidth()
            , m_windowResolution.getHeight()
        );

        float aspect = m_windowResolution.getAspectRatio();

        uint32_t resHeight = m_windowResolution.getHeight();
        uint32_t resWidth = m_windowResolution.getWidth();

        uint32_t dresHeight = dres.getHeight();
        uint32_t dresWidth = dres.getWidth();

        if( resHeight > dresHeight )
        {
            uint32_t new_witdh = static_cast<uint32_t>(float( resHeight ) * aspect + 0.5f);
            uint32_t new_height = dresHeight;

            if( new_witdh > dresWidth )
            {
                new_witdh = dresWidth;
                new_height = static_cast<uint32_t>(float( dresWidth ) / aspect + 0.5f);
            }

            _windowResolution.setWidth( new_witdh );
            _windowResolution.setHeight( new_height );
        }
        else if( resWidth > dresWidth )
        {
            uint32_t new_witdh = dresWidth;
            uint32_t new_height = static_cast<uint32_t>(float( resWidth ) / aspect + 0.5f);

            if( new_height > dresHeight )
            {
                new_witdh = static_cast<uint32_t>(float( dresHeight ) * aspect + 0.5f);
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
        LOGGER_INFO( "Application::findBestAspectViewport_ for aspect %f"
            , _aspect
        );

        if( m_aspectRatioViewports.empty() == true )
        {
            LOGGER_INFO( "Application::findBestAspectViewport_ empty"
            );

            return false;
        }

        float minimal_aspect = 100.f;

        for( MapAspectRatioViewports::const_iterator
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

        LOGGER_INFO( "Application::findBestAspectViewport_ best aspect %f viewport [%f, %f, %f, %f]"
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
        LOGGER_INFO( "Application::calcRenderViewport resolution [%d %d]"
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

            float areaWidth = ceilf( dw * rw );
            float areaHeight = ceilf( dh * rh );

            LOGGER_INFO( "area [%d %d]"
                , areaWidth
                , areaHeight
            );

            _viewport.begin.x = ceilf( (rw - areaWidth) * 0.5f );
            _viewport.begin.y = ceilf( (rh - areaHeight) * 0.5f );
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
        bool windowModeCheck = CONFIG_VALUE( "Game", "WindowModeCheck", false );

        if( windowModeCheck == false )
        {
            return true;
        }

        Resolution windowResolution;
        this->calcWindowResolution( windowResolution );

        LOGGER_INFO( "Application::isValidWindowMode resolution %d:%d\n"
            , windowResolution.getWidth()
            , windowResolution.getHeight()
        );

        float aspect = windowResolution.getAspectRatio();

        Viewport aspectRatioViewport;
        float bestAspect;

        if( this->findBestAspectViewport_( aspect, bestAspect, aspectRatioViewport ) == true )
        {
            LOGGER_INFO( "Application::isValidWindowMode viewport (1) %f:%f\n"
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
            LOGGER_INFO( "Application::isValidWindowMode viewport (2) %d:%d\n"
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
        LOGGER_ERROR( "Application::changeWindowResolution %u %u -> %u %u"
            , m_windowResolution.getWidth()
            , m_windowResolution.getHeight()
            , _resolution.getWidth()
            , _resolution.getHeight()
        );

        if( m_windowResolution == _resolution )
        {
            return;
        }

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

        GAME_SERVICE()
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
            PLATFORM_SERVICE()
                ->getDesktopResolution( desktopResolution );

            m_currentResolution = desktopResolution;
        }
        else
        {
            this->calcWindowResolution( m_currentResolution );
        }

        LOGGER_WARNING( "Application::invalidateWindow_ %d Current Resolution %d %d"
            , fullscreen
            , m_currentResolution.getWidth()
            , m_currentResolution.getHeight()
        );

        bool vsync = this->getVSync();

        RENDER_SERVICE()
            ->setVSync( vsync );

        PLATFORM_SERVICE()
            ->notifyWindowModeChanged( m_currentResolution, fullscreen );

        this->calcRenderViewport_( m_currentResolution, m_renderViewport );
        //m_renderEngine->applyRenderViewport( renderViewport );

        LOGGER_WARNING( "Application::invalidateWindow_ Render Viewport %f %f - %f %f"
            , m_renderViewport.begin.x
            , m_renderViewport.begin.y
            , m_renderViewport.getWidth()
            , m_renderViewport.getHeight()
        );

        RENDER_SERVICE()
            ->changeWindowMode( m_currentResolution, m_contentResolution, m_renderViewport, fullscreen );

        NOTIFICATION_SERVICE()
            ->notify( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION, fullscreen, m_currentResolution );

        if( SERVICE_EXIST( Mengine::GameServiceInterface ) == true )
        {
            GAME_SERVICE()
                ->setRenderViewport( m_renderViewport, m_contentResolution );

            float gameViewportAspect;
            Viewport gameViewport;

            APPLICATION_SERVICE()
                ->getGameViewport( gameViewportAspect, gameViewport );

            GAME_SERVICE()
                ->setGameViewport( gameViewport, gameViewportAspect );

            LOGGER_WARNING( "Application::invalidateWindow_ Game Viewport %f %f - %f %f Aspect %f"
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
        if( PLATFORM_SERVICE()
            ->getNoFullscreen() == true )
        {
            return false;
        }

        if( PLATFORM_SERVICE()
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

        if( SERVICE_EXIST( RenderServiceInterface ) == false )
        {
            return;
        }

        if( SERVICE_EXIST( GameServiceInterface ) == false )
        {
            return;
        }

        if( RENDER_SERVICE()->beginScene() == true )
        {
            GAME_SERVICE()
                ->render();

            RENDER_SERVICE()
                ->endScene();

            RENDER_SERVICE()
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
            LOGGER_WARNING( "Application::setLocale alredy set locale '%s'"
                , _locale.c_str()
            );

            return;
        }

        ConstString prevLocale = m_locale;

        m_locale = _locale;

        LOGGER_WARNING( "Application::setLocale new locale '%s' old '%s'"
            , m_locale.c_str()
            , prevLocale.c_str()
        );

        NOTIFICATION_SERVICE()
            ->notify( NOTIFICATOR_CHANGE_LOCALE_PREPARE, prevLocale, m_locale );

        NOTIFICATION_SERVICE()
            ->notify( NOTIFICATOR_CHANGE_LOCALE, prevLocale, m_locale );

        NOTIFICATION_SERVICE()
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

        GAME_SERVICE()
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

        GAME_SERVICE()
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
        bool novsync = HAS_OPTION( "novsync" );

        if( novsync == true )
        {
            return false;
        }

        return m_vsync;
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::updateNotification()
    {
        if( m_invalidateVsync == true )
        {
            bool vsync = this->getVSync();

            if( RENDER_SERVICE() != nullptr )
            {
                RENDER_SERVICE()
                    ->setVSync( vsync );
            }

            PLATFORM_SERVICE()
                ->notifyVsyncChanged( vsync );

            m_invalidateVsync = false;
        }

        if( m_invalidateCursorMode == true )
        {
            PLATFORM_SERVICE()
                ->notifyCursorModeChanged( m_cursorMode );

            m_invalidateCursorMode = false;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::setCursorMode( bool _mode )
    {
        m_cursorMode = _mode;

        if( SERVICE_EXIST( GameServiceInterface ) == true )
        {
            GAME_SERVICE()
                ->setCursorMode( m_cursorMode );
        }

        if( m_cursorMode == true && m_cursorResource != nullptr )
        {
            const ConstString & name = m_cursorResource->getName();

            const FilePath & path = m_cursorResource->getPath();

            const MemoryInterfacePtr & buffer = m_cursorResource->getBuffer();

            PLATFORM_SERVICE()
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

        const ResourceCursorPtr & cursorResource = RESOURCE_SERVICE()
            ->getResource( _resourceName );

        if( cursorResource == nullptr )
        {
            LOGGER_ERROR( "Application::setCursorIcon: can't find resource cursor %s"
                , _resourceName.c_str()
            );

            return;
        }

        m_cursorResource = cursorResource;

        if( m_cursorMode == false )
        {
            return;
        }

        const ConstString & name = m_cursorResource->getName();
        const FilePath & path = m_cursorResource->getPath();

        const MemoryInterfacePtr & buffer = m_cursorResource->getBuffer();

        PLATFORM_SERVICE()
            ->notifyCursorIconSetup( name, path, buffer );
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::showKeyboard()
    {
        PLATFORM_SERVICE()
            ->showKeyboard();
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::hideKeyboard()
    {
        PLATFORM_SERVICE()
            ->hideKeyboard();
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::debugPause( bool _pause )
    {
        m_debugPause = _pause;
    }
}

