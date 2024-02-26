#pragma once

#include "Kernel/Factorable.h"
#include "Kernel/Renderable.h"
#include "Kernel/BaseRender.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderRoot
        : public Factorable
        , public Renderable
        , protected BaseRender
    {
        DECLARE_FACTORABLE( RenderRoot );        
        DECLARE_RENDERABLE();

    public:
        RenderRoot();
        ~RenderRoot() override;

    protected:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderRoot> RenderRootPtr;
    //////////////////////////////////////////////////////////////////////////
}