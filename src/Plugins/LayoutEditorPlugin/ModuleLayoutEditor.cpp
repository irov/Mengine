#include "ModuleLayoutEditor.h"

#include "Interface/RenderMaterialServiceInterface.h"
#include "Interface/SceneServiceInterface.h"
#include "Interface/SoundServiceInterface.h"

#include "Kernel/TimestampHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/NodeCast.h"
#include "Kernel/SceneHelper.h"

#include "Config/StdIntTypes.h"
#include "Config/StdIO.h"
#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ModuleLayoutEditor::ModuleLayoutEditor()
        : m_scene( nullptr )
        , m_enumeratorLayout( 0 )
        , m_selectedLayoutIndex( ~0 )
        , m_editedNameLayoutIndex( ~0 )
        , m_selectedWidgetIndex( ~0 )
        , m_editedNameWidgetIndex( ~0 )
        , m_show( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ModuleLayoutEditor::~ModuleLayoutEditor()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleLayoutEditor::_initializeModule()
    {
        const ImGUIRenderProviderInterfacePtr & imguiRenderProvider = IMGUI_SERVICE()
            ->getRenderProvider();

        m_imguiRenderProvider = imguiRenderProvider;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleLayoutEditor::_finalizeModule()
    {
        m_imguiRenderProvider = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleLayoutEditor::_handleKeyEvent( const InputKeyEvent & _event )
    {
        if( _event.code == KC_F1 && _event.isDown == true )
        {
            if( m_show == false )
            {
                m_fromScene = SCENE_SERVICE()
                    ->getCurrentScene();

                ScenePtr scene = Helper::makeScene( SceneEventReceiverInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE );

                scene->setName( STRINGIZE_STRING_LOCAL( "LayoutEditor" ) );

                SCENE_SERVICE()
                    ->setCurrentScene( scene, false, false, nullptr );
            }
            else
            {
                SCENE_SERVICE()
                    ->setCurrentScene( m_fromScene, false, false, nullptr );

                m_fromScene = nullptr;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleLayoutEditor::_beginUpdate( bool _focus )
    {
        MENGINE_UNUSED( _focus );

    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleLayoutEditor::_render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context )
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
    bool ModuleLayoutEditor::onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity )
    {
        MENGINE_UNUSED( _behavior );
        
        m_scene = Helper::staticNodeCast<Scene *>( _entity );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleLayoutEditor::onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        m_scene = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleLayoutEditor::onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleLayoutEditor::onEntityActivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        SOUND_SERVICE()
            ->mute( STRINGIZE_STRING_LOCAL( "LayoutEditor" ), true );

        m_show = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleLayoutEditor::onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        SOUND_SERVICE()
            ->mute( STRINGIZE_STRING_LOCAL( "LayoutEditor" ), false );

        m_show = false;
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleLayoutEditor::onRenderDrawPrimitives( const RenderPrimitive * _primitives, uint32_t _count ) const
    {
        MENGINE_UNUSED( _primitives );
        MENGINE_UNUSED( _count );

        m_imguiRenderProvider->newFrame();

        const_cast<ModuleLayoutEditor *>(this)->renderImGUI();

        m_imguiRenderProvider->endFrame();
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleLayoutEditor::renderImGUI()
    {
        ImGui::Begin( "Layout Editor" );

        this->renderImGUILayouts();
        this->renderImGUIWidgets();

        ImGui::End();
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleLayoutEditor::renderImGUILayouts()
    {
        if( ImGui::Button( "+" ) == true )
        {
            this->newLayout();
        }

        ImGui::SameLine();

        ImGui::BeginDisabled( m_selectedLayoutIndex == ~0 );
        if( ImGui::Button( "-" ) == true )
        {
            this->removeLayout();
        }
        ImGui::EndDisabled();

        ImGui::SameLine();

        ImGui::Text( "Layouts:" );

        ImGui::Separator();

        if( ImGui::IsWindowFocused( ImGuiFocusedFlags_RootAndChildWindows ) == true && ImGui::IsAnyItemActive() == false )
        {
            if( m_selectedLayoutIndex != ~0 && ImGui::IsKeyPressed( ImGuiKey_Enter ) == true )
            {
                m_editedNameLayoutIndex = m_selectedLayoutIndex;

                Layout & layout = m_layouts[m_selectedLayoutIndex];

                StdString::strcpy( m_editedBufferNameLayout, layout.name.c_str() );
            }

            if( ImGui::IsKeyPressed( ImGuiKey_Equal, false ) == true )
            {
                this->newLayout();
            }

            if( ImGui::IsKeyPressed( ImGuiKey_Minus, false ) == true )
            {
                this->removeLayout();
            }

            if( ImGui::IsKeyPressed( ImGuiKey_Delete, false ) == true )
            {
                this->removeLayout();
            }

            if( ImGui::IsKeyPressed( ImGuiKey_UpArrow, false ) == true )
            {
                if( m_selectedLayoutIndex > 0 )
                {
                    m_selectedLayoutIndex--;
                    m_editedNameLayoutIndex = ~0;
                }
            }

            if( ImGui::IsKeyPressed( ImGuiKey_DownArrow, false ) == true )
            {
                if( m_selectedLayoutIndex < (int)m_layouts.size() - 1 )
                {
                    m_selectedLayoutIndex++;
                    m_editedNameLayoutIndex = ~0;
                }
            }
        }

        ImGui::BeginChild( "Layouts", ImVec2( 0, 200 ), true );

        for( int32_t index = 0; index < (int32_t)m_layouts.size(); ++index )
        {
            Layout & layout = m_layouts[index];

            bool isSelected = (index == m_selectedLayoutIndex);

            ImGui::PushID( index );

            if( index != m_editedNameLayoutIndex )
            {
                if( ImGui::Selectable( layout.name.c_str(), isSelected ) == true )
                {
                    m_selectedLayoutIndex = index;
                }

                if( ImGui::BeginPopupContextItem() == true )
                {
                    if( ImGui::MenuItem( "Add" ) == true )
                    {
                        this->newLayout();
                    }

                    if( ImGui::MenuItem( "Remove" ) == true )
                    {
                        this->removeLayout();
                    }

                    ImGui::EndPopup();
                }

                if( ImGui::IsItemHovered() == true && ImGui::IsMouseDoubleClicked( ImGuiMouseButton_Left ) == true )
                {
                    m_editedNameLayoutIndex = index;

                    StdString::strcpy( m_editedBufferNameLayout, layout.name.c_str() );
                }
            }
            else
            {
                ImGui::SetKeyboardFocusHere();

                if( ImGui::InputText( "##layout_edit", m_editedBufferNameLayout, 256, ImGuiInputTextFlags_EnterReturnsTrue ) == true )
                {
                    layout.name = m_editedBufferNameLayout;

                    m_editedNameLayoutIndex = ~0;
                }
            }

            ImGui::PopID();
        }

        ImGui::EndChild();

        ImGui::Separator();
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleLayoutEditor::renderImGUIWidgets()
    {
        if( m_selectedLayoutIndex == ~0 )
        {
            return;
        }

        Layout & layout = m_layouts[m_selectedLayoutIndex];

        ImGui::Text( "Widgets:" );

        ImGui::BeginChild( "Widgets", ImVec2( 0, 200 ), true );

        for( int32_t index = 0; index < (int32_t)layout.widgets.size(); ++index )
        {
            Widget & widget = layout.widgets[index];

            bool isSelected = (index == m_selectedWidgetIndex);

            ImGui::PushID( index );

            if( index != m_editedNameWidgetIndex )
            {
                if( ImGui::Selectable( widget.name.c_str(), isSelected ) == true )
                {
                    m_selectedWidgetIndex = index;
                }

                if( ImGui::BeginPopupContextItem() == true )
                {
                    if( ImGui::MenuItem( "Add" ) == true )
                    {
                        this->newLayout();
                    }

                    if( ImGui::MenuItem( "Remove" ) == true )
                    {
                        this->removeLayout();
                    }

                    ImGui::EndPopup();
                }

                if( ImGui::IsItemHovered() == true && ImGui::IsMouseClicked( ImGuiMouseButton_Left ) == true )
                {
                    m_selectedWidgetIndex = index;
                }
                else if( ImGui::IsItemHovered() == true && ImGui::IsMouseDoubleClicked( ImGuiMouseButton_Left ) == true )
                {
                    m_selectedWidgetIndex = index;
                    m_editedNameWidgetIndex = index;

                    StdString::strcpy( m_editedBufferNameWidget, widget.name.c_str() );
                }
            }
            else
            {
                ImGui::SetKeyboardFocusHere();

                if( ImGui::InputText( "##widget_edit", m_editedBufferNameWidget, 256, ImGuiInputTextFlags_EnterReturnsTrue ) == true )
                {
                    widget.name = m_editedBufferNameWidget;

                    m_editedNameWidgetIndex = ~0;
                }
            }

            ImGui::PopID();
        }

        ImGui::EndChild();
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleLayoutEditor::newLayout()
    {
        Layout layout;

        uint32_t enumerator = m_enumeratorLayout++;

        Char layoutName[256 + 1] = {'\0'};
        MENGINE_SNPRINTF( layoutName, 256, "Layout (%u)", enumerator );

        layout.name = layoutName;

        if( m_selectedLayoutIndex != ~0 )
        {
            m_selectedLayoutIndex++;

            m_layouts.insert( m_layouts.begin() + m_selectedLayoutIndex, layout );
        }
        else
        {
            m_layouts.push_back( layout );

            m_selectedLayoutIndex = m_layouts.size() - 1;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleLayoutEditor::removeLayout()
    {
        if( m_layouts.empty() == true )
        {
            return;
        }

        MENGINE_ASSERTION_FATAL( (int32_t)m_layouts.size() >= m_selectedLayoutIndex, "selected index %u more layouts count %u", m_selectedLayoutIndex, m_layouts.size() );

        m_layouts.erase( m_layouts.begin() + m_selectedLayoutIndex );

        if( m_layouts.empty() == true )
        {
            m_selectedLayoutIndex = ~0;
        }
        else if( m_selectedLayoutIndex >= (int32_t)m_layouts.size() )
        {
            int32_t newSelected = m_layouts.size() - 1;

            m_selectedLayoutIndex = newSelected;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
