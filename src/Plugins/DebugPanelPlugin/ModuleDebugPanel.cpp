#include "ModuleDebugPanel.h"

#include "Interface/RenderMaterialServiceInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ModuleDebugPanel::ModuleDebugPanel()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ModuleDebugPanel::~ModuleDebugPanel()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleDebugPanel::_initializeModule()
    {
        const ImGUIRenderProviderInterfacePtr & imguiRenderProvider = IMGUI_SERVICE()
            ->getRenderProvider();

        m_imguiRenderProvider = imguiRenderProvider;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleDebugPanel::_finalizeModule()
    {
        m_imguiRenderProvider = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleDebugPanel::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context )
    {
        const RenderMaterialInterfacePtr & renderMaterial = RENDERMATERIAL_SERVICE()
            ->getDebugTriangleMaterial();

        _renderPipeline->addRenderExternal( _context, renderMaterial, nullptr, RenderExternalInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleDebugPanel::onRenderExternal() const
    {
        m_imguiRenderProvider->newFrame();

        ImGui::Begin( "debug panel" );

        ImGui::End();

        m_imguiRenderProvider->endFrame();
    }
    //////////////////////////////////////////////////////////////////////////
}
