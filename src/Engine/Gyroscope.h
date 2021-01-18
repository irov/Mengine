#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseTransformation.h"

namespace Mengine
{
    class Gyroscope
        : public Node
        , public BaseTransformation
    {
        DECLARE_VISITABLE( Node );
        DECLARE_TRANSFORMABLE();

    public:
        Gyroscope();
        ~Gyroscope() override;

    protected:
        void updateWorldMatrix() const override;
    };
}
