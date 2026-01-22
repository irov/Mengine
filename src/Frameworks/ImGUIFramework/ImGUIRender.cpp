#include "ImGUIRender.h"

#include "Interface/RenderMaterialServiceInterface.h"

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
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIRender::_deactivate()
    {
        //Empty

        Node::_deactivate();
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
    void ImGUIRender::setImGUIRenderProvider( const ImGUIRenderProviderInterfacePtr & _renderProvider )
    {
        m_renderProvider = _renderProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    const ImGUIRenderProviderInterfacePtr & ImGUIRender::getImGUIRenderProvider() const
    {
        return m_renderProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIRender::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        const RenderMaterialInterfacePtr & material = RENDERMATERIAL_SERVICE()
            ->getDebugTriangleMaterial();

        _renderPipeline->addRenderExternal( _context, material, nullptr, RenderDrawPrimitiveInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    void ImGUIRender::onRenderDrawPrimitives( const RenderPrimitive * _primitives, uint32_t _count ) const
    {
        MENGINE_UNUSED( _primitives );
        MENGINE_UNUSED( _count );

        m_renderProvider->newFrame();

        m_provider( m_renderProvider );

        m_renderProvider->endFrame();
    }
    //////////////////////////////////////////////////////////////////////////
}
