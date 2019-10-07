#pragma once

#include "Kernel/Node.h"

namespace Mengine
{
    class Layer
        : public Node
    {
        DECLARE_VISITABLE( Node );

    public:
        Layer();
        ~Layer() override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<Layer> LayerPtr;
    //////////////////////////////////////////////////////////////////////////
}
