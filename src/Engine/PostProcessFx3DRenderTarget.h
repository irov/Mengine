#pragma once

#include "Kernel/Factorable.h"
#include "Kernel/Renderable.h"
#include "Kernel/BaseRender.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PostProcessFx3D;
    //////////////////////////////////////////////////////////////////////////
    class PostProcessFx3DRenderTarget
        : public Factorable
        , public Renderable
        , protected BaseRender
    {
        DECLARE_FACTORABLE( PostProcessFx3DRenderTarget );
        DECLARE_RENDERABLE();

    public:
        explicit PostProcessFx3DRenderTarget( PostProcessFx3D * _owner );
        ~PostProcessFx3DRenderTarget() override;

    protected:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    protected:
        PostProcessFx3D * m_owner;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PostProcessFx3DRenderTarget> PostProcessFx3DRenderTargetPtr;
    //////////////////////////////////////////////////////////////////////////
}
