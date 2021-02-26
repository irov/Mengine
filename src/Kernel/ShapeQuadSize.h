#pragma once

#include "ShapeQuad.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ShapeQuadSize
        : public ShapeQuad
    {
    public:
        ShapeQuadSize();
        ~ShapeQuadSize() override;

    public:
        void setSize( const mt::vec2f & _size );
        const mt::vec2f & getSize() const;

    protected:
        void updateVerticesLocal() const override;

    protected:
        AnimationInterface * getAnimation() override;
        const AnimationInterface * getAnimation() const override;

    protected:
        EventationInterface * getEventation() override;
        const EventationInterface * getEventation() const override;

    protected:
        mt::vec2f m_size;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<ShapeQuadSize> ShapeQuadSizePtr;
    //////////////////////////////////////////////////////////////////////////
}