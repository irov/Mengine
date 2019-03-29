#include "HotSpotPolygonDebugRender.h"

#include "NodeDebugRenderHelper.h"

#include "Interface/RenderServiceInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    HotSpotPolygonDebugRender::HotSpotPolygonDebugRender()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    HotSpotPolygonDebugRender::~HotSpotPolygonDebugRender()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void HotSpotPolygonDebugRender::_render( const RenderContext * _context, HotSpotPolygon * _node )
    {
        const Polygon & polygon = _node->getPolygon();

        uint32_t numpoints = polygon.size();

        if( numpoints == 0 )
        {
            return;
        }

        uint32_t vertexCount = numpoints * 2;

        VectorRenderVertex2D & vertices = RENDER_SERVICE()
            ->getDebugRenderVertex2D( vertexCount );

        uint32_t debugColor = Detail::COLOR_IDENTITY_VALUE;

        if( _node->isMousePickerOver() == true )
        {
            debugColor = 0xFFFF0000;
        }

        const mt::mat4f & wm = _node->getWorldMatrix();

        const VectorPoints & ring = polygon.getPoints();

        for( uint32_t i = 0; i != numpoints; ++i )
        {
            uint32_t j = (i + 1) % numpoints;

            mt::vec3f trP0;
            mt::mul_v3_v2_m4( trP0, ring[i], wm );

            RenderVertex2D & v0 = vertices[i * 2 + 0];

            v0.position = trP0;

            v0.color = debugColor;

            for( uint32_t uv_index = 0; uv_index != MENGINE_RENDER_VERTEX_UV_COUNT; ++uv_index )
            {
                v0.uv[uv_index].x = 0.f;
                v0.uv[uv_index].y = 0.f;
            }

            mt::vec3f trP1;
            mt::mul_v3_v2_m4( trP1, ring[j], wm );

            RenderVertex2D & v1 = vertices[i * 2 + 1];

            v1.position = trP1;

            v1.color = debugColor;

            for( uint32_t uv_index = 0; uv_index != MENGINE_RENDER_VERTEX_UV_COUNT; ++uv_index )
            {
                v1.uv[uv_index].x = 0.f;
                v1.uv[uv_index].y = 0.f;
            }
        }

        Helper::nodeDebugRenderLine( _context, vertices );
    }
}