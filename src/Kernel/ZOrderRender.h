#pragma once

#include "Kernel/BaseRender.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ZOrderRender
        : public BaseRender
        , public Factorable
    {
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