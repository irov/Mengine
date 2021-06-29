#pragma once

#include "Kernel/Mixin.h"

#include "math/box2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class BoundingBox
        : public Mixin
    {
    public:
        BoundingBox();
        ~BoundingBox() override;

    public:
        MENGINE_INLINE const mt::box2f * getBoundingBox() const;

    public:
        MENGINE_INLINE void invalidateBoundingBox() const;

    protected:
        void updateBoundingBox() const;

    protected:
        virtual void _updateBoundingBox( mt::box2f * const _boundingBox, mt::box2f ** const _boundingBoxCurrent ) const;

    protected:
        mutable mt::box2f m_boundingBox;
        mutable mt::box2f * m_boundingBoxCurrent;
        mutable bool m_invalidateBoundingBox;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<BoundingBox> BoundingBoxPtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE void BoundingBox::invalidateBoundingBox() const
    {
        m_invalidateBoundingBox = true;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const mt::box2f * BoundingBox::getBoundingBox() const
    {
        if( m_invalidateBoundingBox == true )
        {
            this->updateBoundingBox();
        }

        return m_boundingBoxCurrent;
    }
    //////////////////////////////////////////////////////////////////////////
}
