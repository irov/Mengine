#include "ImGUIRender.h"

#include "Interface/RenderMaterialServiceInterface.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx9.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ImGUIRender::ImGUIRender()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ImGUIRender::~ImGUIRender()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ImGUIRender::_activate()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIRender::_deactivate()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIRender::setProvider( const LambdaImGUIProvider & _provider )
    {
        m_provider = _provider;
    }
    //////////////////////////////////////////////////////////////////////////
    const ImGUIRender::LambdaImGUIProvider & ImGUIRender::getProvider() const
    {
        return m_provider;
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIRender::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        const RenderMaterialInterfacePtr & material = RENDERMATERIAL_SERVICE()
            ->getDebugTriangleMaterial();

        _renderPipeline->addRenderExternal( _context, material, nullptr, RenderExternalInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIRender::onRenderExternal() const
    {
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();

        ImGui::NewFrame();

        m_provider();

        ImGui::EndFrame();

        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData( ImGui::GetDrawData() );
    }
}