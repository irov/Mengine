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
        inline VectorRenderVertex2D & getVertices();
        inline void invalidateVertices( uint8_t _invalidate = 0xFE );

    protected:
        void updateVertices_();

    protected:
        virtual void _updateVertices( VectorRenderVertex2D & _vertices, uint8_t _invalidate ) = 0;

    private:
        VectorRenderVertex2D m_vertices;
        uint8_t m_invalidateVertices;
    };
    //////////////////////////////////////////////////////////////////////////
    inline VectorRenderVertex2D & VectorVertices::getVertices()
    {
        if( m_invalidateVertices != 0 )
        {
            _updateVertices( m_vertices, m_invalidateVertices );
        }

        return m_vertices;
    }
    //////////////////////////////////////////////////////////////////////////
    inline void VectorVertices::invalidateVertices( uint8_t _invalidate )
    {
        m_invalidateVertices |= _invalidate;
    }
}
