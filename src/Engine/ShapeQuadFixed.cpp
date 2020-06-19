#include "ShapeQuadFixed.h"

#include "Kernel/Surface.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ShapeQuadFixed::ShapeQuadFixed()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ShapeQuadFixed::~ShapeQuadFixed()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeQuadFixed::updateVerticesLocal() const
    {
        const mt::vec2f & maxSize = m_surface->getMaxSize();
        const mt::vec2f & size = m_surface->getSize();
        const mt::vec2f & offset = m_surface->getOffset();
        const mt::vec2f & anchor = m_surface->getAnchor();

        mt::vec2f total_offset = offset - anchor * maxSize;

        m_verticesLocal[0].x = total_offset.x + 0.f;
        m_verticesLocal[0].y = total_offset.y + 0.f;

        m_verticesLocal[1].x = total_offset.x + size.x;
        m_verticesLocal[1].y = total_offset.y + 0.f;

        m_verticesLocal[2].x = total_offset.x + size.x;
        m_verticesLocal[2].y = total_offset.y + size.y;

        m_verticesLocal[3].x = total_offset.x + 0.f;
        m_verticesLocal[3].y = total_offset.y + size.y;

        uint32_t uvCount = m_surface->getUVCount();

        for( uint32_t i = 0; i != uvCount; ++i )
        {
            const mt::uv4f & uv = m_surface->getUV( i );

            for( uint32_t v = 0; v != 4; ++v )
            {
                m_verticesWM[v].uv[i] = uv[v];
            }
        }
    }
}