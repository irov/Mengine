#include "NodeDebugRenderVisitor.h"

#include "Interface/RenderServiceInterface.h"

#include "Engine/HotSpotPolygon.h"

#include "Kernel/RenderVertex2D.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    NodeDebugRenderVisitor::NodeDebugRenderVisitor()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    NodeDebugRenderVisitor::~NodeDebugRenderVisitor()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugRenderVisitor::accept( HotSpotCircle * _node )
    {
        uint32_t debugColor = 0xFFFFFFFF;

        if( _node->isMousePickerOver() == true )
        {
            debugColor = 0xFFFF0000;
        }

        const mt::mat4f & wm = _node->getWorldMatrix();

        float radius = _node->getRadius();

        this->renderCircle_( wm, radius, 32, debugColor );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugRenderVisitor::accept( HotSpotPolygon * _node )
    {
        const Polygon & polygon = _node->getPolygon();

        uint32_t numpoints = polygon.size();

        if( numpoints == 0 )
        {
            return;
        }

        uint32_t vertexCount = numpoints * 2;

        RenderVertex2D * vertices = RENDER_SERVICE()
            ->getDebugRenderVertex2D( vertexCount );

        if( vertices == nullptr )
        {
            return;
        }

        uint32_t debugColor = 0xFFFFFFFF;

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

        this->renderLine_( vertices, vertexCount );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugRenderVisitor::accept( HotSpotImage * _node )
    {
        RenderVertex2D * vertices = RENDER_SERVICE()
            ->getDebugRenderVertex2D( 4 * 2 );

        if( vertices == nullptr )
        {
            return;
        }

        uint32_t debugColor = 0xFFFFFFFF;

        if( _node->isMousePickerOver() == true )
        {
            debugColor = 0xFFFF0000;
        }

        const mt::box2f & box = _node->getBoundingBox();

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

        this->renderLine_( vertices, 8 );
    }
    ////////////////////////////////////////////////////////////////////////////
    //void NodeDebugRenderVisitor::accept( MovieSlot * _node )
    //{
    //    const mt::mat4f & wm = _node->getWorldMatrix();

    //    float radius = 10.f;
    //    this->renderCircle_( wm, radius, 16, 0x000000FF );
    //}
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugRenderVisitor::accept( Point * _node )
    {
        const mt::mat4f & wm = _node->getWorldMatrix();

        float radius = 2.5f;
        this->renderCircle_( wm, radius, 4, 0x000000FF );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugRenderVisitor::accept( Arrow * _node )
    {
        const mt::mat4f & wm = _node->getWorldMatrix();

        EArrowType arrowType = _node->getArrowType();

        switch( arrowType )
        {
        case EAT_POINT:
            {
                float radius = 2.5f;
                this->renderCircle_( wm, radius, 4, 0x8080FFFF );

                return;
            }break;
        case EAT_RADIUS:
            {
                float radius = _node->getRadius();

                this->renderCircle_( wm, radius, 32, 0x8080FFFF );
            }break;
        case EAT_POLYGON:
            {
                const Polygon & polygon = _node->getPolygon();

                this->renderPolygon_( wm, polygon, 0x8080FFFF );
            }break;
        }
    }
}