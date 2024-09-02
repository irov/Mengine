#include "Layer2DRenderTarget.h"

#include "Layer2D.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Layer2DRenderTarget::Layer2DRenderTarget( Layer2D * _layer )
        : m_layer( _layer )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Layer2DRenderTarget::~Layer2DRenderTarget()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Layer2DRenderTarget::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        const RenderVertex2D * verticesImageMask = m_layer->getVerticesImageMaskWM();
        const RenderMaterialInterfacePtr & materialImageMask = m_layer->getMaterialImageMask();

        const RenderInterface * render = m_layer->getRender();

        const mt::box2f * bb = render->getBoundingBox();

        _renderPipeline->addRenderQuad( _context, materialImageMask, verticesImageMask, 4, bb, false, MENGINE_DOCUMENT_FORWARD );
    }
    //////////////////////////////////////////////////////////////////////////
}