#pragma once

#include "Plugins/ImGUIPlugin/ImGUIInterface.h"

#include "Kernel/ModuleBase.h"
#include "Kernel/Histogram.h"

#ifndef MENGINE_DEBUG_PANEL_HISTOGRAM_COUNT
#define MENGINE_DEBUG_PANEL_HISTOGRAM_COUNT 32
#endif

namespace Mengine
{
    class ModuleDebugPanel
        : public ModuleBase
        , public RenderExternalInterface
    {
        DECLARE_FACTORABLE( ModuleDebugPanel );

    public:
        ModuleDebugPanel();
        ~ModuleDebugPanel() override;

    public:
        bool _initializeModule() override;
        void _finalizeModule() override;

    protected:
        bool _handleKeyEvent( const InputKeyEvent & _event ) override;

    protected:
        void _beginUpdate( bool _focus ) override;

    protected:
        void _render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) override;

    protected:
        void onRenderExternal() const override;

    protected:
        ImGUIRenderProviderInterfacePtr m_imguiRenderProvider;

        Histogram<float, MENGINE_DEBUG_PANEL_HISTOGRAM_COUNT> m_histogramFPS;

        bool m_showDebugPanel;
    };
}