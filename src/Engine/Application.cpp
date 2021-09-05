#include "Application.h"

#include "Interface/OptionsServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/RenderServiceInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/SoundSystemInterface.h"
#include "Interface/SoundServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/PickerServiceInterface.h"
#include "Interface/LoaderServiceInterface.h"
#include "Interface/CodecInterface.h"
#include "Interface/ConverterInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/AccountInterface.h"
#include "Interface/GameServiceInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/WatchdogServiceInterface.h"
#include "Interface/GraveyardServiceInterface.h"
#include "Interface/PackageServiceInterface.h"
#include "Interface/TimelineServiceInterface.h"
#include "Interface/TimepipeServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/ModuleServiceInterface.h"
#include "Interface/SceneServiceInterface.h"
#include "Interface/ChronometerServiceInterface.h"
#include "Interface/SettingsServiceInterface.h"
#include "Interface/FactoryServiceInterface.h"
#include "Interface/UpdateServiceInterface.h"

#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Scene.h"
#include "Kernel/FactoryDefault.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/TagsHelper.h"
#include "Kernel/Entity.h"
#include "Kernel/Arrow.h"
#include "Kernel/Interender.h"
#include "Kernel/MatrixProxy.h"
#include "Kernel/FileContent.h"
#include "Kernel/RenderViewport.h"
#include "Kernel/RenderScissor.h"
#include "Kernel/RenderCameraOrthogonal.h"
#include "Kernel/RenderCameraProjection.h"
#include "Kernel/RenderCameraOrthogonalTarget.h"
#include "Kernel/BasePrototypeGenerator.h"
#include "Kernel/AssertionMemoryPanic.h"
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
#include "Kernel/MT19937Randomizer.h"
#include "Kernel/SecureValue.h"
#include "Kernel/BaseAffectorHub.h"
#include "Kernel/String.h"
#include "Kernel/Stringstream.h"
#include "Kernel/ViewportHelper.h"
#include "Kernel/List.h"
#include "Kernel/BuildMode.h"
#include "Kernel/FilePathDateTimeHelper.h"
#include "Kernel/ProfilerHelper.h"

#include "Config/StdString.h"
#include "Config/StdIntTypes.h"
#include "Config/GitSHA1.h"
#include "Config/BuildVersion.h"
#include "Config/BuildInfo.h"

// All Node type
#include "Isometric.h"
#include "HotSpot.h"
#include "HotSpotPolygon.h"
#include "HotSpotGlobal.h"
#include "HotSpotCircle.h"
#include "HotSpotBubbles.h"
#include "HotSpotImage.h"
#include "HotSpotResourceShape.h"
#include "HotSpotSurface.h"
#include "Gyroscope.h"
#include "Point.h"
#include "Line.h"
#include "SoundEmitter.h"
#include "Grid2D.h"
#include "TextField.h"
#include "Meshget.h"
#include "Layer2D.h"
#include "Window.h"
#include "Landscape2D.h"

#include "Kernel/ShapeCircle.h"
#include "Kernel/ShapePacMan.h"
#include "Kernel/ShapeQuadSize.h"
#include "Kernel/ShapeQuadFixed.h"
#include "Kernel/ShapeQuadFlex.h"

#include "Kernel/SurfaceSound.h"
#include "Kernel/SurfaceImage.h"
#include "Kernel/SurfaceImageSequence.h"
#include "Kernel/SurfaceTrackMatte.h"
#include "Kernel/SurfaceSolidColor.h"

// All Resource type
#include "Kernel/ResourceImageSequence.h"
#include "Kernel/ResourceImageData.h"
#include "Kernel/ResourceImageSolid.h"
#include "Kernel/ResourceImageDefault.h"
#include "Kernel/ResourceImageEmpty.h"
#include "Kernel/ResourceImageSubstract.h"
#include "Kernel/ResourceImageSubstractRGBAndAlpha.h"
#include "Kernel/ResourceSound.h"

#include "ResourceMusic.h"
#include "ResourceFile.h"
#include "ResourceWindow.h"
#include "ResourceHIT.h"
#include "ResourceShape.h"
#include "ResourceCursorICO.h"
#include "ResourceCursorSystem.h"

#include "EntityPrototypeGenerator.h"
#include "ScenePrototypeGenerator.h"
#include "ArrowPrototypeGenerator.h"

#include "Kernel/Stringstream.h"

#include <ctime>

