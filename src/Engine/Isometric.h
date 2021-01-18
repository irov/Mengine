#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseUpdation.h"

namespace Mengine
{
    class Isometric
        : public Node
        , protected BaseUpdation
    {
        DECLARE_VISITABLE( Node );
        DECLARE_UPDATABLE();

    public:
        Isometric();
        ~Isometric() override;

    protected:
        void update( const UpdateContext * _context ) override;
    };
}
