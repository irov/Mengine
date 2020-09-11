#pragma once

#include "ShapeQuad.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ShapeQuadFixed
        : public ShapeQuad
    {
    public:
        ShapeQuadFixed();
        ~ShapeQuadFixed() override;

    protected:
        void updateVerticesLocal() const override;

    protected:
        AnimationInterface * getAnimation() override;
        const AnimationInterface * getAnimation() const override;

    protected:
        EventationInterface * getEventation() override;
        const EventationInterface * getEventation() const override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<ShapeQuadFixed> ShapeQuadFixedPtr;
    //////////////////////////////////////////////////////////////////////////
}