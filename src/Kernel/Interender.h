#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseRender.h"

namespace Mengine
{
    class Interender
        : public Node
        , public BaseRender
    {
        DECLARE_VISITABLE( Node );
        DECLARE_RENDERABLE();
        
    public:
        Interender();
        ~Interender() override;

    protected:
        void _render( const RenderContext * _context ) override;
    };    
}
