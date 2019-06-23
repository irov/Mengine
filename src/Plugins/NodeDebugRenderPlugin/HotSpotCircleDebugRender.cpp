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
    void HotSpotCircleDebugRender::_render( const RenderContext * _context, HotSpotCircle * _node )
    {
        uint32_t debugColor = Detail::COLOR_IDENTITY_VALUE;

        if( _node->isMousePickerOver() == true )
        {
            debugColor = 0xFFFF0000;
        }

        const mt::mat4f & wm = _node->getWorldMatrix();

        float radius = _node->getRadius();

        Helper::nodeDebugRenderCircle( _context, wm, radius, 32, debugColor );
    }
}