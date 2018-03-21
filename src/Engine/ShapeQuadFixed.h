#   pragma once

#   include "ShapeQuad.h"

namespace Mengine
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