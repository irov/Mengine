#include "HotSpotSurfaceDebugRender.h"

#include "NodeDebugRenderHelper.h"

#include "Interface/RenderServiceInterface.h"

#include "Kernel/Color.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    HotSpotSurfaceDebugRender::HotSpotSurfaceDebugRender()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    HotSpotSurfaceDebugRender::~HotSpotSurfaceDebugRender()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpotSurfaceDebugRender::_render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, HotSpotSurface * _node, bool _hide )
    {
        MENGINE_UNUSED( _hide );

        VectorRenderVertex2D & vertices = RENDER_SERVICE()
            ->getDebugRenderVertex2D( 8 );

        uint32_t debugColor = Detail::COLOR_IDENTITY_VALUE;

        PickerInterface * picker = _node->getPicker();

        if( picker->isPickerPicked() == true )
        {
            debugColor = 0xFF22FF00;
        }

        mt::box2f box;
        _node->getWorldBoundingBox( &box );

        vertices[0].position.x = box.minimum.x;
        vertices[0].position.y = box.minimum.y;
        vertices[1].position.x = box.maximum.x;
        vertices[1].position.y = box.minimum.y;
        vertices[2].position.x = box.maximum.x;
        vertices[2].position.y = box.minimum.y;
        vertices[3].position.x = box.maximum.x;
        vertices[3].position.y = box.maximum.y;
        vertices[4].position.x = box.maximum.x;
        vertices[4].position.y = box.maximum.y;
        vertices[5].position.x = box.minimum.x;
        vertices[5].position.y = box.maximum.y;
        vertices[6].position.x = box.minimum.x;
        vertices[6].position.y = box.maximum.y;
        vertices[7].position.x = box.minimum.x;
        vertices[7].position.y = box.minimum.y;

        for( uint32_t i = 0; i != 8; ++i )
        {
            vertices[i].position.z = 0.f;

            vertices[i].color = debugColor;
            vertices[i].uv[0].x = 0.f;
            vertices[i].uv[0].y = 0.f;
            vertices[i].uv[1].x = 0.f;
            vertices[i].uv[1].y = 0.f;
        }

        Helper::nodeDebugRenderLine( _renderPipeline, _context, vertices, MENGINE_DOCUMENT_FORWARD );
    }
    //////////////////////////////////////////////////////////////////////////
}