#define MENGINE_DEBUG_HOTSPOTS 0x00000001
#define MENGINE_DEBUG_PHYSICS 0x00000002
#define MENGINE_DEBUG_NODES 0x00000004
#define MENGINE_DEBUG_SHADOWS 0x00000008
#define MENGINE_DEBUG_TILEPOLYGON 0x00000010

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
        , m_freeze( false )
        , m_update( true )
        , m_nopause( false )
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
        , m_createRenderWindow( false )
        , m_debugMask( 0 )
        , m_maxFrameTime( 100.f )
        , m_projectVersion( 0 )
        , m_invalidateVsync( true )
        , m_cursorMode( false )
        , m_invalidateCursorMode( true )
        , m_inputMouseButtonEventBlock( false )
        , m_debugPause( false )
        , m_debugFileOpen( false )
        , m_debugResourceCompile( false )
        , m_initailizeGame( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Application::~Application()
    {
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

        if( this->registerArrowGenerator_() == false )
        {
            LOGGER_ERROR( "invalid register arrow generator" );

            return false;
        }

        Helper::registerDecoder<ImageDecoderMemory>( STRINGIZE_STRING_LOCAL( "memoryImage" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerDecoder<ImageDecoderArchive>( STRINGIZE_STRING_LOCAL( "archiveImage" ), MENGINE_DOCUMENT_FACTORABLE );

        const Char * companyName = CONFIG_VALUE( "Project", "Company", "NONAME" );
        const Char * projectName = CONFIG_VALUE( "Project", "Name", "UNKNOWN" );

        MENGINE_STRCPY( m_companyName, companyName );
        MENGINE_STRCPY( m_projectName, projectName );

        m_projectCodename = CONFIG_VALUE( "Project", "Codename", ConstString::none() );
        m_projectVersion = CONFIG_VALUE( "Project", "Version", 0U );

        m_contentResolution = CONFIG_VALUE( "Game", "ContentResolution", Resolution( 1024, 768 ) );
        m_fixedContentResolution = CONFIG_VALUE( "Game", "FixedContentResolution", true );
        m_fixedDisplayResolution = CONFIG_VALUE( "Game", "FixedDisplayResolution", true );

        VectorAspectRatioViewports aspectRatioViewports;
        CONFIG_VALUES( "Game", "AspectRatioViewport", &aspectRatioViewports );

        for( const AspectRatioViewport & viewports : aspectRatioViewports )
        {
            float aspect = viewports.width / viewports.height;

            m_aspectRatioViewports[aspect] = viewports.viewport;
        }

        if( HAS_OPTION( "locale" ) == true )
        {
            const Char * option_locale = GET_OPTION_VALUE( "locale", nullptr );

            m_locale = Helper::stringizeString( option_locale );
        }
        else
        {
            m_locale = CONFIG_VALUE( "Locale", "Default", STRINGIZE_FILEPATH_LOCAL( "en" ) );
        }

        LOGGER_MESSAGE( "setup locale '%s'"
            , m_locale.c_str()
        );

#if defined(MENGINE_PLATFORM_IOS)
#elif defined(MENGINE_PLATFORM_ANDROID)
#else
        m_windowResolution = CONFIG_VALUE( "Window", "Size", Resolution( 1024, 768 ) );
#endif

        m_bits = CONFIG_VALUE( "Window", "Bits", 32U );
        m_fullscreen = CONFIG_VALUE( "Window", "Fullscreen", true );
        m_nofullscreen = CONFIG_VALUE( "Window", "NoFullscreen", false );
        m_alwaysfullscreen = CONFIG_VALUE( "Window", "AlwaysFullscreen", false );
        m_FSAAType = CONFIG_VALUE( "Window", "FSAAType", 0 );
        m_FSAAQuality = CONFIG_VALUE( "Window", "FSAAQuality", 0 );
        m_vsync = CONFIG_VALUE( "Window", "VSync", true );

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

        if( HAS_OPTION( "buildcommit" ) == true )
        {
            PLATFORM_SERVICE()
                ->messageBox( "Mengine", "build commit: %s", MENGINE_ENGINE_GIT_SHA1 );
        }
        else
        {
            LOGGER_MESSAGE_RELEASE( "Build Commit: %s", MENGINE_ENGINE_GIT_SHA1 );
        }

        const Char * Info_ResourceCommit = CONFIG_VALUE( "Info", "ResourceCommit", "0000000000000000000000000000000000000000" );

        if( HAS_OPTION( "resourcecommit" ) == true )
        {
            PLATFORM_SERVICE()
                ->messageBox( "Mengine", "content commit: %s", Info_ResourceCommit );
        }
        else
        {
            LOGGER_MESSAGE_RELEASE( "Content Commit: %s", Info_ResourceCommit );
        }

        if( HAS_OPTION( "buildversion" ) == true )
        {
            PLATFORM_SERVICE()
                ->messageBox( "Mengine", "build version: %s", MENGINE_BUILD_VERSION );
        }
        else
        {
            LOGGER_MESSAGE_RELEASE( "Build Version: %s", MENGINE_BUILD_VERSION );
        }

        if( HAS_OPTION( "buildinfo" ) == true )
        {
            PLATFORM_SERVICE()
                ->messageBox( "Mengine", "build info: %s %s", MENGINE_BUILD_USERNAME, MENGINE_BUILD_TIMESTAMP );
        }
        else
        {
            LOGGER_MESSAGE_RELEASE( "Build Info: %s %s", MENGINE_BUILD_USERNAME, MENGINE_BUILD_TIMESTAMP );
        }

        bool fullscreen = this->getFullscreenMode();

        if( this->calcWindowResolution( fullscreen, &m_currentResolution ) == false )
        {
            LOGGER_ERROR( "invalid calc window resolution" );

            return false;
        }

        LOGGER_MESSAGE( "current resolution [%u:%u]"
            , m_currentResolution.getWidth()
            , m_currentResolution.getHeight()
        );

        if( CONFIG_VALUE( "Debug", "ShowHotspots", false ) == true )
        {
            m_debugMask |= MENGINE_DEBUG_HOTSPOTS;
        }

        LOGGER_MESSAGE( "Application company '%s' project '%s' version [%u] locale '%s'"
            , m_companyName
            , m_projectName
            , m_projectVersion
            , m_locale.c_str()
        );

        bool OPTION_logopenfiles = HAS_OPTION( "logopenfiles" );

        if( OPTION_logopenfiles == true )
        {
            m_debugFileOpen = true;

            this->updateDebugOpenFile_();
        }

        bool OPTION_novideo = HAS_OPTION( "novideo" );

        if( OPTION_novideo == true )
        {
            this->setVideoEnable( false );
        }

        bool OPTION_nopause = HAS_OPTION( "nopause" );

        this->setNopause( OPTION_nopause );

        m_cursorMode = CONFIG_VALUE( "Platform", "Cursor", false );

        ConstString Engine_RenderPipeline = CONFIG_VALUE( "Engine", "RenderPipeline", STRINGIZE_STRING_LOCAL( "Batch" ) );

        RenderPipelineInterfacePtr renderPipeline = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "RenderPipeline" ), Engine_RenderPipeline, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( renderPipeline );

        if( renderPipeline->initialize() == false )
        {
            LOGGER_ERROR( "render pipeline '%s' invalid initialize"
                , Engine_RenderPipeline.c_str()
            );

            return false;
        }

        m_renderPipeline = renderPipeline;

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_ENGINE_PREPARE_FINALIZE, &Application::notifyEnginePrepareFinalize_, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_ENGINE_PREPARE_FINALIZE );

        if( m_debugFileOpen == true )
        {
            NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_DEBUG_OPEN_FILE );
        }

        if( m_debugResourceCompile == true )
        {
            NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_DEVELOPMENT_RESOURCE_COMPILE );
            NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_DEVELOPMENT_RESOURCE_RELEASE );
        }

        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "memoryImage" ) );
        Helper::unregisterDecoder( STRINGIZE_STRING_LOCAL( "archiveImage" ) );

        this->unregisterBaseTypes_();
        this->unregisterBaseNodeTypes_();
        this->unregisterBaseResourceTypes_();
        this->unregisterEntityGenerator_();
        this->unregisterSceneGenerator_();
        this->unregisterArrowGenerator_();
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::_stopService()
    {
        if( SERVICE_EXIST( PlayerServiceInterface ) == true )
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
        LOGGER_MESSAGE( "Register Base Generator..." );

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "FileContent" ), ConstString::none(), Helper::makeDefaultPrototypeGenerator<FileContent, 128>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "EntityEventable" ), ConstString::none(), Helper::makeDefaultPrototypeGenerator<EntityEventable, 128>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Randomizer" ), STRINGIZE_STRING_LOCAL( "MT19937Randomizer" ), Helper::makeDefaultPrototypeGenerator<MT19937Randomizer, 8>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "SecureValue" ), ConstString::none(), Helper::makeDefaultPrototypeGenerator<SecureValue, 64>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "BaseAffectorHub" ), ConstString::none(), Helper::makeDefaultPrototypeGenerator<BaseAffectorHub, 128>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::unregisterBaseTypes_()
    {
        LOGGER_MESSAGE( "Unregister Base Generator..." );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "FileContent" ), ConstString::none() );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "EntityEventable" ), ConstString::none() );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Randomizer" ), STRINGIZE_STRING_LOCAL( "MT19937Randomizer" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "SecureValue" ), ConstString::none() );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "BaseAffectorHub" ), ConstString::none() );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::registerBaseNodeTypes_()
    {
        LOGGER_MESSAGE( "Register Node Generator..." );

#define NODE_FACTORY( Type )\
        if( PROTOTYPE_SERVICE()\
            ->addPrototype( STRINGIZE_STRING_LOCAL("Node"), STRINGIZE_STRING_LOCAL(#Type), Helper::makeFactorableUnique<NodePrototypeGenerator<Type, 128>>(MENGINE_DOCUMENT_FACTORABLE) ) == false )\
        {\
            return false;\
        }

        NODE_FACTORY( Entity );
        NODE_FACTORY( Arrow );
        NODE_FACTORY( Scene );
        NODE_FACTORY( Gyroscope );
        NODE_FACTORY( Isometric );
        NODE_FACTORY( MatrixProxy );
        NODE_FACTORY( HotSpotPolygon );
        NODE_FACTORY( HotSpotGlobal );
        NODE_FACTORY( HotSpotCircle );
        NODE_FACTORY( HotSpotBubbles );
        NODE_FACTORY( HotSpotImage );
        NODE_FACTORY( HotSpotResourceShape );
        NODE_FACTORY( HotSpotSurface );
        NODE_FACTORY( Point );
        NODE_FACTORY( Line );
        NODE_FACTORY( SoundEmitter );
        NODE_FACTORY( Grid2D );
        NODE_FACTORY( TextField );
        NODE_FACTORY( Meshget );
        NODE_FACTORY( Interender );
        NODE_FACTORY( Layer2D );
        NODE_FACTORY( Landscape2D );
        NODE_FACTORY( RenderViewport );
        NODE_FACTORY( RenderScissor );
        NODE_FACTORY( RenderCameraOrthogonal );
        NODE_FACTORY( RenderCameraProjection );
        NODE_FACTORY( RenderCameraOrthogonalTarget );
        NODE_FACTORY( Window );
        NODE_FACTORY( ShapeCircle );
        NODE_FACTORY( ShapePacMan );
        NODE_FACTORY( ShapeQuadSize );
        NODE_FACTORY( ShapeQuadFixed );
        NODE_FACTORY( ShapeQuadFlex );

#undef NODE_FACTORY

#define SURFACE_FACTORY(Type)\
        if( PROTOTYPE_SERVICE()\
            ->addPrototype( STRINGIZE_STRING_LOCAL("Surface"), STRINGIZE_STRING_LOCAL(#Type), Helper::makeFactorableUnique<SurfacePrototypeGenerator<Type, 128>>(MENGINE_DOCUMENT_FACTORABLE) ) == false )\
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
        LOGGER_MESSAGE( "Unregister Node Generator..." );

#define NODE_FACTORY( Type )\
        PROTOTYPE_SERVICE()\
            ->removePrototype( STRINGIZE_STRING_LOCAL("Node"), STRINGIZE_STRING_LOCAL(#Type) )

        NODE_FACTORY( Entity );
        NODE_FACTORY( Arrow );
        NODE_FACTORY( Scene );
        NODE_FACTORY( Gyroscope );
        NODE_FACTORY( Isometric );
        NODE_FACTORY( MatrixProxy );
        NODE_FACTORY( HotSpotPolygon );
        NODE_FACTORY( HotSpotGlobal );
        NODE_FACTORY( HotSpotCircle );
        NODE_FACTORY( HotSpotBubbles );
        NODE_FACTORY( HotSpotImage );
        NODE_FACTORY( HotSpotResourceShape );
        NODE_FACTORY( HotSpotSurface );
        NODE_FACTORY( Point );
        NODE_FACTORY( Line );
        NODE_FACTORY( SoundEmitter );
        NODE_FACTORY( Grid2D );
        NODE_FACTORY( TextField );
        NODE_FACTORY( Meshget );
        NODE_FACTORY( Interender );
        NODE_FACTORY( Layer2D );
        NODE_FACTORY( Landscape2D );
        NODE_FACTORY( RenderViewport );
        NODE_FACTORY( RenderScissor );
        NODE_FACTORY( RenderCameraOrthogonal );
        NODE_FACTORY( RenderCameraProjection );
        NODE_FACTORY( RenderCameraOrthogonalTarget );
        NODE_FACTORY( Window );
        NODE_FACTORY( ShapeCircle );
        NODE_FACTORY( ShapePacMan );
        NODE_FACTORY( ShapeQuadSize );
        NODE_FACTORY( ShapeQuadFixed );
        NODE_FACTORY( ShapeQuadFlex );

#undef NODE_FACTORY

#define SURFACE_FACTORY(Type)\
        PROTOTYPE_SERVICE()\
            ->removePrototype( STRINGIZE_STRING_LOCAL("Surface"), STRINGIZE_STRING_LOCAL(#Type) )

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
        LOGGER_INFO( "system", "Register Entity Generator..." );

        EntityPrototypeGeneratorPtr generator = Helper::makeFactorableUnique<EntityPrototypeGenerator>( MENGINE_DOCUMENT_FACTORABLE );

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Entity" ), ConstString::none(), generator ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::registerSceneGenerator_()
    {
        LOGGER_INFO( "system", "Register Scene Generator..." );

        PrototypeGeneratorInterfacePtr generator = Helper::makeFactorableUnique<ScenePrototypeGenerator>( MENGINE_DOCUMENT_FACTORABLE );

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Scene" ), ConstString::none(), generator ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::unregisterEntityGenerator_()
    {
        LOGGER_INFO( "system", "Unregister Entity Generator..." );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Entity" ), ConstString::none() );
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::unregisterSceneGenerator_()
    {
        LOGGER_INFO( "system", "Unregister Scene Generator..." );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Scene" ), ConstString::none() );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::registerArrowGenerator_()
    {
        LOGGER_INFO( "system", "Register Arrow Generator..." );

        PrototypeGeneratorInterfacePtr generator = Helper::makeFactorableUnique<ArrowPrototypeGenerator>( MENGINE_DOCUMENT_FACTORABLE );

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Arrow" ), ConstString::none(), generator ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::unregisterArrowGenerator_()
    {
        LOGGER_INFO( "system", "Unregister Arrow Generator..." );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Arrow" ), ConstString::none() );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::registerBaseResourceTypes_()
    {
        LOGGER_INFO( "system", "Register Resource Type..." );

#define ADD_PROTOTYPE( Type )\
        if( PROTOTYPE_SERVICE()\
            ->addPrototype( STRINGIZE_STRING_LOCAL("Resource"), STRINGIZE_STRING_LOCAL(#Type), Helper::makeFactorableUnique<ResourcePrototypeGenerator<Type, 128>>(MENGINE_DOCUMENT_FACTORABLE) ) == false )\
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

#undef ADD_PROTOTYPE

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::unregisterBaseResourceTypes_()
    {
        LOGGER_INFO( "system", "Initializing Resource Type..." );

#define REMOVE_PROTOTYPE( Type )\
        PROTOTYPE_SERVICE()\
            ->removePrototype( STRINGIZE_STRING_LOCAL("Resource"), STRINGIZE_STRING_LOCAL(#Type) )

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

#undef REMOVE_PROTOTYPE
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::createRenderWindow()
    {
        bool fullscreen = this->getFullscreenMode();

        bool vsync = this->getVSync();

        RENDER_SERVICE()
            ->setVSync( vsync );

        LOGGER_MESSAGE( "current resolution [%u:%u] %s"
            , m_currentResolution.getWidth()
            , m_currentResolution.getHeight()
            , fullscreen ? "Fullscreen" : "Window"
        );

        this->calcRenderViewport_( m_currentResolution, &m_renderViewport );

        LOGGER_MESSAGE( "render viewport %f %f - %f %f"
            , m_renderViewport.begin.x
            , m_renderViewport.begin.y
            , m_renderViewport.getWidth()
            , m_renderViewport.getHeight()
        );

        bool result = RENDER_SERVICE()
            ->createRenderWindow( m_currentResolution
                , m_contentResolution
                , m_renderViewport
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

        GAME_SERVICE()
            ->initializeRenderResources();

        PLAYER_SERVICE()
            ->initializeRenderResources();

        NOTIFICATION_NOTIFY( NOTIFICATOR_CREATE_RENDER_WINDOW );

        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION, fullscreen, m_currentResolution );

        GAME_SERVICE()
            ->setRenderViewport( m_renderViewport, m_contentResolution );

        float gameViewportAspect;
        Viewport gameViewport;
        this->getGameViewport( &gameViewportAspect, &gameViewport );

        GAME_SERVICE()
            ->setGameViewport( gameViewport, gameViewportAspect );

        ResourceImagePtr resourceWhitePixel = RESOURCE_SERVICE()
            ->createResource( ConstString::none(), ConstString::none(), STRINGIZE_STRING_LOCAL( "WhitePixel" ), STRINGIZE_STRING_LOCAL( "ResourceImage" ), false, false, MENGINE_DOCUMENT_FACTORABLE );

        resourceWhitePixel->setMaxSize( mt::vec2f( 2.f, 2.f ) );
        resourceWhitePixel->setSize( mt::vec2f( 2.f, 2.f ) );
        resourceWhitePixel->setPow2( true );

        const RenderTextureInterfacePtr & whiteTexture = RENDER_SERVICE()
            ->getWhiteTexture();

        resourceWhitePixel->setTexture( whiteTexture );

        if( resourceWhitePixel->initialize() == false )
        {
            LOGGER_ERROR( "invalid initialize resource white pixel" );

            return false;
        }

        m_resourceWhitePixel = resourceWhitePixel;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::initializeGame( const FileGroupInterfacePtr & _fileGroup, const VectorFilePath & _packagesPaths, const VectorFilePath & _settingsPaths )
    {
        for( const FilePath & packagePath : _packagesPaths )
        {
            if( PACKAGE_SERVICE()
                ->loadPackages( _fileGroup, packagePath, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                LOGGER_ERROR( "invalid load package '%s'"
                    , packagePath.c_str()
                );

                return false;
            }
        }

        const Tags & platformTags = PLATFORM_SERVICE()
            ->getPlatformTags();

        LOGGER_INFO( "system", "load game resource tags '%s'"
            , Helper::tagsToString( platformTags ).c_str()
        );

        if( PACKAGE_SERVICE()
            ->enablePackages( m_locale, platformTags ) == false )
        {
            LOGGER_ERROR( "invalid enable for locale '%s' platform '%s'!"
                , m_locale.c_str()
                , Helper::tagsToString( platformTags ).c_str()
            );

            return false;
        }

        for( const FilePath & settingPath : _settingsPaths )
        {
            if( SETTINGS_SERVICE()
                ->loadSettings( _fileGroup, settingPath, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                LOGGER_ERROR( "invalid load setting '%s'"
                    , settingPath.c_str()
                );

                return false;
            }
        }

#ifndef MENGINE_MASTER_RELEASE
        bool developmentMode = Helper::isDevelopmentMode();
        bool OPTION_noresourceCheck = HAS_OPTION( "noresourcecheck" );

        if( developmentMode == true && OPTION_noresourceCheck == false )
        {
            if( TEXT_SERVICE()
                ->validate() == false )
            {
                LOGGER_ERROR( "invalid validate!" );
            }
        }
#endif

        m_initailizeGame = true;

        NOTIFICATION_NOTIFY( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_GAME );

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
    bool Application::handleKeyEvent( const InputKeyEvent & _event )
    {
#ifdef MENGINE_PLATFORM_DESKTOP
        bool developmentMode = Helper::isDevelopmentMode();

        if( developmentMode == true )
        {
            bool controlDown = _event.special.isControl;

            if( _event.code == KC_BACK && _event.isDown && controlDown == true )
            {
                Char userPath[MENGINE_MAX_PATH] = {'\0'};
                PLATFORM_SERVICE()
                    ->getUserPath( userPath );

                String processDumpPath;
                processDumpPath += userPath;
                processDumpPath += "Process";
                processDumpPath += "_";

                DateTimeProviderInterfacePtr dateTimeProvider = PLATFORM_SERVICE()
                    ->createDateTimeProvider( MENGINE_DOCUMENT_FACTORABLE );

                Char filePathDate[MENGINE_MAX_PATH] = {'\0'};
                Helper::makeFilePathDateTimeHelper( dateTimeProvider, filePathDate );

                processDumpPath += filePathDate;
                processDumpPath += ".dmp";

                PLATFORM_SERVICE()
                    ->createProcessDump( processDumpPath.c_str(), nullptr, true );
            }

            if( _event.code == KC_F6 && _event.isDown == true )
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

            if( _event.code == KC_F11 && _event.isDown == true )
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

            if( _event.code == KC_F10 && _event.isDown == true )
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

            if( _event.code == KC_F8 && _event.isDown == true )
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

            if( _event.code == KC_F5 && _event.isDown == true && controlDown == true )
            {
                SCENE_SERVICE()
                    ->removeCurrentScene( false, nullptr );
            }

            if( _event.code == KC_F5 && _event.isDown == true && controlDown == false )
            {
                SCENE_SERVICE()
                    ->restartCurrentScene( false, nullptr );
            }

            if( _event.code == KC_G && _event.isDown == true && controlDown == true )
            {
                FACTORY_SERVICE()
                    ->increfFactoryGeneration();
            }

            if( _event.code == KC_OEM_MINUS && _event.isDown == true )
            {
                if( m_renderPipeline->decrefDebugLimitRenderObjects() == false )
                {
                    m_debugPause = false;

                    m_renderPipeline->enableDebugStepRenderMode( false );
                }
            }

            if( _event.code == KC_MULTIPLY && _event.isDown == true )
            {
                m_debugPause = !m_debugPause;
            }

            if( _event.code == KC_OEM_PLUS && _event.isDown == true )
            {
                m_renderPipeline->increfDebugLimitRenderObjects();

                if( m_debugPause == false )
                {
                    m_renderPipeline->enableDebugStepRenderMode( true );
                }

                m_debugPause = true;
            }

            if( _event.special.isAlt == true && _event.isDown == true )
            {
                float timeFactor = -1.f;

                switch( _event.code )
                {
                case KC_P: timeFactor = 0.f; break;
                case KC_0: timeFactor = 0.5f; break;
                case KC_1: timeFactor = 1.f; break;
                case KC_2: timeFactor = 2.f; break;
                case KC_3: timeFactor = 3.f; break;
                case KC_4: timeFactor = 4.f; break;
                case KC_5: timeFactor = 5.f; break;
                case KC_6: timeFactor = 6.f; break;
                case KC_7: timeFactor = 7.f; break;
                case KC_8: timeFactor = 8.f; break;
                case KC_9: timeFactor = 9.f; break;
                default: break;
                }

                if( timeFactor >= 0.f )
                {
                    TIMELINE_SERVICE()
                        ->setTimeFactor( timeFactor );

                    LOGGER_MESSAGE( "time factor: %f"
                        , timeFactor
                    );
                }
            }

            if( _event.code == KC_OEM_6 && _event.isDown == true )
            {
                float timeFactor = TIMELINE_SERVICE()
                    ->getTimeFactor();

                float Debug_TimeFactorStep = CONFIG_VALUE( "Debug", "TimeFactorStep", 0.0625f );

                if( _event.special.isAlt == true )
                {
                    Debug_TimeFactorStep = 1.f;
                }

                timeFactor += Debug_TimeFactorStep;

                TIMELINE_SERVICE()
                    ->setTimeFactor( timeFactor );

                LOGGER_MESSAGE( "time factor: %f"
                    , timeFactor
                );
            }

            if( _event.code == KC_OEM_4 && _event.isDown == true )
            {
                float timeFactor = TIMELINE_SERVICE()
                    ->getTimeFactor();

                float Debug_TimeFactorStep = CONFIG_VALUE( "Debug", "TimeFactorStep", 0.0625f );

                if( _event.special.isAlt == true )
                {
                    Debug_TimeFactorStep = 1.f;
                }

                timeFactor -= Debug_TimeFactorStep;

                if( timeFactor < 0.f )
                {
                    timeFactor = 0.f;
                }

                TIMELINE_SERVICE()
                    ->setTimeFactor( timeFactor );

                LOGGER_MESSAGE( "time factor: %f"
                    , timeFactor
                );
            }

            //if( _event.code == KC_F12 && _event.isDown == true )
            //{
            //    bool enable = RENDER_SERVICE()
            //        ->isRedAlertMode();

            //    RENDER_SERVICE()
            //        ->enableRedAlertMode( !enable );
            //}

            if( _event.code == KC_F3 && _event.isDown == true )
            {
                m_debugResourceCompile = !m_debugResourceCompile;

                this->updateDebugResourceCompile_();
            }

            if( _event.code == KC_F4 && _event.isDown == true )
            {
                m_debugFileOpen = !m_debugFileOpen;

                this->updateDebugOpenFile_();
            }

            if( _event.code == KC_P && _event.isDown == true && controlDown == true )
            {
                static bool s_particle_enable = true;

                s_particle_enable = !s_particle_enable;

                APPLICATION_SERVICE()
                    ->setParticleEnable( s_particle_enable );

                LOGGER_MESSAGE_RELEASE( "Particle [%s]"
                    , s_particle_enable == true ? "enable" : "disable"
                );
            }

            if( _event.code == KC_T && _event.isDown == true && controlDown == true )
            {
                static bool s_text_enable = true;

                s_text_enable = !s_text_enable;

                APPLICATION_SERVICE()
                    ->setTextEnable( s_text_enable );

                LOGGER_MESSAGE_RELEASE( "Text [%s]"
                    , s_text_enable == true ? "enable" : "disable"
                );
            }

            if( _event.code == KC_V && _event.isDown == true && controlDown == true )
            {
                static bool s_video_enable = true;

                s_video_enable = !s_video_enable;

                APPLICATION_SERVICE()
                    ->setVideoEnable( s_video_enable );

                LOGGER_MESSAGE_RELEASE( "Video [%s]"
                    , s_video_enable == true ? "enable" : "disable"
                );
            }

            if( _event.code == KC_R && _event.isDown == true && controlDown == true )
            {
                static bool s_text_debug = true;

                s_text_debug = !s_text_debug;

                NOTIFICATION_NOTIFY( NOTIFICATOR_DEBUG_TEXT_MODE, s_text_debug );
            }

            if( _event.code == KC_E && _event.isDown == true && controlDown == true )
            {
                const ConstString & locale = APPLICATION_SERVICE()
                    ->getLocale();

                LOGGER_MESSAGE_RELEASE( "Reload locale [%s]"
                    , locale.c_str()
                );

                class MyRestartSceneChange
                    : public SceneChangeCallbackInterface
                    , public Factorable
                {
                protected:
                    void onSceneChange( const ScenePtr & _scene, bool _enable, bool _remove, bool _error ) override
                    {
                        MENGINE_UNUSED( _enable );
                        MENGINE_UNUSED( _remove );
                        MENGINE_UNUSED( _error );

                        if( _scene == nullptr )
                        {
                            const ConstString & locale = APPLICATION_SERVICE()
                                ->getLocale();

                            NOTIFICATION_NOTIFY( NOTIFICATOR_RELOAD_LOCALE_PREPARE );
                            NOTIFICATION_NOTIFY( NOTIFICATOR_RELOAD_LOCALE );
                            NOTIFICATION_NOTIFY( NOTIFICATOR_RELOAD_LOCALE_POST );

                            NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_LOCALE_PREPARE, locale, locale );
                            NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_LOCALE, locale, locale );
                            NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_LOCALE_POST, locale, locale );
                        }
                    }
                };

                typedef IntrusivePtr<MyRestartSceneChange> MyRestartSceneChangePtr;

                MyRestartSceneChangePtr cb = Helper::makeFactorableUnique<MyRestartSceneChange>( MENGINE_DOCUMENT_FACTORABLE );

                SCENE_SERVICE()
                    ->restartCurrentScene( false, cb );
            }

            //if( _event.code == KC_0 && _event.isDown == true )
            //{
            //    static uint32_t batchMode = RENDER_SERVICE()
            //        ->getBatchMode();

            //    ++batchMode;

            //    ERenderBatchMode mode = ERBM_NONE;
            //    switch( batchMode % 3 )
            //    {
            //    case 0:
            //        mode = ERBM_NONE;
            //        break;
            //    case 1:
            //        mode = ERBM_NORMAL;
            //        break;
            //    case 2:
            //        mode = ERBM_SMART;
            //        break;
            //    }

            //    RENDER_SERVICE()
            //        ->setBatchMode( mode );
            //}

            //if( _event.code == KC_F3 && _event.isDown == true )
            //{
            //    uint32_t count = stdex_allocator_report_count();

            //    for( uint32_t i = 0; i != count; ++i )
            //    {
            //        stdex_memory_report_t * report = stdex_allocator_report_info( i );

            //        LOGGER_ERROR( "block '%s' size [%u]"
            //            , report->name
            //            , report->count
            //        );
            //    }
            //}

            //if( _event.code == KC_F2 && _event.isDown == true )
            //{
            //}
        }
#endif

        bool handle = GAME_SERVICE()
            ->handleKeyEvent( _event );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::handleTextEvent( const InputTextEvent & _event )
    {
        bool handle = GAME_SERVICE()
            ->handleTextEvent( _event );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::handleMouseButtonEvent( const InputMouseButtonEvent & _event )
    {
        if( m_inputMouseButtonEventBlock == true )
        {
            return false;
        }

        bool handle = GAME_SERVICE()
            ->handleMouseButtonEvent( _event );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::handleMouseButtonEventBegin( const InputMouseButtonEvent & _event )
    {
        if( m_inputMouseButtonEventBlock == true )
        {
            return false;
        }

        bool handle = GAME_SERVICE()
            ->handleMouseButtonEventBegin( _event );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::handleMouseButtonEventEnd( const InputMouseButtonEvent & _event )
    {
        if( m_inputMouseButtonEventBlock == true )
        {
            return false;
        }

        bool handle = GAME_SERVICE()
            ->handleMouseButtonEventEnd( _event );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::handleMouseMove( const InputMouseMoveEvent & _event )
    {
        bool handle = GAME_SERVICE()
            ->handleMouseMove( _event );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::updateDebugOpenFile_()
    {
        if( m_debugFileOpen == true )
        {
            NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_DEBUG_OPEN_FILE, &Application::notifyDebugOpenFile_, MENGINE_DOCUMENT_FACTORABLE );
        }
        else
        {
            NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_DEBUG_OPEN_FILE );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::updateDebugResourceCompile_()
    {
        if( m_debugResourceCompile == true )
        {
            NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_DEVELOPMENT_RESOURCE_COMPILE, &Application::notifyDebugResourceCompile_, MENGINE_DOCUMENT_FACTORABLE );
            NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_DEVELOPMENT_RESOURCE_RELEASE, &Application::notifyDebugResourceRelease_, MENGINE_DOCUMENT_FACTORABLE );
        }
        else
        {
            NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_DEVELOPMENT_RESOURCE_COMPILE );
            NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_DEVELOPMENT_RESOURCE_RELEASE );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::notifyDebugOpenFile_( const Char * _folder, const Char * _filePath, bool _streaming )
    {
        bool isMainThread = THREAD_SERVICE()
            ->isMainThread();

        if( isMainThread == true )
        {
            if( _streaming == false )
            {
                LOGGER_VERBOSE_LEVEL( ConstString::none(), LM_STATISTIC, LCOLOR_GREEN, nullptr, 0 )("open %s%s"
                    , _folder
                    , _filePath
                    );
            }
            else
            {
                LOGGER_VERBOSE_LEVEL( ConstString::none(), LM_STATISTIC, LCOLOR_GREEN, nullptr, 0 )("streaming %s%s"
                    , _folder
                    , _filePath
                    );
            }
        }
        else
        {
            uint64_t tid = THREAD_SYSTEM()
                ->getCurrentThreadId();

            if( _streaming == false )
            {
                LOGGER_VERBOSE_LEVEL( ConstString::none(), LM_STATISTIC, LCOLOR_GREEN, nullptr, 0 )("open [multithread:%" PRIu64 "] open %s%s"
                    , tid
                    , _folder
                    , _filePath
                    );
            }
            else
            {
                LOGGER_VERBOSE_LEVEL( ConstString::none(), LM_STATISTIC, LCOLOR_GREEN, nullptr, 0 )("streaming [multithread:%" PRIu64 "] open %s%s"
                    , tid
                    , _folder
                    , _filePath
                    );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::notifyDebugResourceCompile_( Resource * _resource )
    {
        LOGGER_VERBOSE_LEVEL( ConstString::none(), LM_STATISTIC, LCOLOR_GREEN, nullptr, 0 )("compile %s type %s"
            , _resource->getName().c_str()
            , _resource->getType().c_str()
            );
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::notifyDebugResourceRelease_( Resource * _resource )
    {
        LOGGER_VERBOSE_LEVEL( ConstString::none(), LM_STATISTIC, LCOLOR_GREEN, nullptr, 0 )("release %s type %s"
            , _resource->getName().c_str()
            , _resource->getType().c_str()
            );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::handleMouseWheel( const InputMouseWheelEvent & _event )
    {
        bool handle = GAME_SERVICE()
            ->handleMouseWheel( _event );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::handleMouseEnter( const InputMouseEnterEvent & _event )
    {
        bool handle = GAME_SERVICE()
            ->handleMouseEnter( _event );

        return handle;
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::handleMouseLeave( const InputMouseLeaveEvent & _event )
    {
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
    bool Application::isFocus() const
    {
        return m_focus;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::isFrozen() const
    {
        return m_freeze;
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::setFocus( bool _focus )
    {
        if( m_focus == _focus )
        {
            return;
        }

        m_focus = _focus;

        LOGGER_INFO( "system", "focus [%u] (freeze %u)"
            , m_focus
            , m_freeze
        );

        if( m_freeze == true )
        {
            return;
        }

        if( SERVICE_EXIST( GameServiceInterface ) == true )
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

        LOGGER_INFO( "system", "freeze [%u] (focus %u)"
            , m_freeze
            , m_focus
        );

        if( m_focus == false )
        {
            return;
        }

        if( SERVICE_EXIST( GameServiceInterface ) == true )
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
        MENGINE_PROFILER_CATEGORY();

        if( SERVICE_EXIST( ThreadServiceInterface ) == true )
        {
            THREAD_SERVICE()
                ->update();
        }

        if( SERVICE_EXIST( ModuleServiceInterface ) == true )
        {
            MODULE_SERVICE()
                ->update( m_focus );
        }

        if( m_nopause == false )
        {
            if( m_update == false && (m_focus == false || m_freeze == true) )
            {
                return false;
            }
        }

        GAME_SERVICE()
            ->update();

        INPUT_SERVICE()
            ->update();

        CHRONOMETER_SERVICE()
            ->update();

        if( PLAYER_SERVICE()
            ->update() == false )
        {
            this->quit();

            return false;
        }

        if( (m_focus == false || m_freeze == true) && m_update == true )
        {
            m_update = false;
        }
        else if( (m_focus == true && m_freeze == false) && m_update == false )
        {
            m_update = true;
        }

        this->updateNotification();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::tick( float _time )
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
            ->getTimeFactor();

        time *= timeFactor;

        float current = TIMELINE_SERVICE()
            ->getTotalTime();

        UpdateContext applicationContext;
        applicationContext.revision = m_updateRevision;
        applicationContext.current = current;
        applicationContext.time = time;

        TIMELINE_SERVICE()
            ->begin( &applicationContext );

        TIMEPIPE_SERVICE()
            ->tick( &applicationContext );

        TIMELINE_SERVICE()
            ->end();
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::endUpdate()
    {
        SCENE_SERVICE()
            ->update();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Application::render()
    {
        if( RENDER_SERVICE()
            ->beginScene( m_renderPipeline ) == false )
        {
            return false;
        }

        GAME_SERVICE()
            ->render( m_renderPipeline );

        RENDER_SERVICE()
            ->endScene( m_renderPipeline );

        m_renderPipeline->clear();

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

        if( SERVICE_EXIST( GameServiceInterface ) == true )
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
            if( SERVICE_EXIST( SoundServiceInterface ) == true )
            {
                SOUND_SERVICE()
                    ->onTurnStream( false );
            }

            if( SERVICE_EXIST( GameServiceInterface ) == true )
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

        NOTIFICATION_NOTIFY( NOTIFICATOR_TURN_SOUND, _turn );

        if( SERVICE_EXIST( SoundServiceInterface ) == true )
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

        Resolution dres;
        if( PLATFORM_SERVICE()
            ->getMaxClientResolution( &dres ) == false )
        {
            LOGGER_MESSAGE( "Invalid get max client resolution" );

            return false;
        }

        LOGGER_MESSAGE( "Max Client Resolution %u %u"
            , dres.getWidth()
            , dres.getHeight()
        );

        LOGGER_MESSAGE( "Window Resolution %u %u"
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

            _windowResolution->setWidth( new_witdh );
            _windowResolution->setHeight( new_height );
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

            _windowResolution->setWidth( new_witdh );
            _windowResolution->setHeight( new_height );
        }
        else
        {
            *_windowResolution = m_windowResolution;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const Resolution & Application::getCurrentResolution() const
    {
        return m_currentResolution;
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::calcRenderViewport_( const Resolution & _resolution, Viewport * const _viewport )
    {
        uint32_t width = _resolution.getWidth();
        uint32_t height = _resolution.getHeight();

        LOGGER_INFO( "system", "resolution [%u %u]"
            , width
            , height
        );

        float rw = (float)width;
        float rh = (float)height;

        float r_aspect = _resolution.getAspectRatio();

        if( m_fixedDisplayResolution == true )
        {
            float c_aspect = m_contentResolution.getAspectRatio();

            float contentAspect;
            if( Helper::findBestAspectViewport( m_aspectRatioViewports, r_aspect, &contentAspect, nullptr ) == false )
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

            float areaWidth = MT_ceilf( dw * rw );
            float areaHeight = MT_ceilf( dh * rh );

            _viewport->begin.x = MT_ceilf( (rw - areaWidth) * 0.5f );
            _viewport->begin.y = MT_ceilf( (rh - areaHeight) * 0.5f );
            _viewport->end.x = _viewport->begin.x + areaWidth;
            _viewport->end.y = _viewport->begin.y + areaHeight;
        }
        else
        {
            _viewport->begin.x = 0.f;
            _viewport->begin.y = 0.f;
            _viewport->end.x = rw;
            _viewport->end.y = rh;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::changeWindowResolution( const Resolution & _resolution )
    {
        LOGGER_MESSAGE( "change window resolution size %u:%u -> %u:%u"
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

        if( m_alwaysfullscreen == true )
        {
            _fullscreen = true;
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
        bool fullscreen = this->getFullscreenMode();

        if( this->calcWindowResolution( fullscreen, &m_currentResolution ) == false )
        {
            return;
        }

        LOGGER_MESSAGE( "Current Resolution [%s] [%u %u]"
            , fullscreen == true ? "Fullscreen" : "Window"
            , m_currentResolution.getWidth()
            , m_currentResolution.getHeight()
        );

        bool vsync = this->getVSync();

        RENDER_SERVICE()
            ->setVSync( vsync );

        if( PLATFORM_SERVICE()
            ->notifyWindowModeChanged( m_currentResolution, fullscreen ) == false )
        {
            return;
        }

        this->calcRenderViewport_( m_currentResolution, &m_renderViewport );

        LOGGER_MESSAGE( "Render Viewport [%f %f - %f %f]"
            , m_renderViewport.begin.x
            , m_renderViewport.begin.y
            , m_renderViewport.getWidth()
            , m_renderViewport.getHeight()
        );

        RENDER_SERVICE()
            ->changeWindowMode( m_currentResolution, m_contentResolution, m_renderViewport, fullscreen );

        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_WINDOW_RESOLUTION, fullscreen, m_currentResolution );

        if( SERVICE_EXIST( GameServiceInterface ) == true )
        {
            GAME_SERVICE()
                ->setRenderViewport( m_renderViewport, m_contentResolution );

            float gameViewportAspect;
            Viewport gameViewport;

            APPLICATION_SERVICE()
                ->getGameViewport( &gameViewportAspect, &gameViewport );

            GAME_SERVICE()
                ->setGameViewport( gameViewport, gameViewportAspect );

            LOGGER_MESSAGE( "Game Viewport [%f %f - %f %f] Aspect [%f]"
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
    uint32_t Application::getDebugMask() const
    {
        return m_debugMask;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Application::getCompanyName( Char * const _companyName ) const
    {
        MENGINE_STRCPY( _companyName, m_companyName );

        size_t companyNameLen = MENGINE_STRLEN( m_companyName );

        return companyNameLen;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Application::getProjectName( Char * const _projectName ) const
    {
        MENGINE_STRCPY( _projectName, m_projectName );

        size_t projectNameLen = MENGINE_STRLEN( m_projectName );

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
        if( m_locale == _locale )
        {
            LOGGER_WARNING( "alredy set locale '%s'"
                , _locale.c_str()
            );

            return;
        }

        ConstString prevLocale = m_locale;

        m_locale = _locale;

        LOGGER_MESSAGE( "new locale '%s' old '%s'"
            , m_locale.c_str()
            , prevLocale.c_str()
        );

        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_LOCALE_PREPARE, prevLocale, m_locale );
        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_LOCALE, prevLocale, m_locale );
        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_LOCALE_POST, prevLocale, m_locale );
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
    bool Application::getFixedDisplayResolution() const
    {
        return m_fixedDisplayResolution;
    }
    //////////////////////////////////////////////////////////////////////////
    void Application::getGameViewport( float * const _aspect, Viewport * const _viewport ) const
    {
        float aspect = m_currentResolution.getAspectRatio();

        float width = m_contentResolution.getWidthF();
        float height = m_contentResolution.getHeightF();

        if( m_fixedContentResolution == false )
        {
            *_aspect = aspect;

            _viewport->begin.x = 0.f;
            _viewport->begin.y = 0.f;
            _viewport->end.x = width;
            _viewport->end.y = height;

            return;
        }

        float bestAspect;
        Viewport aspectRatioViewport;

        if( Helper::findBestAspectViewport( m_aspectRatioViewports, aspect, &bestAspect, &aspectRatioViewport ) == true )
        {
            *_aspect = bestAspect;

            *_viewport = aspectRatioViewport;
        }
        else
        {
            *_aspect = aspect;

            _viewport->begin.x = 0.f;
            _viewport->begin.y = 0.f;
            _viewport->end.x = width;
            _viewport->end.y = height;
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

            if( SERVICE_EXIST( RenderServiceInterface ) == true )
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

        if( SERVICE_EXIST( GameServiceInterface ) == true )
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
                , cursorResource->getType().c_str()
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
    void Application::debugPause( bool _pause )
    {
        m_debugPause = _pause;
    }
    //////////////////////////////////////////////////////////////////////////
}