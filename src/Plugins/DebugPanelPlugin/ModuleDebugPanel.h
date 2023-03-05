#pragma once

#include "Plugins/ImGUIPlugin/ImGUIInterface.h"

#include "Kernel/ModuleBase.h"

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
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) override;

    protected:
        void onRenderExternal() const override;

    protected:
        ImGUIRenderProviderInterfacePtr m_imguiRenderProvider;
    };
}