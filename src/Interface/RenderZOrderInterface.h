#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/RenderPipelineInterface.h"

#include "Kernel/RenderContext.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderInterface;
    //////////////////////////////////////////////////////////////////////////
    class RenderZOrderInterface
        : public ServantInterface
    {
    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;

    public:
        virtual void fetchZOrder( RenderInterface * _render, const RenderContext * _context ) = 0;

    public:
        virtual void addZOrderRender( int32_t zIndex, int32_t zOrder, const RenderInterface * _render, const RenderContext * _context ) = 0;
        
    public:
        virtual void flushZOrderRenderAfter( const RenderPipelineInterfacePtr & _renderPipeline ) = 0;
        virtual void flushZOrderRenderBefore( const RenderPipelineInterfacePtr & _renderPipeline ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderZOrderInterface> RenderZOrderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}