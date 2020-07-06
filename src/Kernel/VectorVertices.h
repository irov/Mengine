#pragma once

#include "Kernel/VectorRenderVertex2D.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class VectorVertices
    {
    public:
        VectorVertices();
        ~VectorVertices();

    public:
        MENGINE_INLINE VectorRenderVertex2D & getVertices() const;
        MENGINE_INLINE void invalidateVertices( uint8_t _invalidate = 0xFE ) const;

    protected:
        virtual void _updateVertices( VectorRenderVertex2D & _vertices, uint8_t _invalidate ) const = 0;

    protected:
        mutable VectorRenderVertex2D m_vertices;
        mutable uint8_t m_invalidateVertices;
    };
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE VectorRenderVertex2D & VectorVertices::getVertices() const
    {
        if( m_invalidateVertices != 0 )
        {
            this->_updateVertices( m_vertices, m_invalidateVertices );
        }

        return m_vertices;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE void VectorVertices::invalidateVertices( uint8_t _invalidate ) const
    {
        m_invalidateVertices |= _invalidate;
    }
    //////////////////////////////////////////////////////////////////////////
}
