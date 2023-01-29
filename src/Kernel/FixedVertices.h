#pragma once

#include "Kernel/RenderVertex2D.h"
#include "Kernel/MemoryCopy.h"

namespace Mengine
{
    template<uint32_t VerticesCount>
    class FixedVertices
    {
    public:
        FixedVertices()
            : m_invalidateVertices( 0xFF )
        {
        }

    public:
        void setVertices( const RenderVertex2D * _vertices )
        {
            Helper::memoryCopy( m_vertices, 0, _vertices, VerticesCount );
        }

        MENGINE_INLINE const RenderVertex2D * getVertices() const
        {
            if( m_invalidateVertices != 0 )
            {
                this->_updateVertices( m_vertices, m_invalidateVertices );

                m_invalidateVertices = 0;
            }

            return m_vertices;
        }

        MENGINE_INLINE void invalidateVertices( uint8_t _invalidate = 0xFF ) const
        {
            m_invalidateVertices |= _invalidate;
        }

    protected:
        virtual void _updateVertices( RenderVertex2D * const _vertices, uint8_t _invalidateVertices ) const = 0;

    protected:
        mutable RenderVertex2D m_vertices[VerticesCount];
        mutable uint8_t m_invalidateVertices;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef FixedVertices<4> QuadVertices;
    //////////////////////////////////////////////////////////////////////////
}
