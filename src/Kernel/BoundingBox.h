#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/IntrusivePtr.h"

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
        inline const mt::box2f & getBoundingBox() const;

    protected:
        inline void invalidateBoundingBox() const;

    protected:
        void updateBoundingBox() const;

    protected:
        virtual void _updateBoundingBox( mt::box2f & _boundingBox ) const;

    protected:
        mutable mt::box2f m_boundingBox;
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
    inline const mt::box2f & BoundingBox::getBoundingBox() const
    {
        if( m_invalidateBoundingBox == true )
        {
            this->updateBoundingBox();
        }

        return m_boundingBox;
    }
}
