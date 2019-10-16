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
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<ShapeQuadFixed> ShapeQuadFixedPtr;
    //////////////////////////////////////////////////////////////////////////
}