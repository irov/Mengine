#include "Kernel/BoundingBox.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    constexpr float bounding_max = (std::numeric_limits<float>::max)();
    //////////////////////////////////////////////////////////////////////////
    BoundingBox::BoundingBox()
        : m_boundingBox( -bounding_max, -bounding_max, bounding_max, bounding_max )
        , m_boundingBoxCurrent( nullptr )
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

        this->_updateBoundingBox( &m_boundingBox, &m_boundingBoxCurrent );
    }
    //////////////////////////////////////////////////////////////////////////
    void BoundingBox::_updateBoundingBox( mt::box2f * const _boundingBox, mt::box2f ** const _boundingBoxCurrent ) const
    {
        MENGINE_UNUSED( _boundingBox );
        MENGINE_UNUSED( _boundingBoxCurrent );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}
