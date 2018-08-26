#include "Kernel/BoundingBox.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BoundingBox::BoundingBox()
        : m_boundingBox( 0.f, 0.f, 0.f, 0.f )
        , m_invalidateBoundingBox( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    BoundingBox::~BoundingBox()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void BoundingBox::updateBoundingBox() const
    {
        m_invalidateBoundingBox = false;

        this->_updateBoundingBox( m_boundingBox );
    }
    //////////////////////////////////////////////////////////////////////////
    void BoundingBox::_updateBoundingBox( mt::box2f & _boundingBox ) const
    {
        (void)_boundingBox;
        //Empty
    }
}
