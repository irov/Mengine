#include "Application.h"

#include "Interface/CodecServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/RenderServiceInterface.h"
#include "Interface/SoundSystemInterface.h"
#include "Interface/SoundServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/PickerServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/AccountInterface.h"
#include "Interface/GameServiceInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/WatchdogServiceInterface.h"
#include "Interface/GraveyardServiceInterface.h"
#include "Interface/PackageServiceInterface.h"
#include "Interface/TimelineServiceInterface.h"
#include "Interface/TimepipeServiceInterface.h"
#include "Interface/ModuleServiceInterface.h"
#include "Interface/ChronometerServiceInterface.h"
#include "Interface/SettingsServiceInterface.h"
#include "Interface/UpdateServiceInterface.h"
#include "Interface/PlatformServiceInterface.h"
#include "Interface/LanguageServiceInterface.h"
#include "Interface/RenderPlatformInterface.h"
#include "Interface/AnalyticsServiceInterface.h"
#include "Interface/TimerServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/LifecycleServiceInterface.h"

#include "Kernel/MixerBoolean.h"
#include "Kernel/ConfigurationHelper.h"

#include "Isometric.h"
#include "HotSpotPolygon.h"
#include "HotSpotGlobal.h"
#include "HotSpotCircle.h"
#include "HotSpotImage.h"
#include "HotSpotResourceShape.h"
#include "HotSpotSurface.h"
#include "Gyroscope.h"
#include "Point.h"
#include "Line.h"
#include "SoundEmitter.h"
#include "Grid2D.h"
#include "MotionTrail2D.h"
#include "TextField.h"
#include "Meshget.h"
#include "Mesh3D.h"
#include "ResourceMesh3D.h"
#include "SkinnedMesh3D.h"
#include "ResourceSkinnedMesh3D.h"
#include "DirectionalLight3D.h"
#include "PointLight3D.h"
#include "BlobShadow3D.h"
#include "PostProcessFx3D.h"
#include "Layer2D.h"
#include "VirtualArea.h"
#include "Window.h"
#include "Landscape2D.h"
#include "TileMap2D.h"
#include "ResourceMusic.h"
#include "ResourceFile.h"
#include "ResourceWindow.h"
#include "ResourceHIT.h"
#include "ResourceShape.h"
#include "ResourceCursorICO.h"
#include "ResourceCursorSystem.h"

#include "EntityPrototypeGenerator.h"
#include "ScenePrototypeGenerator.h"

#include "Kernel/ShapeCircle.h"
#include "Kernel/ShapePacMan.h"
#include "Kernel/ShapeQuadSize.h"
#include "Kernel/ShapeQuadFixed.h"
#include "Kernel/ShapeQuadFlex.h"
#include "Kernel/ShapeNinePatch.h"
#include "Kernel/ShapeThreePatchHorizontal.h"
#include "Kernel/ShapeThreePatchVertical.h"
#include "Kernel/SpriteSlice3Horizontal.h"
#include "Kernel/SpriteSlice3Vertical.h"
#include "Kernel/SpriteSlice9.h"
#include "Kernel/SurfaceSound.h"
#include "Kernel/SurfaceImage.h"
#include "Kernel/SurfaceImageSequence.h"
#include "Kernel/SurfaceTrackMatte.h"
#include "Kernel/SurfaceSolidColor.h"
#include "Kernel/ResourceImageSequence.h"
#include "Kernel/ResourceImageData.h"
#include "Kernel/ResourceImageSolid.h"
#include "Kernel/ResourceImageDefault.h"
#include "Kernel/ResourceImageEmpty.h"
#include "Kernel/ResourceImageSubstract.h"
#include "Kernel/ResourceImageSubstractRGBAndAlpha.h"
#include "Kernel/ResourceSound.h"
#include "Kernel/Logger.h"
#include "Kernel/Scene.h"
#include "Kernel/FactoryDefault.h"
#include "Kernel/TagsHelper.h"
#include "Kernel/Entity.h"
#include "Kernel/Interender.h"
#include "Kernel/MatrixProxy.h"
#include "Kernel/FileContent.h"
#include "Kernel/RenderResolution.h"
#include "Kernel/RenderViewport.h"
#include "Kernel/RenderViewportDefault.h"
#include "Kernel/RenderScissor.h"
#include "Kernel/RenderCameraOrthogonal.h"
#include "Kernel/RenderCameraProjection.h"
#include "Kernel/RenderCameraOrthogonalTarget.h"
#include "Kernel/RenderCameraOrthogonalDefault.h"
#include "Kernel/BasePrototypeGenerator.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionUtf8.h"
#include "Kernel/DefaultPrototypeGenerator.h"
#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/ResourcePrototypeGenerator.h"
#include "Kernel/SurfacePrototypeGenerator.h"
#include "Kernel/ImageDecoderMemory.h"
#include "Kernel/ImageDecoderArchive.h"
#include "Kernel/DecoderFactory.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/EntityEventable.h"
#include "Kernel/BaseAffectorHub.h"
#include "Kernel/String.h"
#include "Kernel/Stringstream.h"
#include "Kernel/ViewportHelper.h"
#include "Kernel/BuildMode.h"
#include "Kernel/FilePathDateTimeHelper.h"
#include "Kernel/ProfilerHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/TimestampHelper.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/PrototypeHelper.h"

