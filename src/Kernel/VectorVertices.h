#pragma once

#include "Kernel/VectorRenderVertex2D.h"

#include "Config/Vector.h"

namespace Mengine
{
    class VectorVertices
    {
    public:
        VectorVertices();
        ~VectorVertices();

    public:
        MENGINE_INLINE VectorRenderVertex2D & getVertices();
        MENGINE_INLINE void invalidateVertices( uint8_t _invalidate = 0xFE );

    protected:
        void updateVertices_();

    protected:
        virtual void _updateVertices( VectorRenderVertex2D & _vertices, uint8_t _invalidate ) = 0;

    private:
        VectorRenderVertex2D m_vertices;
        uint8_t m_invalidateVertices;
    };
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE VectorRenderVertex2D & VectorVertices::getVertices()
    {
        if( m_invalidateVertices != 0 )
        {
            _updateVertices( m_vertices, m_invalidateVertices );
        }

        return m_vertices;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE void VectorVertices::invalidateVertices( uint8_t _invalidate )
    {
        m_invalidateVertices |= _invalidate;
    }
}
