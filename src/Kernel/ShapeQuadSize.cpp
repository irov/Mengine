#include "ShapeQuadSize.h"

#include "Kernel/Surface.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ShapeQuadSize::ShapeQuadSize()
        : m_size( 0.f, 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ShapeQuadSize::~ShapeQuadSize()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeQuadSize::setSize( const mt::vec2f & _size )
    {
        m_size = _size;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & ShapeQuadSize::getSize() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeQuadSize::updateVerticesLocal() const
    {
        const mt::vec2f & maxSize = m_surface->getMaxSize();
        const mt::vec2f & size = m_surface->getSize();
        const mt::vec2f & offset = m_surface->getOffset();
        const mt::vec2f & anchor = m_surface->getAnchor();

        mt::vec2f total_offset = offset - anchor * maxSize;
        mt::vec2f scale = m_size / maxSize;

        m_verticesLocal[0].x = scale.x * (total_offset.x + 0.f);
        m_verticesLocal[0].y = scale.y * (total_offset.y + 0.f);

        m_verticesLocal[1].x = scale.x * (total_offset.x + size.x);
        m_verticesLocal[1].y = scale.y * (total_offset.y + 0.f);

        m_verticesLocal[2].x = scale.x * (total_offset.x + size.x);
        m_verticesLocal[2].y = scale.y * (total_offset.y + size.y);

        m_verticesLocal[3].x = scale.x * (total_offset.x + 0.f);
        m_verticesLocal[3].y = scale.y * (total_offset.y + size.y);

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
    //////////////////////////////////////////////////////////////////////////
    AnimationInterface * ShapeQuadSize::getAnimation()
    {
        const SurfacePtr & surface = this->getSurface();

        if( surface == nullptr )
        {
            return nullptr;
        }

        AnimationInterface * animation = surface->getAnimation();

        return animation;
    };
    //////////////////////////////////////////////////////////////////////////
    const AnimationInterface * ShapeQuadSize::getAnimation() const
    {
        const SurfacePtr & surface = this->getSurface();

        if( surface == nullptr )
        {
            return nullptr;
        }

        const AnimationInterface * animation = surface->getAnimation();

        return animation;
    };
    //////////////////////////////////////////////////////////////////////////
    EventationInterface * ShapeQuadSize::getEventation()
    {
        const SurfacePtr & surface = this->getSurface();

        if( surface == nullptr )
        {
            return nullptr;
        }

        EventationInterface * eventation = surface->getEventation();

        return eventation;
    };
    //////////////////////////////////////////////////////////////////////////
    const EventationInterface * ShapeQuadSize::getEventation() const
    {
        const SurfacePtr & surface = this->getSurface();

        if( surface == nullptr )
        {
            return nullptr;
        }

        const EventationInterface * eventation = surface->getEventation();

        return eventation;
    };
    //////////////////////////////////////////////////////////////////////////
}