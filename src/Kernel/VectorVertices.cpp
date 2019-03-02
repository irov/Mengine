#include "Kernel/VectorVertices.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    VectorVertices::VectorVertices()
        : m_invalidateVertices( 0xFF )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    VectorVertices::~VectorVertices()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void VectorVertices::updateVertices_()
    {
        this->_updateVertices( m_vertices, m_invalidateVertices );

        m_invalidateVertices = 0;
    }
}
