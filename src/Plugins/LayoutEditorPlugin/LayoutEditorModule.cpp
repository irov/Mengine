#include "LayoutEditorModule.h"

#include "Interface/RenderMaterialServiceInterface.h"
#include "Interface/RenderTextureServiceInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/RenderEnumInterface.h"
#include "Interface/SceneServiceInterface.h"
#include "Interface/SoundServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/PlatformServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/ScriptProviderServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/TransformationInterface.h"
#include "Interface/UpdationInterface.h"
#include "Interface/RenderInterface.h"
#include "Interface/AnimationInterface.h"
#include "Interface/PickerInterface.h"

#include "LayoutEditorWidgetType.h"

#include "Kernel/TimestampHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/NodeCast.h"
#include "Kernel/SceneHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/LoggerHelper.h"
#include "Kernel/Map.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/Color.h"
#include "Kernel/ColorHelper.h"
#include "Kernel/Viewport.h"
#include "Kernel/NodeRenderHierarchy.h"

#include "Config/StdMath.h"
#include "Config/Version.h"

#include "Services/RenderService/BatchRenderPipeline.h"

#include "Config/StdIntTypes.h"
#include "Config/StdIO.h"
#include "Config/StdString.h"

#include "math/stdmath.h"

#include <limits>
#include <cfloat>
#include <cmath>
#include <unordered_map>
#include <algorithm>

namespace
{
    struct RotationDialState
    {
        float lastRemainder = 0.f;
    };

