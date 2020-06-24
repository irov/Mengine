#pragma once

#include "Interface/RenderZOrderInterface.h"

#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class NoneRenderZOrder
        : public RenderZOrderInterface
    {
    public:
        NoneRenderZOrder();
        ~NoneRenderZOrder() override;

    public:
        bool initialize() override;
        void finalize() override;

    public:
        void fetchZOrder( RenderInterface * _render, const RenderContext * _context ) override;

    public:
        void addZOrderRender( int32_t zIndex, int32_t zOrder, const RenderInterface * _render, const RenderContext * _context ) override;

    public:
        void flushZOrderRenderAfter( const RenderPipelineInterfacePtr & _renderPipeline ) override;
        void flushZOrderRenderBefore( const RenderPipelineInterfacePtr & _renderPipeline ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<NoneRenderZOrder> NoneRenderZOrderPtr;
    //////////////////////////////////////////////////////////////////////////
}