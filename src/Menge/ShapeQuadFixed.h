#   pragma once

#   include "ShapeQuad.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    class ShapeQuadFixed
        : public ShapeQuad
    {
    public:
        ShapeQuadFixed();
        ~ShapeQuadFixed();

    protected:
        void updateVerticesLocal() const override;
    };
}