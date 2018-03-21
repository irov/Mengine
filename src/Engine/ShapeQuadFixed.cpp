#   include "ShapeQuadFixed.h"

#   include "Kernel/Surface.h"

#include "Logger/Logger.h"

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
        const mt::vec2f & size = m_surface->getSize();
        const mt::vec2f & offset = m_surface->getOffset();

        m_verticesLocal[0].x = offset.x + 0.f;
        m_verticesLocal[0].y = offset.y + 0.f;

        m_verticesLocal[1].x = offset.x + size.x;
        m_verticesLocal[1].y = offset.y + 0.f;

        m_verticesLocal[2].x = offset.x + size.x;
        m_verticesLocal[2].y = offset.y + size.y;

        m_verticesLocal[3].x = offset.x + 0.f;
        m_verticesLocal[3].y = offset.y + size.y;

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