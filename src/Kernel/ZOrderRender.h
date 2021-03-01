#pragma once

#include "Kernel/Renderable.h"
#include "Kernel/BaseRender.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ZOrderRender
        : public Renderable
        , public BaseRender
        , public Factorable
    {
        DECLARE_RENDERABLE();

    public:
        ZOrderRender();
        ~ZOrderRender() override;

    public:
        void setZOrder( int32_t _zOrder );
        int32_t getZOrder() const;

    public:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    protected:
        int32_t m_zOrder;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ZOrderRender, RenderInterface> ZOrderRenderPtr;
    //////////////////////////////////////////////////////////////////////////
}