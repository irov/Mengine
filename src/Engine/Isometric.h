#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseUpdation.h"
#include "Kernel/BaseTransformation.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Isometric
        : public Node
        , protected BaseUpdation
        , protected BaseTransformation
    {
        DECLARE_VISITABLE( Node );
        DECLARE_UPDATABLE();
        DECLARE_TRANSFORMABLE();

    public:
        Isometric();
        ~Isometric() override;

    protected:
        void update( const UpdateContext * _context ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<Isometric> IsometricPtr;
    //////////////////////////////////////////////////////////////////////////
}
