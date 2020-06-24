#pragma once

#include "Interface/RenderZOrderInterface.h"

#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class BaseRenderZOrder
        : public RenderZOrderInterface
    {
    public:
        BaseRenderZOrder();
        ~BaseRenderZOrder() override;

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

    protected:
        struct ZRenderObject
        {
            int32_t zIndex;
            int32_t zOrder;
            const RenderInterface * render;
            RenderContext context;
        };

        typedef Vector<ZRenderObject> VectorZRenderObject;
        VectorZRenderObject m_zRenderObjectsAfter;
        VectorZRenderObject m_zRenderObjectsBefore;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<BaseRenderZOrder> BaseRenderZOrderPtr;
    //////////////////////////////////////////////////////////////////////////
}