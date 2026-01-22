#pragma once

#include "Frameworks/ImGUIFramework/ImGUIInterface.h"

#include "Kernel/ModuleBase.h"
#include "Kernel/DummySceneEventReceiver.h"
#include "Kernel/Scene.h"
#include "Kernel/Histogram.h"
#include "Kernel/String.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    class ModuleLayoutEditor
        : public ModuleBase
        , public DummySceneEventReceiver
        , public RenderDrawPrimitiveInterface
    {
        DECLARE_FACTORABLE( ModuleLayoutEditor );

    public:
        ModuleLayoutEditor();
        ~ModuleLayoutEditor() override;

    public:
        bool _initializeModule() override;
        void _finalizeModule() override;

    protected:
        bool _handleKeyEvent( const InputKeyEvent & _event ) override;

    protected:
        void _render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) override;

    public:
        bool onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity ) override;
        void onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onEntityActivate( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior ) override;

    protected:
        void onRenderDrawPrimitives( const RenderPrimitive * _primitives, uint32_t _count ) const override;

    protected:
        void renderImGUI();
        void renderImGUILayouts();
        void renderImGUIWidgets();

    protected:
        void newLayout();
        void removeLayout();

    protected:
        ScenePtr m_fromScene;

        Scene * m_scene;

        ImGUIRenderProviderInterfacePtr m_imguiRenderProvider;

        struct Widget
        {
            String name;
        };

        struct Layout
        {
            String name;

            typedef Vector<Widget> VectorWidgets;
            VectorWidgets widgets;
        };

        typedef Vector<Layout> VectorLayout;
        VectorLayout m_layouts;

        uint32_t m_enumeratorLayout;

        int32_t m_selectedLayoutIndex;
        int32_t m_editedNameLayoutIndex;
        Char m_editedBufferNameLayout[256 + 1];

        int32_t m_selectedWidgetIndex;
        int32_t m_editedNameWidgetIndex;
        Char m_editedBufferNameWidget[256 + 1];

        bool m_show;
    };
}