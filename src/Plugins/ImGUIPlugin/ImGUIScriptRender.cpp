#include "ImGUIScriptRender.h"

#include "Interface/RenderMaterialServiceInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ImGUIScriptRender::ImGUIScriptRender()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ImGUIScriptRender::~ImGUIScriptRender()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIScriptRender::initialize( const pybind::object & _cb, const pybind::args & _args, const ImGUIRenderProviderInterfacePtr & _imguiRenderProvider )
    {
        m_cb = _cb;
        m_args = _args;

        m_imguiRenderProvider = _imguiRenderProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIScriptRender::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        const RenderMaterialInterfacePtr & material = RENDERMATERIAL_SERVICE()
            ->getDebugTriangleMaterial();

        _renderPipeline->addRenderExternal( _context, material, nullptr, RenderDrawPrimitiveInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIScriptRender::onRenderDrawPrimitives( const RenderPrimitive * _primitives, uint32_t _count ) const
    {
        MENGINE_UNUSED( _primitives );
        MENGINE_UNUSED( _count );

        m_imguiRenderProvider->newFrame();

        m_cb.call_args( m_args );

        m_imguiRenderProvider->endFrame();
    }
    //////////////////////////////////////////////////////////////////////////
}