#include "Config/StdString.h"
#include "Config/StdIntTypes.h"
#include "Config/StdMath.h"

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
        , m_focus( true )
        , m_updateGraceful( true )
        , m_nopause( false )
        , m_pauseUpdatingTime( -1.f )
        , m_bits( 0 )
        , m_fullscreen( false )
        , m_nofullscreen( false )
        , m_alwaysfullscreen( false )
        , m_vsync( false )
        , m_FSAAType( 0 )
        , m_FSAAQuality( 0 )
        , m_updateRevision( 0 )
        , m_fixedContentResolution( false )
        , m_fixedDisplayResolution( false )
        , m_fixedViewportResolution( false )
        , m_createRenderWindow( false )
        , m_maxFrameTime( 100.f )
        , m_projectVersion( 0 )
        , m_invalidateVsync( true )
        , m_cursorMode( false )
        , m_invalidateCursorMode( true )
        , m_inputMouseButtonEventBlock( false )
        , m_debugPause( false )
        , m_initailizeGame( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Application::~Application()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const ServiceRequiredList & Application::requiredServices() const
    {
        static const ServiceRequiredList required = {
            SERVICE_ID( PrototypeServiceInterface ),
            SERVICE_ID( CodecServiceInterface ),
            SERVICE_ID( ConfigServiceInterface ),
            SERVICE_ID( PlatformServiceInterface ),
            SERVICE_ID( NotificationServiceInterface ),
            SERVICE_ID( RenderServiceInterface )
        };

        return required;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::_initializeService()
    {
        if( this->registerBaseTypes_() == false )
        {
            LOGGER_ERROR( "invalid register base types" );

            return false;
        }

        if( this->registerBaseNodeTypes_() == false )
        {
            LOGGER_ERROR( "invalid register base node types" );

            return false;
        }

        if( this->registerBaseResourceTypes_() == false )
        {
            LOGGER_ERROR( "invalid register resource types" );

            return false;
        }

        if( this->registerEntityGenerator_() == false )
        {
            LOGGER_ERROR( "invalid register entity generator" );

            return false;
        }

        if( this->registerSceneGenerator_() == false )
        {
            LOGGER_ERROR( "invalid register scene generator" );

            return false;
        }

        Helper::registerDecoder<ImageDecoderMemory>( STRINGIZE_STRING_LOCAL( "memoryImage" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerDecoder<ImageDecoderArchive>( STRINGIZE_STRING_LOCAL( "archiveImage" ), MENGINE_DOCUMENT_FACTORABLE );

        PathString Project_Company = CONFIG_VALUE_PATHSTRING( "Project", "Company", "UNKNOWN" );
        PathString Project_Name = CONFIG_VALUE_PATHSTRING( "Project", "Name", "UNKNOWN" );

        MENGINE_ASSERTION_VALIDATE_UTF8( Project_Company.c_str(), MENGINE_UNKNOWN_SIZE );
        MENGINE_ASSERTION_VALIDATE_UTF8( Project_Name.c_str(), MENGINE_UNKNOWN_SIZE );

        m_companyName.assign( Project_Company.c_str() );
        m_projectName.assign( Project_Name.c_str() );

        m_projectCodename = CONFIG_VALUE_CONSTSTRING( "Project", "Codename", ConstString::none() );
        m_projectVersion = CONFIG_VALUE_INTEGER( "Project", "Version", 0 );

        m_updateFreezer = PROTOTYPE_SERVICE()
            ->generatePrototype( ConstString::none(), MixerBoolean::getFactorableType(), MENGINE_DOCUMENT_FACTORABLE );

        m_renderFreezer = PROTOTYPE_SERVICE()
            ->generatePrototype( ConstString::none(), MixerBoolean::getFactorableType(), MENGINE_DOCUMENT_FACTORABLE );

        m_contentResolution = CONFIG_VALUE_RESOLUTION( "Game", "ContentResolution", Resolution( 1024, 768 ) );
        m_fixedContentResolution = CONFIG_VALUE_BOOLEAN( "Game", "FixedContentResolution", true );
        m_fixedDisplayResolution = CONFIG_VALUE_BOOLEAN( "Game", "FixedDisplayResolution", true );
        m_fixedViewportResolution = CONFIG_VALUE_BOOLEAN( "Game", "FixedViewportResolution", true );

        CONFIG_VALUES( "Game", "AspectRatioViewport", &m_aspectRatioViewports );

        if( HAS_OPTION( "locale" ) == true )
        {
            const Char * option_locale = GET_OPTION_VALUE( "locale", "en" );

            m_locale = Helper::stringizeString( option_locale );
        }
        else if( HAS_CONFIG( "Locale", "Force" ) == true )
        {
            m_locale = CONFIG_VALUE_CONSTSTRING( "Locale", "Force", ConstString::none() );
        }

#if defined(MENGINE_PLATFORM_IOS)
#elif defined(MENGINE_PLATFORM_ANDROID)
#else
        m_windowResolution = CONFIG_VALUE_RESOLUTION( "Window", "Size", Resolution( 1024, 768 ) );
#endif

        m_bits = CONFIG_VALUE_INTEGER( "Window", "Bits", 32U );
        m_fullscreen = CONFIG_VALUE_BOOLEAN( "Window", "Fullscreen", true );
        m_nofullscreen = CONFIG_VALUE_BOOLEAN( "Window", "NoFullscreen", false );
        m_alwaysfullscreen = CONFIG_VALUE_BOOLEAN( "Window", "AlwaysFullscreen", false );
        m_FSAAType = CONFIG_VALUE_INTEGER( "Window", "FSAAType", 0 );
        m_FSAAQuality = CONFIG_VALUE_INTEGER( "Window", "FSAAQuality", 0 );
        m_vsync = CONFIG_VALUE_BOOLEAN( "Window", "VSync", true );

        if( HAS_OPTION( "novsync" ) == true )
        {
            m_vsync = false;
        }

        if( HAS_OPTION( "maxfps" ) == true )
        {
            m_vsync = false;
        }

        if( HAS_OPTION( "nofullscreen" ) == true )
        {
            m_nofullscreen = true;
        }

        if( HAS_OPTION( "fullscreen" ) == true )
        {
            m_alwaysfullscreen = true;
        }

        if( HAS_OPTION( "author" ) == true || HAS_OPTION( "support" ) == true )
        {
            PLATFORM_SERVICE()
                ->messageBox( "Mengine", "author: IROV\nemail for support/feedbacks/improvement request and suggestions: irov13@mail.ru" );
        }

        const Char * engineGitURL = Helper::getEngineGitURL();
        const Char * engineGitSHA1 = Helper::getEngineGitSHA1();
        const Char * engineGitBranch = Helper::getEngineGitBranch();
        const Char * engineGitDate = Helper::getEngineGitDate();

        if( HAS_OPTION( "engineinfo" ) == true )
        {
            PLATFORM_SERVICE()
                ->messageBox( "Mengine", "url '%s'\ncommit '%s'\nbranch '%s'\ndate '%s'"
                    , engineGitURL
                    , engineGitSHA1
                    , engineGitBranch
                    , engineGitDate
                );
        }

        LOGGER_MESSAGE( "mengine URL '%s'", engineGitURL );
        LOGGER_MESSAGE( "mengine commit '%s'", engineGitSHA1 );
        LOGGER_MESSAGE( "mengine branch '%s'", engineGitBranch );
        LOGGER_MESSAGE( "mengine date '%s'", engineGitDate );

        const Char * buildVersion = Helper::getBuildVersion();
        uint32_t buildVersionNumber = Helper::getBuildVersionNumber();
        const Char * buildUserName = Helper::getBuildUsername();
        const Char * buildDate = Helper::getBuildDate();
        const Char * buildProjectName = Helper::getBuildSolutionName();
        uint64_t buildNumber = Helper::getBuildNumber();

        if( HAS_OPTION( "buildinfo" ) == true )
        {
            PLATFORM_SERVICE()
                ->messageBox( "Mengine", "build version '%s' [0x%08x] user '%s' date '%s' project '%s' number %" MENGINE_PRIu64 ""
                    , buildVersion
                    , buildVersionNumber
                    , buildUserName
                    , buildDate
                    , buildProjectName
                    , buildNumber
                );
        }

        LOGGER_MESSAGE( "build version '%s' [0x%08x] user '%s' timestamp '%s' project '%s' build number %" MENGINE_PRIu64 ""
            , buildVersion
            , buildVersionNumber
            , buildUserName
            , buildDate
            , buildProjectName
            , buildNumber
        );

        LOGGER_MESSAGE( "application company '%s' project '%s' version [%u] locale '%s'"
            , m_companyName.c_str()
            , m_projectName.c_str()
            , m_projectVersion
            , m_locale.c_str()
        );

        bool OPTION_novideo = HAS_OPTION( "novideo" );

        if( OPTION_novideo == true )
        {
            this->setVideoEnable( false );
        }

        bool OPTION_nopause = HAS_OPTION( "nopause" );
        bool CONFIG_nopause = Helper::isNopause();

        this->setNopause( OPTION_nopause || CONFIG_nopause );

        m_cursorMode = CONFIG_VALUE_BOOLEAN( "Platform", "Cursor", false );

        ConstString Engine_RenderPipeline = CONFIG_VALUE_CONSTSTRING( "Engine", "RenderPipeline", STRINGIZE_STRING_LOCAL( "Batch" ) );

        RenderPipelineInterfacePtr renderPipeline = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "RenderPipeline" ), Engine_RenderPipeline, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( renderPipeline, "invalid create render pipeline '%s'"
            , Engine_RenderPipeline.c_str()
        );

        if( renderPipeline->initialize() == false )
        {
            LOGGER_ERROR( "render pipeline '%s' invalid initialize"
                , Engine_RenderPipeline.c_str()
            );

            return false;
        }

        m_renderPipeline = renderPipeline;

        PLATFORM_SERVICE()
            ->setSafeAreaViewportChangedCallback( [this]( const Viewport & _viewport )
            {
                this->setSafeAreaViewport( _viewport );
            } );

        Viewport safeAreaViewport;
        if( PLATFORM_SERVICE()
            ->getSafeAreaViewport( &safeAreaViewport ) == true )
        {
            m_safeAreaViewport = safeAreaViewport;
        }

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_ENGINE_PREPARE_FINALIZE, &Application::notifyEnginePrepareFinalize_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::_finalizeService()
    {
        PLATFORM_SERVICE()
            ->setSafeAreaViewportChangedCallback( nullptr );

        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_ENGINE_PREPARE_FINALIZE );

        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "memoryImage" ) );
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "archiveImage" ) );

        m_updateFreezer = nullptr;
        m_renderFreezer = nullptr;

        this->unregisterBaseTypes_();
        this->unregisterBaseNodeTypes_();
        this->unregisterBaseResourceTypes_();
        this->unregisterEntityGenerator_();
        this->unregisterSceneGenerator_();
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::_stopService()
    {
        if( SERVICE_IS_INITIALIZE( PlayerServiceInterface ) == true )
        {
            PLAYER_SERVICE()
                ->finalizeRenderResources();
        }

        if( m_renderPipeline != nullptr )
        {
            m_renderPipeline->finalize();
            m_renderPipeline = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::notifyEnginePrepareFinalize_()
    {
        if( m_resourceWhitePixel != nullptr )
        {
            m_resourceWhitePixel->setTexture( 0, nullptr );

            RESOURCE_SERVICE()
                ->removeResource( m_resourceWhitePixel );

            m_resourceWhitePixel = nullptr;
        }

        if( m_cursorResource != nullptr )
        {
            m_cursorResource->release();
            m_cursorResource = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::registerBaseTypes_()
    {
        LOGGER_INFO( "system", "register base generator..." );

        if( Helper::addDefaultPrototype<BaseAffectorHub, 128>( ConstString::none(), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addDefaultPrototype<RenderResolution, 8>( ConstString::none(), MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::unregisterBaseTypes_()
    {
        LOGGER_INFO( "system", "unregister base generator..." );

        Helper::removePrototype<BaseAffectorHub>( ConstString::none() );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::registerBaseNodeTypes_()
    {
        LOGGER_INFO( "system", "register node generator..." );

#define NODE_FACTORY( Type )\
        if( Helper::addNodePrototype<Type, 128>( MENGINE_DOCUMENT_FACTORABLE ) == false )\
        {\
            return false;\
        }

        NODE_FACTORY( Entity );
        NODE_FACTORY( Scene );
        NODE_FACTORY( Gyroscope );
        NODE_FACTORY( Isometric );
        NODE_FACTORY( MatrixProxy );
        NODE_FACTORY( HotSpotPolygon );
        NODE_FACTORY( HotSpotGlobal );
        NODE_FACTORY( HotSpotCircle );
        NODE_FACTORY( HotSpotImage );
        NODE_FACTORY( HotSpotResourceShape );
        NODE_FACTORY( HotSpotSurface );
        NODE_FACTORY( Point );
        NODE_FACTORY( Line );
        NODE_FACTORY( SoundEmitter );
        NODE_FACTORY( Grid2D );
        NODE_FACTORY( MotionTrail2D );
        NODE_FACTORY( TextField );
        NODE_FACTORY( Meshget );
        NODE_FACTORY( Mesh3D );
        NODE_FACTORY( SkinnedMesh3D );
        NODE_FACTORY( DirectionalLight3D );
        NODE_FACTORY( PointLight3D );
        NODE_FACTORY( BlobShadow3D );
        NODE_FACTORY( PostProcessFx3D );
        NODE_FACTORY( Interender );
        NODE_FACTORY( Layer2D );
        NODE_FACTORY( VirtualArea );
        NODE_FACTORY( Landscape2D );
        NODE_FACTORY( TileMap2D );
        NODE_FACTORY( RenderViewport );
        NODE_FACTORY( RenderViewportDefault );
        NODE_FACTORY( RenderScissor );
        NODE_FACTORY( RenderCameraOrthogonal );
        NODE_FACTORY( RenderCameraProjection );
        NODE_FACTORY( RenderCameraOrthogonalTarget );
        NODE_FACTORY( RenderCameraOrthogonalDefault );
        NODE_FACTORY( Window );
        NODE_FACTORY( ShapeCircle );
        NODE_FACTORY( ShapePacMan );
        NODE_FACTORY( ShapeQuadSize );
        NODE_FACTORY( ShapeQuadFixed );
        NODE_FACTORY( ShapeQuadFlex );
        NODE_FACTORY( ShapeNinePatch );
        NODE_FACTORY( ShapeThreePatchHorizontal );
        NODE_FACTORY( ShapeThreePatchVertical );
        NODE_FACTORY( SpriteSlice3Horizontal );
        NODE_FACTORY( SpriteSlice3Vertical );
        NODE_FACTORY( SpriteSlice9 );

#undef NODE_FACTORY

#define SURFACE_FACTORY(Type)\
        if( Helper::addSurfacePrototype<Type, 128>( MENGINE_DOCUMENT_FACTORABLE ) == false )\
        {\
            return false;\
        }

        SURFACE_FACTORY( SurfaceSound );
        SURFACE_FACTORY( SurfaceImage );
        SURFACE_FACTORY( SurfaceImageSequence );
        SURFACE_FACTORY( SurfaceTrackMatte );
        SURFACE_FACTORY( SurfaceSolidColor );

#undef SURFACE_FACTORY

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::unregisterBaseNodeTypes_()
    {
        LOGGER_INFO( "system", "unregister node generator..." );

#define NODE_FACTORY( Type )\
        Helper::removeNodePrototype<Type>()

        NODE_FACTORY( Entity );
        NODE_FACTORY( Scene );
        NODE_FACTORY( Gyroscope );
        NODE_FACTORY( Isometric );
        NODE_FACTORY( MatrixProxy );
        NODE_FACTORY( HotSpotPolygon );
        NODE_FACTORY( HotSpotGlobal );
        NODE_FACTORY( HotSpotCircle );
        NODE_FACTORY( HotSpotImage );
        NODE_FACTORY( HotSpotResourceShape );
        NODE_FACTORY( HotSpotSurface );
        NODE_FACTORY( Point );
        NODE_FACTORY( Line );
        NODE_FACTORY( SoundEmitter );
        NODE_FACTORY( Grid2D );
        NODE_FACTORY( MotionTrail2D );
        NODE_FACTORY( TextField );
        NODE_FACTORY( Meshget );
        NODE_FACTORY( Mesh3D );
        NODE_FACTORY( SkinnedMesh3D );
        NODE_FACTORY( DirectionalLight3D );
        NODE_FACTORY( PointLight3D );
        NODE_FACTORY( BlobShadow3D );
        NODE_FACTORY( PostProcessFx3D );
        NODE_FACTORY( Interender );
        NODE_FACTORY( Layer2D );
        NODE_FACTORY( VirtualArea );
        NODE_FACTORY( Landscape2D );
        NODE_FACTORY( TileMap2D );
        NODE_FACTORY( RenderViewport );
        NODE_FACTORY( RenderViewportDefault );
        NODE_FACTORY( RenderScissor );
        NODE_FACTORY( RenderCameraOrthogonal );
        NODE_FACTORY( RenderCameraProjection );
        NODE_FACTORY( RenderCameraOrthogonalTarget );
        NODE_FACTORY( RenderCameraOrthogonalDefault );
        NODE_FACTORY( Window );
        NODE_FACTORY( ShapeCircle );
        NODE_FACTORY( ShapePacMan );
        NODE_FACTORY( ShapeQuadSize );
        NODE_FACTORY( ShapeQuadFixed );
        NODE_FACTORY( ShapeQuadFlex );
        NODE_FACTORY( ShapeNinePatch );
        NODE_FACTORY( ShapeThreePatchHorizontal );
        NODE_FACTORY( ShapeThreePatchVertical );
        NODE_FACTORY( SpriteSlice3Horizontal );
        NODE_FACTORY( SpriteSlice3Vertical );
        NODE_FACTORY( SpriteSlice9 );

#undef NODE_FACTORY

        Helper::removePrototype<RenderResolution>( ConstString::none() );


#define SURFACE_FACTORY(Type)\
        Helper::removeSurfacePrototype<Type>()

        SURFACE_FACTORY( SurfaceSound );
        SURFACE_FACTORY( SurfaceImage );
        SURFACE_FACTORY( SurfaceImageSequence );
        SURFACE_FACTORY( SurfaceTrackMatte );
        SURFACE_FACTORY( SurfaceSolidColor );

#undef SURFACE_FACTORY
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::registerEntityGenerator_()
    {
        LOGGER_INFO( "system", "register entity generator..." );

        EntityPrototypeGeneratorPtr generator = Helper::makeFactorableUnique<EntityPrototypeGenerator>( MENGINE_DOCUMENT_FACTORABLE );

        if( PROTOTYPE_SERVICE()
            ->addPrototype( ConstString::none(), Entity::getFactorableType(), generator ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::registerSceneGenerator_()
    {
        LOGGER_INFO( "system", "register scene generator..." );

        PrototypeGeneratorInterfacePtr generator = Helper::makeFactorableUnique<ScenePrototypeGenerator>( MENGINE_DOCUMENT_FACTORABLE );

        if( PROTOTYPE_SERVICE()
            ->addPrototype( ConstString::none(), Scene::getFactorableType(), generator ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::unregisterEntityGenerator_()
    {
        LOGGER_INFO( "system", "unregister entity generator..." );

        Helper::removePrototype<Entity>( ConstString::none() );
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::unregisterSceneGenerator_()
    {
        LOGGER_INFO( "system", "unregister scene generator..." );

        Helper::removePrototype<Scene>( ConstString::none() );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::registerBaseResourceTypes_()
    {
        LOGGER_INFO( "system", "register resource type..." );

#define ADD_PROTOTYPE( Type )\
        if( Helper::addResourcePrototype<Type, 128>(MENGINE_DOCUMENT_FACTORABLE) == false )\
        {\
            return false;\
        }

        ADD_PROTOTYPE( ResourceMusic );
        ADD_PROTOTYPE( ResourceImage );
        ADD_PROTOTYPE( ResourceImageSequence );
        ADD_PROTOTYPE( ResourceImageData );
        ADD_PROTOTYPE( ResourceImageDefault );
        ADD_PROTOTYPE( ResourceImageEmpty );
        ADD_PROTOTYPE( ResourceImageSubstract );
        ADD_PROTOTYPE( ResourceImageSubstractRGBAndAlpha );
        ADD_PROTOTYPE( ResourceImageSolid );
        ADD_PROTOTYPE( ResourceSound );
        ADD_PROTOTYPE( ResourceFile );
        ADD_PROTOTYPE( ResourceWindow );
        ADD_PROTOTYPE( ResourceHIT );
        ADD_PROTOTYPE( ResourceShape );
        ADD_PROTOTYPE( ResourceCursorICO );
        ADD_PROTOTYPE( ResourceCursorSystem );
        ADD_PROTOTYPE( ResourceMesh3D );
        ADD_PROTOTYPE( ResourceSkinnedMesh3D );

#undef ADD_PROTOTYPE

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::unregisterBaseResourceTypes_()
    {
        LOGGER_INFO( "system", "initializing resource type..." );

#define REMOVE_PROTOTYPE( Type )\
        PROTOTYPE_SERVICE()\
            ->removePrototype( Resource::getFactorableType(), Type::getFactorableType(), nullptr )

        REMOVE_PROTOTYPE( ResourceMusic );
        REMOVE_PROTOTYPE( ResourceImage );
        REMOVE_PROTOTYPE( ResourceImageSequence );
        REMOVE_PROTOTYPE( ResourceImageData );
        REMOVE_PROTOTYPE( ResourceImageDefault );
        REMOVE_PROTOTYPE( ResourceImageEmpty );
        REMOVE_PROTOTYPE( ResourceImageSubstract );
        REMOVE_PROTOTYPE( ResourceImageSubstractRGBAndAlpha );
        REMOVE_PROTOTYPE( ResourceImageSolid );
        REMOVE_PROTOTYPE( ResourceSound );
        REMOVE_PROTOTYPE( ResourceFile );
        REMOVE_PROTOTYPE( ResourceWindow );
        REMOVE_PROTOTYPE( ResourceHIT );
        REMOVE_PROTOTYPE( ResourceShape );
        REMOVE_PROTOTYPE( ResourceCursorICO );
        REMOVE_PROTOTYPE( ResourceCursorSystem );
        REMOVE_PROTOTYPE( ResourceMesh3D );
        REMOVE_PROTOTYPE( ResourceSkinnedMesh3D );

#undef REMOVE_PROTOTYPE
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::createRenderWindow()
    {
        bool vsync = this->getVSync();
        bool fullscreen = this->getFullscreenMode();

        Resolution windowResolution;
        if( this->calcWindowResolution( fullscreen, &windowResolution ) == false )
        {
            LOGGER_ERROR( "invalid calc window resolution" );

            return false;
        }

        LOGGER_INFO( "system", "set current window resolution [%u:%u] %s"
            , windowResolution.getWidth()
            , windowResolution.getHeight()
            , fullscreen ? "fullscreen" : "window"
        );

        m_currentWindowResolution = windowResolution;

        float renderViewportAspect;
        Viewport renderViewport;
        this->calcRenderViewport_( &renderViewportAspect, &renderViewport );

        LOGGER_INFO( "system", "set render viewport [%f %f - %f %f]"
            , renderViewport.begin.x
            , renderViewport.begin.y
            , renderViewport.getWidth()
            , renderViewport.getHeight()
        );

        m_renderViewport = renderViewport;

        bool result = RENDER_SERVICE()
            ->createRenderWindow( m_currentWindowResolution
                , m_renderViewport
                , vsync
                , m_bits
                , fullscreen
                , m_FSAAType
                , m_FSAAQuality
            );

        if( result == false )
        {
            LOGGER_ERROR( "failed to create render window" );

            return false;
        }

        m_createRenderWindow = true;

        PLAYER_SERVICE()
            ->initializeRenderResources();

        NOTIFICATION_NOTIFY( NOTIFICATOR_CREATE_RENDER_WINDOW );

        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION, fullscreen, m_currentWindowResolution );

        ResourceCook resourceWhitePixelCook;
        resourceWhitePixelCook.name = STRINGIZE_STRING_LOCAL( "WhitePixel" );
        resourceWhitePixelCook.type = ResourceImage::getFactorableType();
        resourceWhitePixelCook.groupCache = false;
        resourceWhitePixelCook.keep = false;

        ResourceImagePtr resourceWhitePixel = RESOURCE_SERVICE()
            ->createResource( resourceWhitePixelCook, MENGINE_DOCUMENT_FACTORABLE );

        resourceWhitePixel->setMaxSize( mt::vec2f( 2.f, 2.f ) );
        resourceWhitePixel->setSize( mt::vec2f( 2.f, 2.f ) );
        resourceWhitePixel->setPow2( true );

        const RenderTextureInterfacePtr & whiteTexture = RENDER_SERVICE()
            ->getWhiteTexture();

        resourceWhitePixel->setTexture( 0, whiteTexture );

        if( resourceWhitePixel->initialize() == false )
        {
            LOGGER_ERROR( "invalid initialize resource white pixel" );

            return false;
        }

        m_resourceWhitePixel = resourceWhitePixel;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::initializeResources( const FileGroupInterfacePtr & _fileGroup, const VectorFilePath & _packagesPaths )
    {
        for( const FilePath & packagePath : _packagesPaths )
        {
            ContentInterfacePtr content = Helper::makeFileContent( _fileGroup, packagePath, MENGINE_DOCUMENT_FACTORABLE );

            if( PACKAGE_SERVICE()
                ->loadPackages( content, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                LOGGER_ERROR( "invalid load package '%s'"
                    , Helper::getContentFullPath( content ).c_str()
                );

                return false;
            }
        }

        const Tags & platformTags = PLATFORM_SERVICE()
            ->getPlatformTags();

        LOGGER_INFO( "system", "load game resource tags '%s'"
            , Helper::tagsToString( platformTags ).c_str()
        );

        if( m_locale.empty() == true )
        {
            VectorConstString locales;
            PACKAGE_SERVICE()
                ->getLocales( platformTags, &locales );

            if( LANGUAGE_SERVICE()
                ->findUserLocale( locales, &m_locale ) == false )
            {
                const ConstString & defaultLocale = PACKAGE_SERVICE()
                    ->getDefaultLocale();

                m_locale = defaultLocale;
            }
        }

        LOGGER_INFO( "system", "setup locale '%s'"
            , m_locale.c_str()
        );

        if( PACKAGE_SERVICE()
            ->enablePackages( m_locale, platformTags ) == false )
        {
            LOGGER_ERROR( "invalid enable packages for locale '%s' platform '%s'!"
                , m_locale.c_str()
                , Helper::tagsToString( platformTags ).c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::finalizeResources()
    {
        PACKAGE_SERVICE()
            ->unloadPackages();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::initializeGame()
    {
        m_initailizeGame = true;

        if( NOTIFICATION_NOTIFY( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_GAME ) == false )
        {
            return false;
        }

        if( GAME_SERVICE()
            ->preparation() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::finalizeGame()
    {
        if( m_initailizeGame == false )
        {
            return;
        }

        m_initailizeGame = false;

        NOTIFICATION_NOTIFY( NOTIFICATOR_BOOTSTRAPPER_FINALIZE_GAME );
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::handleFocus( bool _focus )
    {
        if( SERVICE_IS_INITIALIZE( ModuleServiceInterface ) == true )
        {
            MODULE_SERVICE()
                ->handleFocus( _focus );
        }

        if( SERVICE_IS_INITIALIZE( GameServiceInterface ) == true )
        {
            GAME_SERVICE()
                ->handleFocus( _focus );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::handleKeyEvent( const InputKeyEvent & _event )
    {
        if( MODULE_SERVICE()
            ->handleKeyEvent( _event ) == true )
        {
            return true;
        }

        if( GAME_SERVICE()
            ->handleKeyEvent( _event ) == true )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::handleTextEvent( const InputTextEvent & _event )
    {
        if( MODULE_SERVICE()
            ->handleTextEvent( _event ) == true )
        {
            return true;
        }

        if( GAME_SERVICE()
            ->handleTextEvent( _event ) == true )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::handleAccelerometerEvent( const InputAccelerometerEvent & _event )
    {
        if( MODULE_SERVICE()
            ->handleAccelerometerEvent( _event ) == true )
        {
            return true;
        }

        if( GAME_SERVICE()
            ->handleAccelerometerEvent( _event ) == true )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::handleMouseButtonEvent( const InputMouseButtonEvent & _event )
    {
        if( m_inputMouseButtonEventBlock == true )
        {
            return false;
        }

        if( MODULE_SERVICE()
            ->handleMouseButtonEvent( _event ) == true )
        {
            return true;
        }

        if( GAME_SERVICE()
            ->handleMouseButtonEvent( _event ) == true )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::handleMouseButtonEventBegin( const InputMouseButtonEvent & _event )
    {
        if( m_inputMouseButtonEventBlock == true )
        {
            return false;
        }

        if( MODULE_SERVICE()
            ->handleMouseButtonEventBegin( _event ) == true )
        {
            return true;
        }

        if( GAME_SERVICE()
            ->handleMouseButtonEventBegin( _event ) == true )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::handleMouseButtonEventEnd( const InputMouseButtonEvent & _event )
    {
        if( m_inputMouseButtonEventBlock == true )
        {
            return false;
        }

        if( MODULE_SERVICE()
            ->handleMouseButtonEventEnd( _event ) == true )
        {
            return true;
        }

        if( GAME_SERVICE()
            ->handleMouseButtonEventEnd( _event ) == true )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::handleMouseMove( const InputMouseMoveEvent & _event )
    {
        if( MODULE_SERVICE()
            ->handleMouseMove( _event ) == true )
        {
            return true;
        }

        if( GAME_SERVICE()
            ->handleMouseMove( _event ) == true )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::handleMouseWheel( const InputMouseWheelEvent & _event )
    {
        if( MODULE_SERVICE()
            ->handleMouseWheel( _event ) == true )
        {
            return true;
        }

        if( GAME_SERVICE()
            ->handleMouseWheel( _event ) == true )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::handleMouseEnter( const InputMouseEnterEvent & _event )
    {
        if( MODULE_SERVICE()
            ->handleMouseEnter( _event ) == true )
        {
            return true;
        }

        if( GAME_SERVICE()
            ->handleMouseEnter( _event ) == true )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::handleMouseLeave( const InputMouseLeaveEvent & _event )
    {
        MODULE_SERVICE()
            ->handleMouseLeave( _event );

        GAME_SERVICE()
            ->handleMouseLeave( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::quit()
    {
        PLATFORM_SERVICE()
            ->closeWindow();

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
    void Application::setFocus( bool _focus )
    {
        if( m_focus == _focus )
        {
            return;
        }

        m_focus = _focus;

        bool updateFrozen = this->isUpdateFrozen();

        LOGGER_INFO( "system", "focus [%u] (frozen %u)"
            , m_focus
            , updateFrozen
        );

        if( updateFrozen == true )
        {
            return;
        }

        this->handleFocus( m_focus );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::isFocus() const
    {
        return m_focus;
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::setUpdateFreeze( const ConstString & _owner, bool _freeze )
    {
        m_updateFreezer->setValue( _owner, _freeze );

        bool updateFrozen = this->isUpdateFrozen();

        LOGGER_INFO( "system", "frozen '%s' [%u] (focus %u)"
            , _owner.c_str()
            , updateFrozen
            , m_focus
        );

        if( updateFrozen == true )
        {
            return;
        }

        if( SERVICE_IS_INITIALIZE( GameServiceInterface ) == true )
        {
            GAME_SERVICE()
                ->setFocus( m_focus );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::isUpdateFrozen() const
    {
        bool updateFrozen = m_updateFreezer->mixValue();

        return updateFrozen;
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::setRenderFreeze( const ConstString & _owner, bool _freeze )
    {
        m_renderFreezer->setValue( _owner, _freeze );

        bool renderFrozen = this->isRenderFrozen();

        LOGGER_INFO( "system", "render frozen '%s' [%u]"
            , _owner.c_str()
            , renderFrozen
        );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::isRenderFrozen() const
    {
        bool renderFrozen = m_renderFreezer->mixValue();

        return renderFrozen;
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
    void Application::tick( float _frameTime )
    {
        MENGINE_PROFILER_CATEGORY();

        if( this->beginUpdate_() == false )
        {
            if( m_pauseUpdatingTime < 0.f )
            {
                m_pauseUpdatingTime = _frameTime;
            }

            this->endUpdate_();

            return;
        }

        float frameTime = _frameTime;

        if( m_pauseUpdatingTime >= 0.f )
        {
            frameTime = m_pauseUpdatingTime;
            m_pauseUpdatingTime = -1.f;
        }

        this->tickTimeline_( frameTime );

        this->endUpdate_();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::beginUpdate_()
    {
        MENGINE_PROFILER_CATEGORY();

        this->updateNotification();

        LIFECYCLE_SERVICE()
            ->preUpdate();

        if( this->isUpdateFrozen() == true )
        {
            return false;
        }

        bool updatePaused = m_nopause == false && m_focus == false;

        if( updatePaused == true && m_updateGraceful == false )
        {
            return false;
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_BEGIN_UPDATE );

        LIFECYCLE_SERVICE()
            ->update();

        if( m_nopause == false )
        {
            m_updateGraceful = m_focus;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::tickTimeline_( float _time )
    {
        MENGINE_PROFILER_CATEGORY();

        ++m_updateRevision;

        float time = _time;

        if( m_debugPause == true )
        {
            return;
        }

        if( _time > m_maxFrameTime )
        {
            time = m_maxFrameTime;
        }

        float timeFactor = TIMELINE_SERVICE()
            ->calcTimeFactor();

        time *= timeFactor;

        float current = TIMELINE_SERVICE()
            ->getTotalTime();

        UpdateContext applicationContext;
        applicationContext.revision = m_updateRevision;
        applicationContext.current = current;
        applicationContext.time = time;
        applicationContext.offset = 0.f;

        TIMELINE_SERVICE()
            ->begin( &applicationContext );

        TIMEPIPE_SERVICE()
            ->tick( &applicationContext );

        TIMELINE_SERVICE()
            ->end();
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::endUpdate_()
    {
        MENGINE_PROFILER_CATEGORY();

        LIFECYCLE_SERVICE()
            ->postUpdate();

        NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_END_UPDATE );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::render()
    {
        MENGINE_PROFILER_CATEGORY();

        m_renderPipeline->clear();

        bool renderFrozen = this->isRenderFrozen();

        if( renderFrozen == true )
        {
            return false;
        }

        if( RENDER_SERVICE()
            ->beginScene( m_renderPipeline ) == false )
        {
            return false;
        }

        GAME_SERVICE()
            ->render( m_renderPipeline );

        RENDER_SERVICE()
            ->endScene( m_renderPipeline );

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

        if( SERVICE_IS_INITIALIZE( GameServiceInterface ) == true )
        {
            needQuit = GAME_SERVICE()
                ->close();
        }

        if( needQuit == true )
        {
            this->quit();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::turnSound( bool _turn )
    {
        if( _turn == false )
        {
            if( SERVICE_IS_INITIALIZE( SoundServiceInterface ) == true )
            {
                SOUND_SERVICE()
                    ->onTurnStream( false );
            }

            if( SERVICE_IS_INITIALIZE( GameServiceInterface ) == true )
            {
                GAME_SERVICE()
                    ->turnSound( false );
            }

            if( SERVICE_IS_INITIALIZE( SoundServiceInterface ) == true )
            {
                SOUND_SERVICE()
                    ->onTurnSound( false );
            }
        }
        else
        {
            if( SERVICE_IS_INITIALIZE( SoundServiceInterface ) == true )
            {
                SOUND_SERVICE()
                    ->onTurnSound( true );
            }

            if( SERVICE_IS_INITIALIZE( GameServiceInterface ) == true )
            {
                GAME_SERVICE()
                    ->turnSound( true );
            }

            if( SERVICE_IS_INITIALIZE( SoundServiceInterface ) == true )
            {
                SOUND_SERVICE()
                    ->onTurnStream( true );
            }
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_TURN_SOUND, _turn );

        if( SERVICE_IS_INITIALIZE( SoundServiceInterface ) == true )
        {
            SOUND_SERVICE()
                ->updateVolume();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::calcWindowResolution( bool _fullscreen, Resolution * const _windowResolution ) const
    {
        if( _fullscreen == true )
        {
            if( PLATFORM_SERVICE()
                ->getDesktopResolution( _windowResolution ) == false )
            {
                return false;
            }

            return true;
        }

        Resolution maxClientResolution;
        if( PLATFORM_SERVICE()
            ->getMaxClientResolution( &maxClientResolution ) == false )
        {
            LOGGER_ERROR( "invalid get max client resolution" );

            return false;
        }

        LOGGER_INFO( "system", "max client resolution: %u %u"
            , maxClientResolution.getWidth()
            , maxClientResolution.getHeight()
        );

        LOGGER_INFO( "system", "window resolution: %u %u"
            , m_windowResolution.getWidth()
            , m_windowResolution.getHeight()
        );

        float aspect = m_windowResolution.getAspectRatio();

        uint32_t windowResolutionWidth = m_windowResolution.getWidth();
        uint32_t windowResolutionHeight = m_windowResolution.getHeight();

        uint32_t maxClientResolutionWidth = maxClientResolution.getWidth();
        uint32_t maxClientResolutionHeight = maxClientResolution.getHeight();

        if( windowResolutionHeight > maxClientResolutionHeight )
        {
            uint32_t new_width = static_cast<uint32_t>(float( maxClientResolutionHeight ) * aspect + 0.5f);
            uint32_t new_height = maxClientResolutionHeight;

            if( new_width > maxClientResolutionWidth )
            {
                new_width = maxClientResolutionWidth;
                new_height = static_cast<uint32_t>(float( maxClientResolutionWidth ) / aspect + 0.5f);
            }

            _windowResolution->setWidth( new_width );
            _windowResolution->setHeight( new_height );
        }
        else if( windowResolutionWidth > maxClientResolutionWidth )
        {
            uint32_t new_width = maxClientResolutionWidth;
            uint32_t new_height = static_cast<uint32_t>(float( maxClientResolutionWidth ) / aspect + 0.5f);

            if( new_height > maxClientResolutionHeight )
            {
                new_width = static_cast<uint32_t>(float( maxClientResolutionHeight ) * aspect + 0.5f);
                new_height = maxClientResolutionHeight;
            }

            _windowResolution->setWidth( new_width );
            _windowResolution->setHeight( new_height );
        }
        else
        {
            *_windowResolution = m_windowResolution;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const Resolution & Application::getCurrentWindowResolution() const
    {
        MENGINE_ASSERTION_FATAL( m_currentWindowResolution.getWidth() != 0 && m_currentWindowResolution.getHeight() != 0, "current window resolution is invalid" );

        return m_currentWindowResolution;
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::setSafeAreaViewport( const Viewport & _viewport )
    {
        if( m_safeAreaViewport == _viewport )
        {
            return;
        }

        m_safeAreaViewport = _viewport;

        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_SAFE_AREA_VIEWPORT, m_safeAreaViewport );
    }
    //////////////////////////////////////////////////////////////////////////
    const Viewport & Application::getSafeAreaViewport() const
    {
        return m_safeAreaViewport;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::getDisplayCutoutViewport( Viewport * const _viewport ) const
    {
        bool successful = PLATFORM_SERVICE()
            ->getDisplayCutoutViewport( _viewport );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::calcRenderViewport_( float * const _aspect, Viewport * const _viewport ) const
    {
        const Resolution & currentWindowResolution = this->getCurrentWindowResolution();

        float windowWidth = currentWindowResolution.getWidthF();
        float windowHeight = currentWindowResolution.getHeightF();
        float windowAspect = currentWindowResolution.getAspectRatio();

        bool fixedDisplayResolution = this->getFixedDisplayResolution();

        if( fixedDisplayResolution == false )
        {
            *_aspect = windowAspect;

            _viewport->begin.x = 0.f;
            _viewport->begin.y = 0.f;
            _viewport->end.x = windowWidth;
            _viewport->end.y = windowHeight;

            return;
        }

        float contentAspect = m_contentResolution.getAspectRatio();

        float bestAspect;
        if( Helper::findBestAspectViewport( m_aspectRatioViewports, windowAspect, &bestAspect, nullptr ) == false )
        {
            bestAspect = contentAspect;
        }

        float dw = 1.f;
        float dh = windowAspect / bestAspect;

        if( dh > 1.f )
        {
            dw = bestAspect / windowAspect;
            dh = 1.f;
        }

        float areaWidth = StdMath::ceilf( dw * windowWidth );
        float areaHeight = StdMath::ceilf( dh * windowHeight );

        *_aspect = bestAspect;

        _viewport->begin.x = StdMath::ceilf( (windowWidth - areaWidth) * 0.5f );
        _viewport->begin.y = StdMath::ceilf( (windowHeight - areaHeight) * 0.5f );
        _viewport->end.x = _viewport->begin.x + areaWidth;
        _viewport->end.y = _viewport->begin.y + areaHeight;
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::setWindowResolution( const Resolution & _resolution )
    {
        if( m_createRenderWindow == false )
        {
            return;
        }

        if( m_windowResolution == _resolution )
        {
            return;
        }

        LOGGER_INFO( "system", "set change window resolution size %u:%u -> %u:%u aspect %f -> %f"
            , m_windowResolution.getWidth()
            , m_windowResolution.getHeight()
            , _resolution.getWidth()
            , _resolution.getHeight()
            , m_windowResolution.getAspectRatio()
            , _resolution.getAspectRatio()
        );

        m_windowResolution = _resolution;

        this->invalidateWindow_();
    }
    //////////////////////////////////////////////////////////////////////////
    const Resolution & Application::getWindowResolution() const
    {
        return m_windowResolution;
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::setFullscreenMode( bool _fullscreen )
    {
        if( m_nofullscreen == true )
        {
            _fullscreen = false;
        }

        if( m_alwaysfullscreen == true )
        {
            _fullscreen = true;
        }

        if( m_fullscreen == _fullscreen )
        {
            return;
        }

        LOGGER_INFO( "system", "set fullscreen mode: %d"
            , _fullscreen
        );

        m_fullscreen = _fullscreen;

        this->invalidateWindow_();

        bool fullscreen = this->getFullscreenMode();

        GAME_SERVICE()
            ->setFullscreen( fullscreen );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Application::getWindowBits() const
    {
        return m_bits;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t Application::getWindowFSAAType() const
    {
        return m_FSAAType;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t Application::getWindowFSAAQuality() const
    {
        return m_FSAAQuality;
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::invalidateWindow_()
    {
        bool vsync = this->getVSync();
        bool fullscreen = this->getFullscreenMode();

        Resolution windowResolution;
        if( this->calcWindowResolution( fullscreen, &windowResolution ) == false )
        {
            return;
        }

        LOGGER_INFO( "system", "current resolution [%s] [%u %u] aspect: %f"
            , fullscreen == true ? "Fullscreen" : "Window"
            , windowResolution.getWidth()
            , windowResolution.getHeight()
            , windowResolution.getAspectRatio()
        );

        m_currentWindowResolution = windowResolution;

        RENDER_SERVICE()
            ->setVSync( vsync );

        if( PLATFORM_SERVICE()
            ->notifyWindowModeChanged( m_currentWindowResolution, fullscreen ) == false )
        {
            return;
        }

        float renderViewportAspect;
        Viewport renderViewport;
        this->calcRenderViewport_( &renderViewportAspect, &renderViewport );

        LOGGER_INFO( "system", "set render viewport [%f %f - %f %f]"
            , renderViewport.begin.x
            , renderViewport.begin.y
            , renderViewport.getWidth()
            , renderViewport.getHeight()
        );

        m_renderViewport = renderViewport;

        RENDER_SERVICE()
            ->changeWindowMode( m_currentWindowResolution, m_renderViewport, fullscreen );

        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION, fullscreen, m_currentWindowResolution );

        if( SERVICE_IS_INITIALIZE( GameServiceInterface ) == true )
        {
            GAME_SERVICE()
                ->setRenderViewport( m_renderViewport, m_contentResolution );

            float gameViewportAspect;
            Viewport gameViewport;
            APPLICATION_SERVICE()
                ->getGameViewport( &gameViewportAspect, &gameViewport );

            GAME_SERVICE()
                ->setGameViewport( gameViewport, gameViewportAspect );

            LOGGER_INFO( "system", "game viewport [%f %f - %f %f] aspect [%f]"
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

        if( m_alwaysfullscreen == true )
        {
            return true;
        }

        return m_fullscreen;
    }
    //////////////////////////////////////////////////////////////////////////
    const Viewport & Application::getRenderViewport() const
    {
        return m_renderViewport;
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::setContentResolution( const Resolution & _resolution )
    {
        if( m_contentResolution == _resolution )
        {
            return;
        }

        LOGGER_INFO( "system", "set change content resolution size %u:%u -> %u:%u aspect %f -> %f"
            , m_contentResolution.getWidth()
            , m_contentResolution.getHeight()
            , _resolution.getWidth()
            , _resolution.getHeight()
            , m_contentResolution.getAspectRatio()
            , _resolution.getAspectRatio()
        );

        m_contentResolution = _resolution;

        if( SERVICE_IS_INITIALIZE( PlayerServiceInterface ) == true )
        {
            const RenderResolutionInterfacePtr & renderResolution = PLAYER_SERVICE()
                ->getRenderResolution();

            if( renderResolution != nullptr )
            {
                renderResolution->setContentResolution( m_contentResolution );
            }
        }

        if( m_createRenderWindow == true )
        {
            this->invalidateWindow_();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Application::getCompanyName( Char * const _companyName ) const
    {
        m_companyName.copy( _companyName );

        size_t companyNameLen = m_companyName.size();

        return companyNameLen;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Application::getProjectName( Char * const _projectName ) const
    {
        m_projectName.copy( _projectName );

        size_t projectNameLen = m_projectName.size();

        return projectNameLen;
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
        if( _locale.empty() == true )
        {
            LOGGER_ERROR( "cannot set empty locale" );

            return;
        }

        if( m_locale == _locale )
        {
            LOGGER_WARNING( "already set locale '%s'"
                , _locale.c_str()
            );

            return;
        }

        ConstString prevLocale = m_locale;

        m_locale = _locale;

        LOGGER_INFO( "system", "begin change locale '%s' old '%s'"
            , m_locale.c_str()
            , prevLocale.c_str()
        );

        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_LOCALE_PREPARE, prevLocale, _locale );
        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_LOCALE, prevLocale, _locale );
        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_LOCALE_POST, prevLocale, _locale );

        LOGGER_INFO( "system", "end change locale '%s' old '%s'"
            , m_locale.c_str()
            , prevLocale.c_str()
        );
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

        LOGGER_INFO( "system", "set fixed content resolution: %d"
            , _fixedContetResolution
        );

        m_fixedContentResolution = _fixedContetResolution;

        this->invalidateWindow_();

        GAME_SERVICE()
            ->setFixedContentResolution( m_fixedContentResolution );
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

        LOGGER_INFO( "system", "set fixed display resolution: %d"
            , _fixedDisplayResolution
        );

        m_fixedDisplayResolution = _fixedDisplayResolution;

        this->invalidateWindow_();

        GAME_SERVICE()
            ->setFixedDisplayResolution( m_fixedDisplayResolution );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::getFixedDisplayResolution() const
    {
        return m_fixedDisplayResolution;
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::setFixedViewportResolution( bool _fixedViewportResolution )
    {
        if( m_fixedViewportResolution == _fixedViewportResolution )
        {
            return;
        }

        LOGGER_INFO( "system", "set fixed display resolution: %d"
            , _fixedViewportResolution
        );

        m_fixedViewportResolution = _fixedViewportResolution;

        this->invalidateWindow_();

        GAME_SERVICE()
            ->setFixedViewportResolution( m_fixedViewportResolution );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::getFixedViewportResolution() const
    {
        return m_fixedViewportResolution;
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::getGameViewport( float * const _aspect, Viewport * const _viewport ) const
    {
        const Resolution & contentResolution = this->getContentResolution();

        float contentWidth = contentResolution.getWidthF();
        float contentHeight = contentResolution.getHeightF();
        float contentAspect = contentResolution.getAspectRatio();

        if( m_fixedContentResolution == false )
        {
            *_aspect = contentAspect;

            _viewport->begin.x = 0.f;
            _viewport->begin.y = 0.f;
            _viewport->end.x = contentWidth;
            _viewport->end.y = contentHeight;

            return;
        }

        const Resolution & currentWindowResolution = this->getCurrentWindowResolution();

        float windowAspect = currentWindowResolution.getAspectRatio();

        if( m_fixedViewportResolution == true )
        {
            float bestAspect;
            Viewport aspectRatioViewport;
            if( Helper::findBestAspectViewport( m_aspectRatioViewports, windowAspect, &bestAspect, &aspectRatioViewport ) == true )
            {
                *_aspect = bestAspect;
                *_viewport = aspectRatioViewport;

                return;
            }

            *_aspect = contentAspect;

            _viewport->begin.x = 0.f;
            _viewport->begin.y = 0.f;
            _viewport->end.x = contentWidth;
            _viewport->end.y = contentHeight;

            return;
        }

        float dw = 1.f;
        float dh = contentAspect / windowAspect;

        if( dh < 1.f )
        {
            dw = windowAspect / contentAspect;
            dh = 1.f;
        }

        float areaWidth = StdMath::ceilf( dw * contentWidth );
        float areaHeight = StdMath::ceilf( dh * contentHeight );

        *_aspect = contentAspect;

        _viewport->begin.x = StdMath::ceilf( (contentWidth - areaWidth) * 0.5f );
        _viewport->begin.y = StdMath::ceilf( (contentHeight - areaHeight) * 0.5f );
        _viewport->end.x = _viewport->begin.x + areaWidth;
        _viewport->end.y = _viewport->begin.y + areaHeight;
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::setAspectRatioViewports( const VectorAspectRatioViewports & _viewports )
    {
        m_aspectRatioViewports = _viewports;

        if( m_createRenderWindow == true )
        {
            this->invalidateWindow_();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    EContentEdgeMode Application::getAspectRatioContentEdgeMode() const
    {
        if( m_fixedContentResolution == false )
        {
            return ECEM_NONE;
        }

        if( m_fixedViewportResolution == true )
        {
            return ECEM_NONE;
        }

        const Resolution & contentResolution = this->getContentResolution();
        const Resolution & currentWindowResolution = this->getCurrentWindowResolution();

        float contentAspect = contentResolution.getAspectRatio();
        float windowAspect = currentWindowResolution.getAspectRatio();

        float dh = contentAspect / windowAspect;

        if( dh < 1.f )
        {
            return ECEM_HORIZONTAL_CONTENT_EDGE;
        }

        return ECEM_VERTICAL_CONTENT_EDGE;
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
        bool OPTION_novsync = HAS_OPTION( "novsync" );

        if( OPTION_novsync == true )
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

            if( SERVICE_IS_INITIALIZE( RenderServiceInterface ) == true )
            {
                RENDER_SERVICE()
                    ->setVSync( vsync );
            }

            RenderPlatformInterface * renderPlatform = PLATFORM_SERVICE()
                ->getRenderPlatform();

            renderPlatform->notifyVsyncChanged( vsync );

            m_invalidateVsync = false;
        }

        if( m_invalidateCursorMode == true )
        {
            m_invalidateCursorMode = false;

            PLATFORM_SERVICE()
                ->notifyCursorModeChanged( m_cursorMode );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::setCursorMode( bool _cursorMode )
    {
        if( m_cursorMode == _cursorMode )
        {
            return;
        }

        m_cursorMode = _cursorMode;

        if( SERVICE_IS_INITIALIZE( GameServiceInterface ) == true )
        {
            GAME_SERVICE()
                ->setCursorMode( m_cursorMode );
        }

        if( m_cursorMode == true && m_cursorResource != nullptr )
        {
            const ConstString & name = m_cursorResource->getName();
            const ContentInterfacePtr & content = m_cursorResource->getContent();
            const MemoryInterfacePtr & buffer = m_cursorResource->getBuffer();

            PLATFORM_SERVICE()
                ->notifyCursorIconSetup( name, content, buffer );
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
        if( m_cursorResource != nullptr && _resourceName == m_cursorResource->getName() )
        {
            return;
        }

        ResourceCursorPtr cursorResource = RESOURCE_SERVICE()
            ->getResourceReference( ConstString::none(), _resourceName );

        MENGINE_ASSERTION_MEMORY_PANIC( cursorResource, "can't find resource cursor '%s'"
            , _resourceName.c_str()
        );

        if( cursorResource->compile() == false )
        {
            LOGGER_ERROR( "resource '%s' type '%s' invalid compile"
                , cursorResource->getName().c_str()
                , Helper::getFactorableType( cursorResource ).c_str()
            );

            return;
        }

        if( m_cursorResource != nullptr )
        {
            m_cursorResource->release();
            m_cursorResource = nullptr;
        }

        m_cursorResource = cursorResource;

        if( m_cursorMode == false )
        {
            return;
        }

        const ConstString & name = m_cursorResource->getName();
        const ContentInterfacePtr & content = m_cursorResource->getContent();
        const MemoryInterfacePtr & buffer = m_cursorResource->getBuffer();

        PLATFORM_SERVICE()
            ->notifyCursorIconSetup( name, content, buffer );
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::setDebugPause( bool _pause )
    {
        m_debugPause = _pause;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::getDebugPause() const
    {
        return m_debugPause;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderPipelineInterfacePtr & Application::getRenderPipeline() const
    {
        return m_renderPipeline;
    }
    //////////////////////////////////////////////////////////////////////////
}
