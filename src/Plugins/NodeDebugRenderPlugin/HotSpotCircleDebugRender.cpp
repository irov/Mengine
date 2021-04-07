#include "HotSpotCircleDebugRender.h"

#include "NodeDebugRenderHelper.h"

#include "Kernel/Color.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    HotSpotCircleDebugRender::HotSpotCircleDebugRender()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    HotSpotCircleDebugRender::~HotSpotCircleDebugRender()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpotCircleDebugRender::_render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, const HotSpotCircle * _node, bool _hide )
    {
        MENGINE_UNUSED( _hide );

        uint32_t debugColor = Detail::COLOR_IDENTITY_VALUE;

        const PickerInterface * picker = _node->getPicker();

        if( picker->isPickerPicked() == true )
        {
            debugColor = 0xFFFF0000;
        }

        const TransformationInterface * transformation = _node->getTransformation();

        const mt::mat4f & wm = transformation->getWorldMatrix();

        float radius = _node->getRadius();

        Helper::nodeDebugRenderCircle( _renderPipeline, _context, wm, radius, 32, debugColor, MENGINE_DOCUMENT_FORWARD );
    }
    //////////////////////////////////////////////////////////////////////////
}