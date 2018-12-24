#pragma once

#include "Kernel/Mixin.h"

#include "math/box2.h"

namespace Mengine
{
    class BoundingBox
        : public Mixin
    {
    public:
        BoundingBox();
        ~BoundingBox();

    public:
        inline const mt::box2f * getBoundingBox() const;

    public:
        inline void invalidateBoundingBox() const;

    protected:
        void updateBoundingBox() const;

    protected:
        virtual void _updateBoundingBox( mt::box2f & _boundingBox, mt::box2f ** _boundingBoxCurrent ) const;

    protected:
        mutable mt::box2f m_boundingBox;
        mutable mt::box2f * m_boundingBoxCurrent;
        mutable bool m_invalidateBoundingBox;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<BoundingBox> BoundingBoxPtr;
    //////////////////////////////////////////////////////////////////////////
    inline void BoundingBox::invalidateBoundingBox() const
    {
        m_invalidateBoundingBox = true;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const mt::box2f * BoundingBox::getBoundingBox() const
    {
        if( m_invalidateBoundingBox == true )
        {
            this->updateBoundingBox();
        }

        return m_boundingBoxCurrent;
    }
}