    static std::unordered_map<ImGuiID, RotationDialState> s_rotationDialStates;
}

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LayoutEditorModule::LayoutEditorModule()
        : m_scene( nullptr )
        , m_imguiRenderProvider( nullptr )
        , m_module( nullptr )
        , m_selectObject( nullptr )
        , m_selectNode( nullptr )
        , m_show( false )
        , m_showObjectsWindow( true )
        , m_showViewportWindow( true )
        , m_showAttributesWindow( true )
        , m_showNodeWindow( true )
        , m_showConsoleWindow( true )
        , m_consoleAutoScroll( true )
        , m_consoleClearOnPlay( false )
        , m_viewportRenderTarget( nullptr )
        , m_viewportRenderTexture( nullptr )
        , m_viewportRenderPipeline( nullptr )
        , m_viewportWidth( 0 )
        , m_viewportHeight( 0 )
        , m_showViewportDeviceFrame( true )
        , m_viewportDeviceFrameColor( Helper::makeColor8( 40, 40, 44, 255 ) )
        , m_viewportDeviceFrameWidth( 8.f )
        , m_viewportDeviceCornerRadius( 18.f )
        , m_viewportDeviceFitScale( 0.9f )
    {
        m_consoleFilterBuffer[0] = '\0';
        
        // Initialize all levels as visible by default
        for( uint32_t i = 0; i != LOGGER_LEVEL_COUNT; ++i )
        {
            m_consoleFilterLevels[i] = true;
        }
        
        // Hide SILENT level
        m_consoleFilterLevels[LM_SILENT] = false;
    }
    //////////////////////////////////////////////////////////////////////////
    LayoutEditorModule::~LayoutEditorModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LayoutEditorModule::_initializeModule()
    {
        const ImGUIRenderProviderInterfacePtr & imguiRenderProvider = IMGUI_SERVICE()
            ->getRenderProvider();

        m_imguiRenderProvider = imguiRenderProvider;

        // Enable docking once at initialization to avoid changing flags after NewFrame
        ImGuiIO & io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_GAME, &LayoutEditorModule::notifyBootstrapperInitializeGame_, MENGINE_DOCUMENT_FACTORABLE );

        // Setup logger to capture logs
        m_logger = Helper::makeFactorableUnique<LayoutEditorLogger>( MENGINE_DOCUMENT_FACTORABLE );

        if( m_logger->initializeLogger() == false )
        {
            return false;
        }

        LOGGER_SERVICE()
            ->registerLogger( m_logger );

        // Create viewport render pipeline
        RenderPipelineInterfacePtr pipeline = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "RenderPipeline" ), STRINGIZE_STRING_LOCAL( "Batch" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( pipeline, "LayoutEditor: invalid create render pipeline" );

        if( pipeline->initialize() == false )
        {
            LOGGER_ERROR( "LayoutEditor: failed to initialize render pipeline" );
            return false;
        }

        m_viewportRenderPipeline = pipeline;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void LayoutEditorModule::_finalizeModule()
    {
        // Cleanup viewport render target
        this->destroyViewportRenderTarget_();

        if( m_viewportRenderPipeline != nullptr )
        {
            m_viewportRenderPipeline->finalize();
            m_viewportRenderPipeline = nullptr;
        }

        // Unregister logger
        if( m_logger != nullptr )
        {
            LOGGER_SERVICE()
                ->unregisterLogger( m_logger );

            m_logger->finalizeLogger();
            m_logger = nullptr;
        }

        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_GAME );

        m_imguiRenderProvider = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LayoutEditorModule::_handleKeyEvent( const InputKeyEvent & _event )
    {
        if( _event.code == KC_F1 && _event.isDown == true )
        {
            if( m_show == false )
            {
                SOUND_SERVICE()
                    ->setMute( STRINGIZE_STRING_LOCAL( "LayoutEditor" ), true );

                Resolution currentWindowResolution = APPLICATION_SERVICE()
                    ->getWindowResolution();

                m_fromResolution = currentWindowResolution;
                m_viewportDeviceResolution = currentWindowResolution;

                const RenderResolutionInterfacePtr & renderResolution = PLAYER_SERVICE()
                    ->getRenderResolution();

                if( renderResolution != nullptr )
                {
                    const Resolution & contentResolution = renderResolution->getContentResolution();

                    if( contentResolution.getWidth() != 0 && contentResolution.getHeight() != 0 )
                    {
                        m_viewportDeviceResolution = contentResolution;
                    }
                }

                Resolution maxClientResolution;
                if( PLATFORM_SERVICE()
                    ->getMaxClientResolution( &maxClientResolution ) == false )
                {
                    LOGGER_ERROR( "invalid get max client resolution" );

                    return false;
                }

                Resolution maxClientResolution90;
                maxClientResolution.scaleTo( {0.9f, 0.9f}, &maxClientResolution90 );

                APPLICATION_SERVICE()
                    ->setWindowResolution( maxClientResolution90 );

                m_show = true;
            }
            else
            {
                SOUND_SERVICE()
                    ->setMute( STRINGIZE_STRING_LOCAL( "LayoutEditor" ), false );

                APPLICATION_SERVICE()
                    ->setWindowResolution( m_fromResolution );

                m_show = false;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void LayoutEditorModule::_beginUpdate( bool _focus )
    {
        MENGINE_UNUSED( _focus );

    }
    //////////////////////////////////////////////////////////////////////////
    void LayoutEditorModule::_render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context )
    {
        if( m_show == false )
        {
            return;
        }

        const RenderMaterialInterfacePtr & renderMaterial = RENDERMATERIAL_SERVICE()
            ->getDebugTriangleMaterial();

        _renderPipeline->addRenderExternal( _context, renderMaterial, nullptr, RenderDrawPrimitiveInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    void LayoutEditorModule::onRenderDrawPrimitives( const RenderPrimitive * _primitives, uint32_t _count ) const
    {
        MENGINE_UNUSED( _primitives );
        MENGINE_UNUSED( _count );

        m_imguiRenderProvider->newFrame();

        const_cast<LayoutEditorModule *>(this)->renderImGUI();

        m_imguiRenderProvider->endFrame();
    }
    //////////////////////////////////////////////////////////////////////////
    void LayoutEditorModule::notifyBootstrapperInitializeGame_()
    {
        ScriptModuleInterfacePtr module = SCRIPT_SERVICE()
            ->importModule( STRINGIZE_STRING_LOCAL( "Editor" ) );

        if( module == nullptr )
        {
            LOGGER_ERROR( "can't import LayoutEditor script module" );

            return;
        }

        m_module = module;
    }
    //////////////////////////////////////////////////////////////////////////
    void LayoutEditorModule::renderImGUI()
    {
        ImGuiViewport * vp = ImGui::GetMainViewport();

        ImGui::SetNextWindowPos( vp->Pos, ImGuiCond_Always );
        ImGui::SetNextWindowSize( vp->Size, ImGuiCond_Always );

        ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0, 0 ) );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );

        const ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse |
            ImGuiWindowFlags_MenuBar;

        if( ImGui::Begin( "Layout Editor", nullptr, flags ) )
        {
            this->renderImGUIHeader();
            this->renderImGUILayouts();
        }

        ImGui::End();

        ImGui::PopStyleVar( 3 );
    }
    //////////////////////////////////////////////////////////////////////////
    void LayoutEditorModule::renderImGUIHeader()
    {
        if( ImGui::BeginMenuBar() )
        {
            if( ImGui::BeginMenu( "File" ) )
            {
                if( ImGui::MenuItem( "Exit Editor" ) )
                {
                    m_show = false;

                    SOUND_SERVICE()
                        ->setMute( STRINGIZE_STRING_LOCAL( "LayoutEditor" ), false );

                    APPLICATION_SERVICE()
                        ->setWindowResolution( m_fromResolution );
                }

                ImGui::EndMenu();
            }

            if( ImGui::BeginMenu( "View" ) )
            {
                ImGui::MenuItem( "Objects", nullptr, &m_showObjectsWindow );
                ImGui::MenuItem( "Viewport", nullptr, &m_showViewportWindow );
                ImGui::MenuItem( "Attributes", nullptr, &m_showAttributesWindow );
                ImGui::MenuItem( "Node", nullptr, &m_showNodeWindow );
                ImGui::MenuItem( "Console", nullptr, &m_showConsoleWindow );

                ImGui::EndMenu();
            }

            if( ImGui::BeginMenu( "Help" ) )
            {
                Char projectName[MENGINE_APPLICATION_PROJECT_MAXNAME] = {'\0'};
                APPLICATION_SERVICE()
                    ->getProjectName( projectName );

                Char companyName[MENGINE_APPLICATION_COMPANY_MAXNAME] = {'\0'};
                APPLICATION_SERVICE()
                    ->getCompanyName( companyName );

                const ConstString & projectCodename = APPLICATION_SERVICE()->getProjectCodename();
                uint32_t projectVersion = APPLICATION_SERVICE()->getProjectVersion();

                ImGui::Text( "Engine Version: %s", MENGINE_ENGINE_VERSION_STRING );
                ImGui::Text( "Project: %s", projectName[0] != '\0' ? projectName : "<unnamed>" );
                ImGui::Text( "Codename: %s", projectCodename.empty() == false ? projectCodename.c_str() : "<none>" );
                ImGui::Text( "Project Version: %u", projectVersion );
                ImGui::Text( "Company: %s", companyName[0] != '\0' ? companyName : "<unknown>" );

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void LayoutEditorModule::renderImGUILayouts()
    {
        ImGuiIO & io = ImGui::GetIO();
        if( (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) == 0 )
        {
            // Fallback: docking not enabled, do nothing to avoid crashes
            return;
        }

        ImGuiID dockspaceId = ImGui::GetID( "LayoutEditorDockSpace" );

        ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;
        ImGui::DockSpace( dockspaceId, ImVec2( 0.0f, 0.0f ), dockspaceFlags );

        static bool initializedDock = false;
        if( initializedDock == false )
        {
            initializedDock = true;

            ImGui::DockBuilderRemoveNode( dockspaceId );
            ImGui::DockBuilderAddNode( dockspaceId, ImGuiDockNodeFlags_DockSpace );
            ImGui::DockBuilderSetNodeSize( dockspaceId, ImGui::GetWindowSize() );

            ImGuiID dockMain = dockspaceId;
            ImGuiID dockBottom = 0;
            ImGuiID dockTop = 0;
            ImGui::DockBuilderSplitNode( dockMain, ImGuiDir_Down, 0.25f, &dockBottom, &dockTop );

            ImGuiID dockLeft = 0;
            ImGuiID dockCenter = 0;
            ImGuiID dockRight = 0;
            ImGui::DockBuilderSplitNode( dockTop, ImGuiDir_Left, 0.20f, &dockLeft, &dockCenter );
            ImGui::DockBuilderSplitNode( dockCenter, ImGuiDir_Right, 0.25f, &dockRight, &dockCenter );

            ImGuiID dockRightBottom = 0;
            ImGui::DockBuilderSplitNode( dockRight, ImGuiDir_Down, 0.45f, &dockRightBottom, &dockRight );

            ImGui::DockBuilderDockWindow( "Objects", dockLeft );
            ImGui::DockBuilderDockWindow( "Viewport", dockCenter );
            ImGui::DockBuilderDockWindow( "Attributes", dockRight );
            ImGui::DockBuilderDockWindow( "Node", dockRightBottom );
            ImGui::DockBuilderDockWindow( "Console", dockBottom );

            ImGui::DockBuilderFinish( dockMain );
        }

        ImGuiWindowFlags panelFlags = ImGuiWindowFlags_NoCollapse;

        if( m_showObjectsWindow == true )
        {
            if( ImGui::Begin( "Objects", &m_showObjectsWindow, panelFlags ) )
            {
                this->renderImGUIObjectsTree();
            }
            ImGui::End();
        }

        if( m_showViewportWindow == true )
        {
            if( ImGui::Begin( "Viewport", &m_showViewportWindow, panelFlags | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse ) )
            {
                this->renderViewportInImGui_();
            }
            ImGui::End();
        }

        if( m_showAttributesWindow == true )
        {
            if( ImGui::Begin( "Attributes", &m_showAttributesWindow, panelFlags ) )
            {
                this->renderImGUISelectedObjectAttributes();
            }
            ImGui::End();
        }

        if( m_showNodeWindow == true )
        {
            if( ImGui::Begin( "Node", &m_showNodeWindow, panelFlags | ImGuiWindowFlags_NoScrollbar ) )
            {
                this->renderImGUINodeInspector();
            }
            ImGui::End();
        }

        if( m_showConsoleWindow == true )
        {
            if( ImGui::Begin( "Console", &m_showConsoleWindow, panelFlags ) )
            {
                this->renderImGUIConsole();
            }
            ImGui::End();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void LayoutEditorModule::renderImGUIObject( const pybind::object & _obj )
    {
        if( m_module == nullptr || _obj.is_none() == true )
        {
            return;
        }

        const Char * objectName = _obj.call_method( "getName" );

        const pybind::module & m = m_module->getModule();

        pybind::list childObjects = m.call_method( "GetLayoutEditorObjectChildren", _obj );

        bool hasObjectChildren = childObjects.empty() == false;

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanFullWidth;

        if( hasObjectChildren == true )
        {
            flags |= ImGuiTreeNodeFlags_OpenOnArrow;
        }
        else
        {
            flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        }

        if( m_selectObject != nullptr && m_selectObject.is( _obj ) == true )
        {
            flags |= ImGuiTreeNodeFlags_Selected;
        }

        void * treeId = _obj.ptr();

        bool open = ImGui::TreeNodeEx( treeId, flags, "%s", objectName );

        if( ImGui::IsItemClicked() )
        {
            m_selectObject = _obj;
            m_selectNode = nullptr;
        }

        if( hasObjectChildren == true && open == true )
        {
            this->renderImGUIObjectsChildren( childObjects );
            ImGui::TreePop();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void LayoutEditorModule::renderImGUIObjectsChildren( const pybind::list & _children )
    {
        for( const pybind::object & obj : _children )
        {
            this->renderImGUIObject( obj );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void LayoutEditorModule::renderImGUIGroups( const pybind::list & _groups )
    {
        for( const pybind::object & group : _groups )
        {
            bool groupEnable = group.call_method( "getEnable" );
            const Char * name = group.call_method( "getName" );

            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;

            if( groupEnable == false )
            {
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.6f, 0.6f, 0.6f, 1.0f ) );
            }

            bool open = ImGui::TreeNodeEx( (void *)name, flags, "%s", name );

            if( groupEnable == false )
            {
                ImGui::PopStyleColor();
            }

            if( open == true )
            {
                pybind::list objects = group.call_method( "getObjects" );

                this->renderImGUIObjectsChildren( objects );

                ImGui::TreePop();
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void LayoutEditorModule::renderImGUIObjectsTree()
    {
        const pybind::module & m = m_module->getModule();

        const Char * sceneName = m.call_method( "GetLayoutEditorSceneName" );

        // Header: current scene name
        ImGui::Text( "%s", sceneName );
        ImGui::Separator();

        // Tree: groups as roots
        pybind::list groups = m.call_method( "GetLayoutEditorSceneGroups" );

        this->renderImGUIGroups( groups );
    }
    //////////////////////////////////////////////////////////////////////////
    void LayoutEditorModule::renderImGUISelectedObjectAttributes()
    {
        if( m_selectObject == nullptr )
        {
            return;
        }

        const pybind::module & m = m_module->getModule();

        pybind::dict attributes = m.call_method( "GetLayoutEditorObjectAttributes", m_selectObject );

        for( pybind::dict::iterator
            it = attributes.begin(),
            it_end = attributes.end();
            it != it_end;
            ++it )
        {
            pybind::object name = it.key();
            pybind::tuple attribute = it.value();

            pybind::object value = attribute[0];
            pybind::object widget = attribute[1];

            if( widget.is_none() == true )
            {
                continue;
            }

            ELayoutEditorWidgetType widget_type = widget.call_method( "getType" );

            if( widget_type == ELayoutEditorWidgetType::LEWT_NONE )
            {
                continue;
            }

            pybind::object description = widget.call_method( "getDescription" );

            const Char * name_str = name.extract();

            ImGui::PushID( name_str );

            bool changed = false;

            switch( widget_type )
            {
            case ELayoutEditorWidgetType::LEWT_CHECKBOX:
                {
                    bool value_b = value.extract();

                    if( ImGui::Checkbox( name_str, &value_b ) )
                    {
                        m.call_method( "SetLayoutEditorObjectAttributeValue", m_selectObject, name, value_b );
                    }

                    if( description.is_none() == false )
                    {
                        const Char * description_str = description.extract();
                        ImGui::SameLine();
                        ImGui::TextDisabled( "(%s)", description_str );
                    }
                }
                break;

            case ELayoutEditorWidgetType::LEWT_POSITION:
            case ELayoutEditorWidgetType::LEWT_SCALE:
                {
                    ImGui::Separator();
                    ImGui::Text( "%s", name_str );

                    if( description.is_none() == false )
                    {
                        const Char * description_str = description.extract();
                        ImGui::SameLine();
                        ImGui::TextDisabled( "(%s)", description_str );
                    }

                    ImGui::Spacing();

                    mt::vec3f value_v3 = value.extract();

                    float step = this->getWidgetStep_( widget, 0.001f );

                    if( this->drawVec3Attribute_( value_v3, step, false, 0.f, 0.f ) == true )
                    {
                        m.call_method( "SetLayoutEditorObjectAttributeValue", m_selectObject, name, value_v3 );
                    }
                }
                break;

            case ELayoutEditorWidgetType::LEWT_ORIENTATION:
                {
                    ImGui::Separator();
                    ImGui::Text( "%s", name_str );

                    if( description.is_none() == false )
                    {
                        const Char * description_str = description.extract();
                        ImGui::SameLine();
                        ImGui::TextDisabled( "(%s)", description_str );
                    }

                    ImGui::Spacing();

                    mt::vec3f value_v3 = value.extract();

                    if( this->drawOrientationAttribute_( value_v3 ) == true )
                    {
                        m.call_method( "SetLayoutEditorObjectAttributeValue", m_selectObject, name, value_v3 );
                    }
                }
                break;

            case ELayoutEditorWidgetType::LEWT_ALPHA:
                {
                    // Заголовок атрибута
                    ImGui::Separator();
                    ImGui::Text( "%s", name_str );

                    if( description.is_none() == false )
                    {
                        const Char * description_str = description.extract();
                        ImGui::SameLine();
                        ImGui::TextDisabled( "(%s)", description_str );
                    }

                    ImGui::Spacing();

                    float value_v1 = value.extract();

                    Char tag_buffer[256];

                    ImGui::SetNextItemWidth( -1 );
                    MENGINE_SNPRINTF( tag_buffer, 256, "##attr_%s_slider", name_str );
                    if( ImGui::SliderFloat( tag_buffer, &value_v1, 0.0f, 1.0f ) )
                    {
                        m.call_method( "SetLayoutEditorObjectAttributeValue", m_selectObject, name, value_v1 );
                    }
                }
                break;

            case ELayoutEditorWidgetType::LEWT_RGB:
                {
                    ImGui::Separator();
                    ImGui::Text( "%s", name_str );

                    if( description.is_none() == false )
                    {
                        const Char * description_str = description.extract();
                        ImGui::SameLine();
                        ImGui::TextDisabled( "(%s)", description_str );
                    }

                    ImGui::Spacing();

                    mt::vec3f value_v3 = value.extract();

                    if( this->drawRGBAttribute_( value_v3 ) == true )
                    {
                        changed = true;
                    }

                    if( changed == true )
                    {
                        m.call_method( "SetLayoutEditorObjectAttributeValue", m_selectObject, name, value_v3 );
                    }
                }
                break;

            case ELayoutEditorWidgetType::LEWT_NONE:
            default:
                {
                    // Заголовок атрибута
                    ImGui::Separator();
                    ImGui::Text( "%s", name_str );

                    if( description.is_none() == false )
                    {
                        const Char * description_str = description.extract();
                        ImGui::SameLine();
                        ImGui::TextDisabled( "(%s)", description_str );
                    }

                    ImGui::Spacing();

                    pybind::string_view value_view = value.str();
                    const Char * value_view_str = value_view.c_str();

                    ImGui::TextUnformatted( value_view_str );
                }
                break;
            }

            ImGui::PopID();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void LayoutEditorModule::renderImGUINodeInspector()
    {
        NodePtr node = m_selectNode;

        if( node == nullptr && m_selectObject != nullptr )
        {
            node = this->getObjectEntityNode_( m_selectObject );
        }

        if( node == nullptr )
        {
            ImGui::TextUnformatted( "No node selected" );
            return;
        }

        const ConstString & type = node->getType();
        const Char * typeStr = type.empty() == false ? type.c_str() : "<unknown>";

        ImGui::Text( "Type: %s", typeStr );

        bool enabledFlag = node->isEnable();
        ImGui::BeginDisabled();
        ImGui::Checkbox( "Enable", &enabledFlag );
        ImGui::EndDisabled();

        bool activeFlag = node->isActivate();
        ImGui::BeginDisabled();
        ImGui::Checkbox( "Active", &activeFlag );
        ImGui::EndDisabled();

        TransformationInterface * transformation = node->getTransformation();
        RenderInterface * render = node->getRender();
        AnimationInterface * animation = node->getAnimation();
        PickerInterface * picker = node->getPicker();

        if( transformation != nullptr && ImGui::CollapsingHeader( "Transform", ImGuiTreeNodeFlags_DefaultOpen ) )
        {
            mt::vec3f localPosition = transformation->getLocalPosition();
            float localPositionBuffer[3] = { localPosition.x, localPosition.y, localPosition.z };

            if( ImGui::DragFloat3( "Local Position", localPositionBuffer, 0.001f, 0.0f, 0.0f, "%.3f" ) )
            {
                transformation->setLocalPosition( mt::vec3f( localPositionBuffer[0], localPositionBuffer[1], localPositionBuffer[2] ) );
            }

            mt::vec3f localOrigin = transformation->getLocalOrigin();
            float localOriginBuffer[3] = { localOrigin.x, localOrigin.y, localOrigin.z };

            if( ImGui::DragFloat3( "Local Origin", localOriginBuffer, 0.001f, 0.0f, 0.0f, "%.3f" ) )
            {
                transformation->setLocalOrigin( mt::vec3f( localOriginBuffer[0], localOriginBuffer[1], localOriginBuffer[2] ) );
            }

            mt::vec3f localScale = transformation->getLocalScale();
            float localScaleBuffer[3] = { localScale.x, localScale.y, localScale.z };

            if( ImGui::DragFloat3( "Local Scale", localScaleBuffer, 0.001f, 0.0f, 0.0f, "%.3f" ) )
            {
                transformation->setLocalScale( mt::vec3f( localScaleBuffer[0], localScaleBuffer[1], localScaleBuffer[2] ) );
            }

            mt::vec2f localSkew = transformation->getLocalSkew();
            float localSkewBuffer[2] = { localSkew.x, localSkew.y };

            if( ImGui::DragFloat2( "Local Skew", localSkewBuffer, 0.001f, 0.0f, 0.0f, "%.3f" ) )
            {
                transformation->setLocalSkew( mt::vec2f( localSkewBuffer[0], localSkewBuffer[1] ) );
            }

            mt::vec3f localRotation = transformation->getLocalOrientation();
            float localRotationBuffer[3] = { MENGINE_RAD2DEG( localRotation.x ), MENGINE_RAD2DEG( localRotation.y ), MENGINE_RAD2DEG( localRotation.z ) };

            bool rotationChanged = false;
            rotationChanged |= this->drawRotationAxisControl_( "X", localRotationBuffer[0] );
            rotationChanged |= this->drawRotationAxisControl_( "Y", localRotationBuffer[1] );
            rotationChanged |= this->drawRotationAxisControl_( "Z", localRotationBuffer[2] );

            if( rotationChanged == true )
            {
                transformation->setLocalOrientation( mt::vec3f( MENGINE_DEG2RAD( localRotationBuffer[0] ), MENGINE_DEG2RAD( localRotationBuffer[1] ), MENGINE_DEG2RAD( localRotationBuffer[2] ) ) );
            }

            const mt::vec3f & worldPosition = transformation->getWorldPosition();
            const mt::vec3f & worldScale = transformation->getWorldScale();
            const mt::vec3f & worldRotation = transformation->getWorldOrientation();

            float worldPositionBuffer[3] = { worldPosition.x, worldPosition.y, worldPosition.z };
            float worldScaleBuffer[3] = { worldScale.x, worldScale.y, worldScale.z };
            float worldRotationBuffer[3] = { MENGINE_RAD2DEG( worldRotation.x ), MENGINE_RAD2DEG( worldRotation.y ), MENGINE_RAD2DEG( worldRotation.z ) };

            ImGui::BeginDisabled();
            ImGui::InputFloat3( "World Position", worldPositionBuffer, "%.3f" );
            ImGui::InputFloat3( "World Scale", worldScaleBuffer, "%.3f" );
            ImGui::InputFloat3( "World Rotation", worldRotationBuffer, "%.3f" );
            ImGui::EndDisabled();
        }

        if( render != nullptr && ImGui::CollapsingHeader( "Render", ImGuiTreeNodeFlags_DefaultOpen ) )
        {
            bool renderEnable = render->isRenderEnable();
            if( ImGui::Checkbox( "Enabled", &renderEnable ) )
            {
                render->setRenderEnable( renderEnable );
            }

            bool renderHide = render->isHide();
            if( ImGui::Checkbox( "Hidden", &renderHide ) )
            {
                render->setHide( renderHide );
            }

            bool renderLocalHide = render->isLocalHide();
            if( ImGui::Checkbox( "Local Hidden", &renderLocalHide ) )
            {
                render->setLocalHide( renderLocalHide );
            }

            int32_t zGroup = static_cast<int32_t>( render->getZGroup() );
            if( ImGui::InputInt( "Z Group", &zGroup ) )
            {
                render->setZGroup( static_cast<ZGroupType>( zGroup ) );
            }

            int32_t zIndex = static_cast<int32_t>( render->getZIndex() );
            if( ImGui::InputInt( "Z Index", &zIndex ) )
            {
                render->setZIndex( static_cast<ZIndexType>( zIndex ) );
            }
        }

        if( animation != nullptr && ImGui::CollapsingHeader( "Animation", ImGuiTreeNodeFlags_None ) )
        {
            bool playing = animation->isPlay();
            bool paused = animation->isPause();
            bool loop = animation->isLoop();
            bool autoPlay = animation->isAutoPlay();
            bool interrupt = animation->isInterrupt();
            bool lastCycle = animation->isLastCicle();

            ImGui::BeginDisabled();
            ImGui::Checkbox( "Playing", &playing );
            ImGui::Checkbox( "Paused", &paused );
            ImGui::Checkbox( "Loop", &loop );
            ImGui::Checkbox( "Auto", &autoPlay );
            ImGui::Checkbox( "Interrupt", &interrupt );
            ImGui::Checkbox( "Last Cycle", &lastCycle );

            float time = animation->getTime();
            float duration = animation->getDuration();
            float speed = animation->getAnimationSpeedFactor();
            float stretch = animation->getStretch();
            float intervalBegin = animation->getIntervalBegin();
            float intervalEnd = animation->getIntervalEnd();
            uint32_t playCount = animation->getPlayCount();
            uint32_t playIterator = animation->getPlayIterator();
            uint32_t playId = static_cast<uint32_t>( animation->getPlayId() );

            ImGui::InputFloat( "Time", &time, 0.0f, 0.0f, "%.3f" );
            ImGui::InputFloat( "Duration", &duration, 0.0f, 0.0f, "%.3f" );
            ImGui::InputFloat( "Speed", &speed, 0.0f, 0.0f, "%.3f" );
            ImGui::InputFloat( "Stretch", &stretch, 0.0f, 0.0f, "%.3f" );
            float intervalValues[2] = { intervalBegin, intervalEnd };
            ImGui::InputFloat2( "Interval", intervalValues, "%.3f" );
            ImGui::InputScalar( "Play Count", ImGuiDataType_U32, &playCount );
            ImGui::InputScalar( "Play Iterator", ImGuiDataType_U32, &playIterator );
            ImGui::InputScalar( "Play Id", ImGuiDataType_U32, &playId );
            ImGui::EndDisabled();
        }

        if( picker != nullptr && ImGui::CollapsingHeader( "Picker", ImGuiTreeNodeFlags_None ) )
        {
            bool pickerEnable = picker->isPickerEnable();
            bool pickerFreeze = picker->isPickerFreeze();
            bool pickerExclusive = picker->isPickerExclusive();
            bool pickerDummy = picker->isPickerDummy();

            ImGui::BeginDisabled();
            ImGui::Checkbox( "Enabled", &pickerEnable );
            ImGui::Checkbox( "Freeze", &pickerFreeze );
            ImGui::Checkbox( "Exclusive", &pickerExclusive );
            ImGui::Checkbox( "Dummy", &pickerDummy );
            ImGui::EndDisabled();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void LayoutEditorModule::renderImGUIConsole()
    {
        if( m_logger == nullptr )
        {
            return;
        }

        const Vector<LoggerRecordInterfacePtr> & records = m_logger->getRecords();

        // Collect unique categories
        Vector<String> uniqueCategories;
        for( const LoggerRecordInterfacePtr & record : records )
        {
            LoggerMessage msg;
            record->getMessage( &msg );
            
            String category( msg.category );
            
            bool found = false;
            for( const String & existingCategory : uniqueCategories )
            {
                if( existingCategory == category )
                {
                    found = true;
                    break;
                }
            }
            
            if( found == false )
            {
                uniqueCategories.emplace_back( category );
                
                // Add to filter map if not exists
                if( m_consoleFilterCategories.find( category ) == m_consoleFilterCategories.end() )
                {
                    m_consoleFilterCategories[category] = true;
                }
            }
        }

        // Toolbar
        ImGui::Text( "Filter:" );
        ImGui::SameLine();
        ImGui::SetNextItemWidth( 200.0f );
        ImGui::InputText( "##ConsoleFilter", m_consoleFilterBuffer, 256 );
        
        ImGui::SameLine();
        if( ImGui::Button( "Clear" ) )
        {
            m_logger->clearRecords();
        }
        
        ImGui::SameLine();
        ImGui::Checkbox( "Auto-scroll", &m_consoleAutoScroll );
        
        ImGui::Separator();

        // Left panel with filters
        ImGui::BeginChild( "LeftPanel", ImVec2( 200.0f, 0 ), true );
        
        // Level filters
        ImGui::Text( "Levels:" );
        ImGui::Separator();
        
        const Char * levelNames[] = { "S", "F", "R", "E", "W", "M", "I", "D", "V" };
        const Char * levelTooltips[] = {
            "Silent",
            "Fatal",
            "Message Release",
            "Error",
            "Warning",
            "Message",
            "Info",
            "Debug",
            "Verbose"
        };
        
        ImGui::Indent( 8.0f );
        
        for( uint32_t i = 1; i < LOGGER_LEVEL_COUNT; ++i ) // Skip LM_SILENT (0)
        {
            if( i > 1 )
            {
                ImGui::SameLine( 0.0f, 12.0f ); // Spacing of 12 pixels
            }
            
            ImGui::PushID( (int)i );
            
            // Color: green if enabled, gray if disabled
            ImVec4 color = m_consoleFilterLevels[i] ? ImVec4( 0.3f, 1.0f, 0.3f, 1.0f ) : ImVec4( 0.5f, 0.5f, 0.5f, 1.0f );
            ImGui::PushStyleColor( ImGuiCol_Text, color );
            
            ImGui::Selectable( levelNames[i], false, 0, ImVec2( 12.0f, 0.0f ) );
            
            if( ImGui::IsItemClicked() )
            {
                m_consoleFilterLevels[i] = !m_consoleFilterLevels[i];
            }
            
            ImGui::PopStyleColor();
            
            if( ImGui::IsItemHovered() )
            {
                ImGui::SetTooltip( "%s", levelTooltips[i] );
            }
            
            ImGui::PopID();
        }
        
        ImGui::Unindent( 8.0f );
        
        ImGui::Spacing();
        ImGui::Separator();
        
        // Category filters
        ImGui::Text( "Categories:" );
        ImGui::Separator();
        
        ImGui::Indent( 8.0f );
        
        for( const String & category : uniqueCategories )
        {
            bool isEnabled = m_consoleFilterCategories[category];
            bool isExclusive = (m_consoleExclusiveCategory.empty() == false && m_consoleExclusiveCategory == category);
            bool isGrayed = (m_consoleExclusiveCategory.empty() == false && m_consoleExclusiveCategory != category);
            
            if( isGrayed )
            {
                ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.5f, 0.5f, 0.5f, 1.0f ) );
            }
            
            bool clicked = ImGui::Checkbox( category.c_str(), &isEnabled );
            
            if( isGrayed )
            {
                ImGui::PopStyleColor();
            }
            
            if( clicked )
            {
                bool ctrlPressed = ImGui::GetIO().KeyCtrl;
                
                if( ctrlPressed )
                {
                    // Exclusive mode with CTRL
                    if( isExclusive )
                    {
                        // Disable exclusive mode, restore checkbox state
                        m_consoleExclusiveCategory.clear();
                        isEnabled = m_consoleFilterCategories[category]; // Restore original state
                    }
                    else
                    {
                        // Enable exclusive mode for this category
                        m_consoleExclusiveCategory = category;
                        isEnabled = m_consoleFilterCategories[category]; // Keep original state
                    }
                }
                else
                {
                    // Normal mode - update filter
                    m_consoleFilterCategories[category] = isEnabled;
                }
            }
        }
        
        ImGui::Unindent( 8.0f );
        
        ImGui::EndChild();
        
        ImGui::SameLine();
        
        // Log messages area
        ImGui::BeginChild( "LogScrolling", ImVec2( 0, 0 ), false, ImGuiWindowFlags_HorizontalScrollbar );
        
        const Char * filterText = m_consoleFilterBuffer[0] != '\0' ? m_consoleFilterBuffer : nullptr;

        for( const LoggerRecordInterfacePtr & record : records )
        {
            LoggerMessage message;
            record->getMessage( &message );

            // Skip SILENT level
            if( message.level == LM_SILENT )
            {
                continue;
            }

            // Filter by level
            if( m_consoleFilterLevels[message.level] == false )
            {
                continue;
            }
            
            // Filter by category
            if( m_consoleExclusiveCategory.empty() == false )
            {
                // Exclusive mode - show only this category
                if( StdString::strcmp( message.category, m_consoleExclusiveCategory.c_str() ) != 0 )
                {
                    continue;
                }
            }
            else
            {
                // Normal mode - check filter
                String categoryStr( message.category );
                auto it = m_consoleFilterCategories.find( categoryStr );
                if( it != m_consoleFilterCategories.end() && it->second == false )
                {
                    continue;
                }
            }
            
            // Filter by text
            if( filterText != nullptr && StdString::strstr( message.data, filterText ) == nullptr )
            {
                continue;
            }
            
            // Build log line
            Char logLine[MENGINE_LOGGER_MAX_MESSAGE] = {'\0'};
            Char tempBuffer[256];
            
            // Timestamp
            Helper::makeLoggerShortDate( message.timestamp, "[%02u:%02u:%02u:%04u] ", tempBuffer, 0, 255 );
            StdString::strcat( logLine, tempBuffer );
            
            // Level symbol
            Char levelSymbol = Helper::getLoggerLevelSymbol( message.level );
            tempBuffer[0] = levelSymbol;
            tempBuffer[1] = ' ';
            tempBuffer[2] = '\0';
            StdString::strcat( logLine, tempBuffer );
            
            // Thread stamp
            if( message.thread.empty() == false )
            {
                Helper::makeLoggerThreadStamp( message.thread, "|%s| ", tempBuffer, 0, 255 );
                StdString::strcat( logLine, tempBuffer );
            }
            
            // Category
            StdString::strcat( logLine, "[" );
            StdString::strcat( logLine, message.category );
            StdString::strcat( logLine, "] " );
            
            // Message
            size_t currentLength = StdString::strlen( logLine );
            size_t remainingSpace = MENGINE_LOGGER_MAX_MESSAGE - currentLength - 1;
            if( message.size < remainingSpace )
            {
                StdString::strncat( logLine, message.data, message.size );
            }
            else
            {
                StdString::strncat( logLine, message.data, remainingSpace );
            }
            
            // Color by level
            ImVec4 color;
            
            switch( message.level )
            {
            case LM_FATAL:
                color = ImVec4( 1.0f, 0.0f, 0.0f, 1.0f ); // Red
                break;
            case LM_MESSAGE_RELEASE:
                color = ImVec4( 1.0f, 0.5f, 0.0f, 1.0f ); // Orange
                break;
            case LM_ERROR:
                color = ImVec4( 1.0f, 0.4f, 0.4f, 1.0f ); // Light Red
                break;
            case LM_WARNING:
                color = ImVec4( 1.0f, 0.8f, 0.0f, 1.0f ); // Yellow
                break;
            case LM_MESSAGE:
                color = ImVec4( 0.8f, 0.8f, 1.0f, 1.0f ); // Light Blue
                break;
            case LM_INFO:
                color = ImVec4( 0.7f, 0.7f, 0.7f, 1.0f ); // Gray
                break;
            case LM_DEBUG:
                color = ImVec4( 0.6f, 0.6f, 0.6f, 1.0f ); // Dark Gray
                break;
            case LM_VERBOSE:
                color = ImVec4( 0.5f, 0.5f, 0.5f, 1.0f ); // Darker Gray
                break;
            default:
                color = ImVec4( 1.0f, 1.0f, 1.0f, 1.0f ); // White
                break;
            }
            
            ImGui::PushStyleColor( ImGuiCol_Text, color );
            ImGui::TextUnformatted( logLine );
            ImGui::PopStyleColor();
        }
        
        if( m_consoleAutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY() )
        {
            ImGui::SetScrollHereY( 1.0f );
        }
        
        ImGui::EndChild();
    }
    //////////////////////////////////////////////////////////////////////////
    bool LayoutEditorModule::createViewportRenderTarget_( uint32_t _width, uint32_t _height )
    {
        if( _width == 0 || _height == 0 )
        {
            return false;
        }

        // Destroy previous render target if exists
        this->destroyViewportRenderTarget_();

        // Create render target texture
        RenderTargetInterfacePtr renderTarget = RENDER_SYSTEM()
            ->createRenderTargetTexture( _width, _height, PF_A8R8G8B8, MENGINE_DOCUMENT_FACTORABLE );

        if( renderTarget == nullptr )
        {
            LOGGER_ERROR( "LayoutEditor: failed to create viewport render target [%u x %u]", _width, _height );
            return false;
        }

        // Create render image from render target
        RenderImageInterfacePtr renderImage = RENDER_SYSTEM()
            ->createRenderImageTarget( renderTarget, MENGINE_DOCUMENT_FACTORABLE );

        if( renderImage == nullptr )
        {
            LOGGER_ERROR( "LayoutEditor: failed to create viewport render image [%u x %u]", _width, _height );
            return false;
        }

        // Create render texture from render image
        RenderTextureInterfacePtr renderTexture = RENDERTEXTURE_SERVICE()
            ->createRenderTexture( renderImage, _width, _height, MENGINE_DOCUMENT_FACTORABLE );

        if( renderTexture == nullptr )
        {
            LOGGER_ERROR( "LayoutEditor: failed to create viewport render texture [%u x %u]", _width, _height );
            return false;
        }

        m_viewportRenderTarget = renderTarget;
        m_viewportRenderTexture = renderTexture;
        m_viewportWidth = _width;
        m_viewportHeight = _height;

        LOGGER_INFO( "layout_editor", "viewport render target created [%u x %u]", _width, _height );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void LayoutEditorModule::destroyViewportRenderTarget_()
    {
        m_viewportRenderTexture = nullptr;
        m_viewportRenderTarget = nullptr;
        m_viewportWidth = 0;
        m_viewportHeight = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    void LayoutEditorModule::renderSceneToViewport_()
    {
        if( m_viewportRenderTarget == nullptr || m_viewportRenderPipeline == nullptr )
        {
            return;
        }

        if( m_viewportRenderTarget->begin() == false )
        {
            LOGGER_ERROR( "LayoutEditor: failed to begin viewport render target" );
            return;
        }

        // Clear the framebuffer with a background color
        Color clearColor = Helper::makeColor8( 45, 45, 48, 255 ); // Dark gray background
        RENDER_SYSTEM()->clearFrameBuffer( FBT_COLOR, clearColor, 1.0, 0 );

        // Setup viewport for the render target size
        Viewport viewport;
        viewport.begin.x = 0.0f;
        viewport.begin.y = 0.0f;
        viewport.end.x = (float)m_viewportWidth;
        viewport.end.y = (float)m_viewportHeight;

        RENDER_SYSTEM()->setViewport( viewport );

        const ScenePtr & scene = SCENE_SERVICE()
            ->getCurrentScene();

        // Prepare render context (copy from player service if available)
        const RenderContext * playerContext = PLAYER_SERVICE()
            ->getRenderContext();

        RenderContext renderContext;

        if( playerContext != nullptr )
        {
            renderContext = *playerContext;
        }
        else
        {
            renderContext.resolution = nullptr;
            renderContext.viewport = nullptr;
            renderContext.camera = nullptr;
            renderContext.transformation = nullptr;
            renderContext.scissor = nullptr;
            renderContext.target = nullptr;
            renderContext.zGroup = MENGINE_RENDER_ZGROUP_DEFAULT;
            renderContext.zIndex = MENGINE_RENDER_ZINDEX_DEFAULT;
        }

        // We manage render target binding manually
        renderContext.target = nullptr;

        m_viewportRenderPipeline->prepare();

        if( scene != nullptr )
        {
            Helper::nodeRenderChildren( scene.get(), m_viewportRenderPipeline, &renderContext, false );
        }

        BatchRenderPipeline * batchPipeline = static_cast<BatchRenderPipeline *>( m_viewportRenderPipeline.get() );

        if( batchPipeline == nullptr )
        {
            LOGGER_ERROR( "LayoutEditor: invalid render pipeline type" );
            m_viewportRenderTarget->end();
            return;
        }

        m_viewportRenderPipeline->flush();

        if( batchPipeline->lockBatches() == false )
        {
            LOGGER_ERROR( "LayoutEditor: failed to lock render batches" );
            m_viewportRenderPipeline->clear();
            m_viewportRenderTarget->end();
            return;
        }

        m_viewportRenderPipeline->batch();
        m_viewportRenderPipeline->render();
        batchPipeline->unlockBatches();
        m_viewportRenderPipeline->clear();

        m_viewportRenderTarget->end();
    }
    //////////////////////////////////////////////////////////////////////////
    void LayoutEditorModule::renderViewportInImGui_()
    {
        ImVec2 fullSize = ImGui::GetContentRegionAvail();

        if( fullSize.x <= 1.f || fullSize.y <= 1.f )
        {
            ImGui::TextUnformatted( "Viewport area is too small" );
            return;
        }

        float controlsHeight = ImGui::GetFrameHeightWithSpacing() * 1.6f;
        controlsHeight = std::min( controlsHeight, fullSize.y * 0.35f );
        float canvasHeight = std::max( fullSize.y - controlsHeight, 1.f );

        float deviceWidth = (float)m_viewportDeviceResolution.getWidth();
        float deviceHeight = (float)m_viewportDeviceResolution.getHeight();

        float targetAspect = 0.f;

        if( deviceWidth > 0.f && deviceHeight > 0.f )
        {
            targetAspect = deviceWidth / deviceHeight;
        }
        else
        {
            const RenderResolutionInterfacePtr & renderResolution = PLAYER_SERVICE()
                ->getRenderResolution();

            if( renderResolution != nullptr )
            {
                const Resolution & contentResolution = renderResolution->getContentResolution();

                if( contentResolution.getHeight() != 0 )
                {
                    targetAspect = contentResolution.getWidthF() / contentResolution.getHeightF();
                    deviceWidth = contentResolution.getWidthF();
                    deviceHeight = contentResolution.getHeightF();
                }
            }
        }

        if( targetAspect <= 0.f )
        {
            targetAspect = fullSize.x / fullSize.y;
            deviceWidth = fullSize.x;
            deviceHeight = fullSize.y;
        }

        ImGui::PushID( "LayoutEditorViewport" );

        if( ImGui::BeginChild( "ViewportCanvas", ImVec2( fullSize.x, canvasHeight ), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse ) )
        {
            ImVec2 canvasSize = ImGui::GetContentRegionAvail();
            if( canvasSize.x <= 0.f ) canvasSize.x = 1.f;
            if( canvasSize.y <= 0.f ) canvasSize.y = 1.f;
            ImVec2 canvasStartCursor = ImGui::GetCursorPos();
            ImVec2 canvasStartScreen = ImGui::GetCursorScreenPos();

            ImVec2 framePadding( m_viewportDeviceFrameWidth, m_viewportDeviceFrameWidth );

            ImVec2 maxOuterSize( canvasSize.x * m_viewportDeviceFitScale, canvasSize.y * m_viewportDeviceFitScale );
            maxOuterSize.x = std::max( maxOuterSize.x, 1.f );
            maxOuterSize.y = std::max( maxOuterSize.y, 1.f );

            ImVec2 maxContentSize( maxOuterSize.x - framePadding.x * 2.f, maxOuterSize.y - framePadding.y * 2.f );
            maxContentSize.x = std::max( maxContentSize.x, 1.f );
            maxContentSize.y = std::max( maxContentSize.y, 1.f );

            ImVec2 contentSize = maxContentSize;

            if( deviceWidth > 0.f && deviceHeight > 0.f )
            {
                float scale = std::min( maxContentSize.x / deviceWidth, maxContentSize.y / deviceHeight );
                scale = std::max( scale, 0.0001f );
                contentSize.x = deviceWidth * scale;
                contentSize.y = deviceHeight * scale;
            }
            else if( targetAspect > 0.f )
            {
                float maxAspect = maxContentSize.x / maxContentSize.y;

                if( maxAspect > targetAspect )
                {
                    contentSize.y = maxContentSize.y;
                    contentSize.x = contentSize.y * targetAspect;
                }
                else
                {
                    contentSize.x = maxContentSize.x;
                    contentSize.y = contentSize.x / targetAspect;
                }
            }

            contentSize.x = std::max( 1.f, contentSize.x );
            contentSize.y = std::max( 1.f, contentSize.y );

            ImVec2 outerSize( contentSize.x + framePadding.x * 2.f, contentSize.y + framePadding.y * 2.f );

            ImVec2 deviceOffset( (canvasSize.x - outerSize.x) * 0.5f, (canvasSize.y - outerSize.y) * 0.5f );
            if( deviceOffset.x < 0.f ) deviceOffset.x = 0.f;
            if( deviceOffset.y < 0.f ) deviceOffset.y = 0.f;

            uint32_t width = (uint32_t)std::max( 1.0f, std::floor( contentSize.x + 0.5f ) );
            uint32_t height = (uint32_t)std::max( 1.0f, std::floor( contentSize.y + 0.5f ) );

            if( m_viewportRenderTarget == nullptr || m_viewportWidth != width || m_viewportHeight != height )
            {
                if( this->createViewportRenderTarget_( width, height ) == false )
                {
                    ImGui::TextUnformatted( "Failed to create viewport render target" );
                    ImGui::EndChild();
                    ImGui::PopID();
                    return;
                }
            }

            this->renderSceneToViewport_();

            ImVec2 deviceScreenMin( canvasStartScreen.x + deviceOffset.x, canvasStartScreen.y + deviceOffset.y );
            ImVec2 imageCursor( canvasStartCursor.x + deviceOffset.x + framePadding.x, canvasStartCursor.y + deviceOffset.y + framePadding.y );

            if( m_viewportRenderTexture != nullptr )
            {
                const mt::uv4f & uv = m_viewportRenderTarget->getUV();

                if( m_showViewportDeviceFrame == true )
                {
                    ImDrawList * drawList = ImGui::GetWindowDrawList();

                    auto clampColor = []( float v )
                    {
                        return std::max( 0.f, std::min( v, 1.f ) );
                    };

                    float borderMultiplier = 1.2f;
                    ImVec4 frameColorVec( m_viewportDeviceFrameColor.getR(), m_viewportDeviceFrameColor.getG(), m_viewportDeviceFrameColor.getB(), m_viewportDeviceFrameColor.getA() );
                    ImVec4 borderColorVec( clampColor( m_viewportDeviceFrameColor.getR() * borderMultiplier ),
                        clampColor( m_viewportDeviceFrameColor.getG() * borderMultiplier ),
                        clampColor( m_viewportDeviceFrameColor.getB() * borderMultiplier ),
                        1.0f );

                    ImU32 frameColor = ImGui::ColorConvertFloat4ToU32( frameColorVec );
                    ImU32 borderColor = ImGui::ColorConvertFloat4ToU32( borderColorVec );

                    ImVec2 frameMax = ImVec2( deviceScreenMin.x + outerSize.x, deviceScreenMin.y + outerSize.y );

                    drawList->AddRectFilled( deviceScreenMin, frameMax, frameColor, m_viewportDeviceCornerRadius );
                    drawList->AddRect( deviceScreenMin, frameMax, borderColor, m_viewportDeviceCornerRadius, 0, m_viewportDeviceFrameWidth * 0.5f );
                }

                ImGui::SetCursorPos( imageCursor );

                ImTextureID textureID = m_imguiRenderProvider->getImTexture( m_viewportRenderTexture );
                ImGui::Image( textureID, contentSize,
                    ImVec2( uv.p0.x, uv.p0.y ),
                    ImVec2( uv.p2.x, uv.p2.y )
                );
            }
            else
            {
                ImGui::SetCursorPos( imageCursor );
                ImGui::TextUnformatted( "Viewport texture not available" );
            }

            float consumedHeight = deviceOffset.y + outerSize.y;
            float remainingHeight = canvasSize.y - consumedHeight;
            if( remainingHeight > 0.f )
            {
                ImGui::Dummy( ImVec2( 1.f, remainingHeight ) );
            }
        }
        ImGui::EndChild();

        ImGui::PushStyleColor( ImGuiCol_ChildBg, ImVec4( 0.08f, 0.08f, 0.10f, 0.9f ) );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 8.f, 6.f ) );
        ImGui::PushStyleVar( ImGuiStyleVar_FrameBorderSize, 0.f );
        if( ImGui::BeginChild( "ViewportControls", ImVec2( fullSize.x, controlsHeight ), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse ) )
        {
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted( "Viewport Tools" );
            ImGui::SameLine();
            ImGui::Dummy( ImVec2( ImGui::GetStyle().ItemSpacing.x * 0.5f, 0.f ) );
            ImGui::SameLine();

            const char * deviceButtonLabel = m_showViewportDeviceFrame == true ? "Hide Device Frame" : "Show Device Frame";
            if( ImGui::Button( deviceButtonLabel ) )
            {
                m_showViewportDeviceFrame = !m_showViewportDeviceFrame;
            }

            ImGui::SameLine();
            ImGui::Dummy( ImVec2( ImGui::GetStyle().ItemSpacing.x, 0.f ) );
            ImGui::SameLine();
            ImGui::Text( "Device %.0fx%.0f (aspect %.3f)", deviceWidth, deviceHeight, targetAspect );
        }
        ImGui::EndChild();
        ImGui::PopStyleVar( 2 );
        ImGui::PopStyleColor();
        ImGui::PopID();
    }
    //////////////////////////////////////////////////////////////////////////
    NodePtr LayoutEditorModule::getObjectEntityNode_( const pybind::object & _obj ) const
    {
        if( m_module == nullptr || _obj.is_none() == true )
        {
            return nullptr;
        }

        const pybind::module & m = m_module->getModule();

        pybind::object nodeObject = m.call_method( "GetLayoutEditorObjectEntityNode", _obj );

        if( nodeObject.is_none() == true )
        {
            return nullptr;
        }

        NodePtr node = nodeObject.extract();

        return node;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LayoutEditorModule::drawRotationAxisControl_( const Char * _label, float & _value )
    {
        ImGui::PushID( _label );

        float revolutionsFloat = _value / 360.f;
        int revolutions = (int)std::floor( revolutionsFloat );
        float remainder = _value - (float)revolutions * 360.f;

        if( remainder < 0.f )
        {
            remainder += 360.f;
            --revolutions;
        }

        bool changed = false;

        ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.7f, 0.8f, 1.0f, 1.0f ) );
        ImGui::PushStyleColor( ImGuiCol_FrameBg, ImVec4( 0.f, 0.f, 0.f, 0.f ) );
        ImGui::PushStyleColor( ImGuiCol_FrameBgHovered, ImVec4( 0.f, 0.f, 0.f, 0.f ) );
        ImGui::PushStyleColor( ImGuiCol_FrameBgActive, ImVec4( 0.f, 0.f, 0.f, 0.f ) );

        ImGui::SetNextItemWidth( 15.f );
        if( ImGui::DragInt( "##RevolutionControl", &revolutions, 1.0f, -100000, 100000, "%d" ) )
        {
            changed = true;
        }

        ImGui::PopStyleColor( 3 );
        ImGui::PopStyleColor();

        ImGui::SameLine();
        ImGui::SetNextItemWidth( 5.f );
        ImGui::TextUnformatted( "x" );
        ImGui::SameLine();

        ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.4f, 0.6f, 1.0f, 1.0f ) );
        ImGui::PushStyleColor( ImGuiCol_FrameBg, ImVec4( 0.f, 0.f, 0.f, 0.f ) );
        ImGui::PushStyleColor( ImGuiCol_FrameBgHovered, ImVec4( 0.f, 0.f, 0.f, 0.f ) );
        ImGui::PushStyleColor( ImGuiCol_FrameBgActive, ImVec4( 0.f, 0.f, 0.f, 0.f ) );

        ImGui::SetNextItemWidth( 50.f );
        if( ImGui::DragFloat( "##AngleControl", &remainder, 0.01f, 0.0f, 0.0f, "%.3f" ) )
        {
            if( remainder >= 360.f )
            {
                remainder = remainder - 360.f;
                ++revolutions;
            }
            else if( remainder < 0.f )
            {
                remainder = 360.f + remainder;
                --revolutions;
            }

            if( remainder < 0.f )
            {
                remainder = 0.f;
            }
            else if( remainder >= 360.f )
            {
                remainder = 0.f;
                ++revolutions;
            }

            changed = true;
        }

        ImGui::PopStyleColor( 3 );
        ImGui::PopStyleColor();

        ImGui::SameLine();

        float dialSize = ImGui::GetFrameHeight() * 1.8f;
        dialSize = std::max( dialSize, 32.0f );

        ImVec2 dialSizeVec( dialSize, dialSize );
        ImVec2 dialPos = ImGui::GetCursorScreenPos();
        ImGui::InvisibleButton( "##Dial", dialSizeVec, ImGuiButtonFlags_MouseButtonLeft );
        bool dialActive = ImGui::IsItemActive();
        ImGuiID dialId = ImGui::GetItemID();
        RotationDialState & dialState = s_rotationDialStates[dialId];

        ImVec2 dialCenter = ImVec2( dialPos.x + dialSize * 0.5f, dialPos.y + dialSize * 0.5f );
        float dialRadius = dialSize * 0.5f;

        ImDrawList * draw = ImGui::GetWindowDrawList();
        ImU32 baseColor = ImGui::GetColorU32( ImGuiCol_FrameBg );
        ImU32 activeColor = ImGui::GetColorU32( dialActive ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab );
        ImU32 borderColor = ImGui::GetColorU32( dialActive ? ImGuiCol_PlotHistogramHovered : ImGuiCol_Border );

        draw->AddCircleFilled( dialCenter, dialRadius, baseColor, 48 );
        draw->AddCircle( dialCenter, dialRadius, borderColor, 48, 1.2f );

        if( dialActive == true )
        {
            ImVec2 mousePos = ImGui::GetIO().MousePos;
            float deltaX = mousePos.x - dialCenter.x;
            float deltaY = mousePos.y - dialCenter.y;
            float angle = std::atan2( deltaX, -deltaY );
            float angleDegrees = MENGINE_RAD2DEG( angle );

            if( angleDegrees < 0.f )
            {
                angleDegrees += 360.f;
            }

            float diff = angleDegrees - dialState.lastRemainder;

            if( diff > 180.f )
            {
                --revolutions;
            }
            else if( diff < -180.f )
            {
                ++revolutions;
            }

            remainder = angleDegrees;
            dialState.lastRemainder = angleDegrees;
            changed = true;
        }
        else
        {
            dialState.lastRemainder = remainder;
        }

        float pointerAngleRad = MENGINE_DEG2RAD( remainder );
        ImVec2 pointerDir = ImVec2( std::sin( pointerAngleRad ), -std::cos( pointerAngleRad ) );
        ImVec2 pointerEnd = ImVec2( dialCenter.x + pointerDir.x * (dialRadius - 4.0f), dialCenter.y + pointerDir.y * (dialRadius - 4.0f) );
        draw->AddLine( dialCenter, pointerEnd, activeColor, 2.0f );
        draw->AddCircleFilled( dialCenter, 2.5f, activeColor );

        if( changed == true )
        {
            if( remainder >= 360.f )
            {
                remainder -= 360.f;
                ++revolutions;
            }
            else if( remainder < 0.f )
            {
                remainder += 360.f;
                --revolutions;
            }

            _value = (float)revolutions * 360.f + remainder;
        }

        ImGui::Spacing();
        ImGui::PopID();

        return changed;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LayoutEditorModule::drawVec3Attribute_( mt::vec3f & _value, float _step, bool _clamp, float _minValue, float _maxValue )
    {
        bool changed = false;

        const char * axisLabels[3] = { "X", "Y", "Z" };
        float * axisValues[3] = { &_value.x, &_value.y, &_value.z };

        ImGuiStyle & style = ImGui::GetStyle();
        float spacing = style.ItemInnerSpacing.x;
        float available = ImGui::GetContentRegionAvail().x;
        float itemWidth = (available - spacing * 2.f) / 3.f;

        if( itemWidth < 40.f )
        {
            itemWidth = 40.f;
        }

        ImDrawList * draw = ImGui::GetWindowDrawList();
        ImU32 labelColor = ImGui::GetColorU32( ImGuiCol_TextDisabled );

        for( int32_t i = 0; i != 3; ++i )
        {
            if( i != 0 )
            {
                ImGui::SameLine( 0.0f, spacing );
            }

            ImGui::PushID( i );

            ImVec2 cursorPos = ImGui::GetCursorScreenPos();

            ImGui::SetNextItemWidth( itemWidth );

            float minValue = _clamp == true ? _minValue : -FLT_MAX;
            float maxValue = _clamp == true ? _maxValue : FLT_MAX;

            if( ImGui::DragFloat( "##AxisValue", axisValues[i], _step, minValue, maxValue, "%.3f" ) )
            {
                if( _clamp == true )
                {
                    if( *axisValues[i] < _minValue )
                    {
                        *axisValues[i] = _minValue;
                    }
                    else if( *axisValues[i] > _maxValue )
                    {
                        *axisValues[i] = _maxValue;
                    }
                }

                changed = true;
            }

            ImVec2 textPos = cursorPos;
            textPos.x += style.FramePadding.x + 2.0f;
            textPos.y += style.FramePadding.y;

            draw->AddText( textPos, labelColor, axisLabels[i] );

            ImGui::PopID();
        }

        return changed;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LayoutEditorModule::drawOrientationAttribute_( mt::vec3f & _value )
    {
        bool changed = false;

        ImGui::PushID( "OrientationAttribute" );

        float angles[3] = { _value.x, _value.y, _value.z };
        const char * axisLabels[3] = { "X", "Y", "Z" };

        for( int32_t i = 0; i != 3; ++i )
        {
            if( i != 0 )
            {
                ImGui::Spacing();
            }

            ImGui::PushID( i );
            bool axisChanged = this->drawRotationAxisControl_( axisLabels[i], angles[i] );
            ImGui::PopID();

            if( axisChanged == true )
            {
                changed = true;
            }
        }

        if( changed == true )
        {
            _value.x = angles[0];
            _value.y = angles[1];
            _value.z = angles[2];
        }

        ImGui::PopID();

        return changed;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LayoutEditorModule::drawRGBAttribute_( mt::vec3f & _value )
    {
        bool changed = false;

        ImGuiStyle & style = ImGui::GetStyle();
        float spacing = style.ItemInnerSpacing.x;
        float available = ImGui::GetContentRegionAvail().x;

        float totalWeights = 2.f + 2.f + 2.f + 1.f;
        float baseWidth = (available - spacing * 3.f) / totalWeights;

        float sliderWidth = std::max( baseWidth * 2.f, 40.f );
        float colorEditWidth = std::max( baseWidth, 40.f );

        const char * axisLabels[3] = { "R", "G", "B" };
        float * axisValues[3] = { &_value.x, &_value.y, &_value.z };
        ImVec4 axisColors[3] = {
            ImVec4( 1.0f, 0.0f, 0.0f, 1.0f ),
            ImVec4( 0.0f, 1.0f, 0.0f, 1.0f ),
            ImVec4( 0.0f, 0.0f, 1.0f, 1.0f )
        };

        for( int32_t i = 0; i != 3; ++i )
        {
            if( i != 0 )
            {
                ImGui::SameLine( 0.0f, spacing );
            }

            float intensity = std::clamp( *axisValues[i], 0.f, 1.f );
            ImVec4 baseColor = axisColors[i];
            baseColor.x *= intensity;
            baseColor.y *= intensity;
            baseColor.z *= intensity;
            baseColor.w = 1.0f;

            ImGui::PushStyleColor( ImGuiCol_FrameBg, baseColor );
            ImGui::PushStyleColor( ImGuiCol_FrameBgHovered, baseColor );
            ImGui::PushStyleColor( ImGuiCol_FrameBgActive, baseColor );

            ImGui::PushID( i );
            ImVec2 cursorPos = ImGui::GetCursorScreenPos();
            ImGui::SetNextItemWidth( sliderWidth );

            if( ImGui::DragFloat( "##ColorComponent", axisValues[i], 0.01f, 0.0f, 1.0f, "%.3f" ) )
            {
                *axisValues[i] = std::clamp( *axisValues[i], 0.f, 1.f );
                changed = true;
            }

            ImVec2 textPos = cursorPos;
            textPos.x += style.FramePadding.x + 2.0f;
            textPos.y += style.FramePadding.y;
            ImDrawList * draw = ImGui::GetWindowDrawList();
            draw->AddText( textPos, ImGui::GetColorU32( ImGuiCol_Text ), axisLabels[i] );

            ImGui::PopID();
            ImGui::PopStyleColor( 3 );
        }

        ImGui::SameLine( 0.0f, spacing );

        ImGui::PushID( "RGBColorEdit" );
        ImGui::SetNextItemWidth( colorEditWidth );
        if( ImGui::ColorEdit3( "##ColorEdit", _value.buff(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel ) )
        {
            changed = true;
        }
        ImGui::PopID();

        return changed;
    }
    //////////////////////////////////////////////////////////////////////////
    float LayoutEditorModule::getWidgetStep_( const pybind::object & _widget, float _default ) const
    {
        if( _widget.is_none() == true )
        {
            return _default;
        }

        if( _widget.has_attr( "getStep" ) == false )
        {
            return _default;
        }

        pybind::object stepObject = _widget.call_method( "getStep" );

        if( stepObject.is_none() == true )
        {
            return _default;
        }

        float step = stepObject.extract();

        if( step <= 0.f )
        {
            return _default;
        }

        return step;
    }
    //////////////////////////////////////////////////////////////////////////
}
